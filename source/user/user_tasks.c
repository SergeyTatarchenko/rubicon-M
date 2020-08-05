/************************************************************************
* File Name          : user_tasks.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : RTOS high level routines
*************************************************************************/

/*----------------------------------------------------------------------*/
#include "user_tasks.h"
/*----------------------------------------------------------------------*/
DEVICE_MODE mode = NORMAL;


/* name: USART3_IRQHandler
*  descriprion: interrupt handler for service serial port communication
*/
void USART3_IRQHandler()
{
	static uint8_t counter = 0;
	static char buffer[COMMAND_BUF_SIZE] = {0};
	char byte;
	
	/*data exist in DR register*/
	if(USART3->SR &= USART_SR_RXNE)
	{
		byte = USART3->DR;
		if(byte != '\r')
		{
			//xQueueSendFromISR(service_serial_reflection,&byte,0);
			/*input buffer oferflow error*/
			if(counter == COMMAND_BUF_SIZE)
			{
				counter = 0;
				memset(buffer,0,COMMAND_BUF_SIZE);
			}
			else
			{
				buffer[counter] = byte;
				/*word counter increment*/
				counter++;
			}
		}
		else if(byte == '\r')
		{
			/*mode switching*/
			switch(mode)
			{
				case NORMAL:
					mode = PROGRAMMING_SS;
					break;
				case DEBUG:
					mode = PROGRAMMING_SS;
					break;
				case PROGRAMMING_SS:
					counter = 0;
					/*send data to queue (command extracted in _task_service_serial function)*/
					xQueueSendFromISR(service_serial_queue,buffer,0);
					memset(buffer,0,COMMAND_BUF_SIZE);
					break;
				default:
					break;
			}
		}
	}
}

/* name: _task_service_mirror
*  descriprion: reflects received bytes to the serial port (rendering of manual input, can be disabled)
*/
void _task_service_mirror(void *pvParameters)
{
	char buffer;
	while(TRUE)
	{
		xQueueReceive(service_serial_reflection,&buffer,portMAX_DELAY);
		serial_send_byte(serial_pointer,buffer);
	}
}

/* name: _task_service_serial
*  descriprion: service serial port input/output data processing
*/
void _task_service_serial(void *pvParameters)
{
	const int tick_overload = 3600;
	static uint8_t trigger = FALSE;
	static int tick = 0;
	TCmdTypeDef input_command;
	char buffer[COMMAND_BUF_SIZE] = {0};
	while(TRUE)
	{
		if(tick > tick_overload)
		{
			tick = 0;
		}
		else
		{
			tick++;
		}
		
		/*debug ADC output*/
		if(mode == DEBUG)
		{
			serial_debug_output();
		}
		/*print welcome message in programming mode*/
		if((trigger == FALSE) && (mode == PROGRAMMING_SS))
		{
			trigger = TRUE;
			serial_print_welcome();
		}
		/*command buffer not empty*/
		if(uxQueueMessagesWaiting( service_serial_queue ) != 0)
		{
			xQueueReceive(service_serial_queue,&buffer,portMAX_DELAY);
			input_command = command_processing(buffer);
			
			/*trigger reset*/
			if(input_command.command == C_EXIT)
			{
				trigger = FALSE;
			}
			serial_command_executor(input_command);
		}
		/*send header message every 2s to terminal*/
		if((mode == NORMAL) && tick %40 == 0)
		{
			cprintf(__NEWLINE);
			cprintf(__HEADER_MESSAGE);
		}
		vTaskDelay(50);
	}
}

/* name: _task_led
*  descriprion: indication control procedure
*/
void _task_led(void *pvParameters)
{
//	static int tick = 0;
	while(TRUE)
	{
/*		if(tick < tick_overload)
		{
			tick++;
		}
		else
		{
		tick = 0;
		}
*/		
		sync_led_off;
		vTaskDelay(200);
		sync_led_on;
		vTaskDelay(200);
	}
}

/* name: _task_state_update
*  descriprion: main routine task, current state handling
*/
void _task_state_update(void *pvParameters)
{
	/*load configuration data*/
	flash_data_read(CONFIG_FLASH_ADDRESS,(uint32_t*)(&CONFIG),sizeof(CONFIG));
	
	while(TRUE)
    {
		/*update all hw states*/
        GetHwAdrState(&ADRESS);
        GetHwModeState(&MODE);
        GetHwOutState(&OUTPUTS);
        vTaskDelay(10);
    }
}



/* name: serial_command_executor
*  descriprion: execute commands from terminal in service serial task
*/
void serial_command_executor (TCmdTypeDef command)
{
	switch(command.command)
	{
		/*exit from programming mode*/
		case C_EXIT:
			mode = NORMAL;
			cprintf(__OUT_MESSAGE);
			cprintf(__POSLINE);
		break;
		/*print help textblock*/
		case C_HELP:
			break;
		/*save current config to flash
		WARNING!!! CPU stop during flash programming*/
		case C_SAVE:
			flash_data_write(CONFIG_FLASH_ADDRESS,CONFIG_SECTOR_NUMBER,CONFIG.array,sizeof(CONFIG));
			cprintf("ok\r\n");
			break;
		/*show something (see argument)*/
		case C_SHOW:
			switch(command.argument)
			{
				case A_ADDRESS:
					serial_print_adress();
					break;
				case A_MODE:
					serial_print_mode();
					break;
				case A_STATE:
					serial_print_state();
					break;
				case A_CONFIG:
					serial_print_config();
					break;
				default:
					cprintf(__ERROR_MESSAGE);
					cprintf(__NEWLINE);
					break;
			}
			break;
		/*set value for some parameter (see argument)*/
		case C_SET:
			switch(command.argument)
			{
				case A_TIMEINT1:
					if(command.value != 0)
					{
						CONFIG.data.zone_0_timeint = command.value;
						cprintf("ok\r\n");
					}
					else
					{
						cprintf(__ERROR_MESSAGE);
						cprintf(__NEWLINE);
					}
					break;
				default:
					cprintf(__ERROR_MESSAGE);
					cprintf(__NEWLINE);
					break;
			}
			break;
		case C_DEBUG:
			mode = DEBUG;
			cprintf(__POSLINE);
			cprintf("current adc values : \r\n");
			break;
		/*error in input*/
		case C_ERROR:
			cprintf(__ERROR_MESSAGE);
			cprintf(__NEWLINE);
			break;
		default:
			break;
	}
}

void serial_debug_output( void )
{
	itoa(ADC_VALUES.alrm_0,ADC_CHANNELS.ch_1,4);
	itoa(ADC_VALUES.alrm_1,ADC_CHANNELS.ch_2,4);
	itoa(ADC_VALUES.sign_0,ADC_CHANNELS.ch_4,4);
	itoa(ADC_VALUES.sign_1,ADC_CHANNELS.ch_5,4);
	itoa(ADC_VALUES.sign_2,ADC_CHANNELS.ch_6,4);
	itoa(ADC_VALUES.sign_3,ADC_CHANNELS.ch_7,4);
	for(int i = 0;i < 4; i++)
	{
		ADC_CHANNELS.ch_1[i]+=0x30;
		ADC_CHANNELS.ch_2[i]+=0x30;
		ADC_CHANNELS.ch_4[i]+=0x30;
		ADC_CHANNELS.ch_5[i]+=0x30;
		ADC_CHANNELS.ch_6[i]+=0x30;
		ADC_CHANNELS.ch_7[i]+=0x30;
	}
	cprintf(" adc ch 1 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_1[i]);
	}
	cprintf(" adc ch 2 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_2[i]);
	}
	cprintf(" adc ch 4 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_4[i]);
	}
	cprintf(" adc ch 5 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_5[i]);
	}
	cprintf(" adc ch 6 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_6[i]);
	}
	cprintf(" adc ch 7 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_7[i]);
	}
	cprintf("\r");
	
}

/****************************end of file ********************************/
