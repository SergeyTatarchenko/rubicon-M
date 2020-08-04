/************************************************************************
* File Name          : user_tasks.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : RTOS high level routines
*************************************************************************/

/*----------------------------------------------------------------------*/
#include "user_tasks.h"
/*----------------------------------------------------------------------*/
const int tick_overload = 3600;
DEVICE_MODE mode = NORMAL;


/* name: USART3_IRQHandler
*  descriprion: interrupt handler for service serial port communication
*/
void USART3_IRQHandler()
{
	if(USART3->SR &= USART_SR_RXNE)
	{
		reflection_byte = USART3->DR;
	}
}


/* name: _task_led
*  descriprion: indication control procedure
*/
void _task_led(void *pvParameters)
{
//	static int tick = 0;
	for(;;)
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
	
	for(;;)
    {
		/*update all hw states*/
        GetHwAdrState(&ADRESS);
        GetHwModeState(&MODE);
        GetHwOutState(&OUTPUTS);
        vTaskDelay(10);
    }
}

/* name: _task_service_serial
*  descriprion: service serial port input/output data processing
*/
void _task_service_serial(void *pvParameters)
{
	static char received_word = 0;
	static int tick = 0;
	static int word_counter = 0;
	TCmdTypeDef input_command;
	for(;;)
	{
		tick++;
		received_word = reflection_byte;
		/*input command reflection for user feedback*/
		if(received_word != 0)
		{
			serial_send_byte(serial_pointer,received_word);
		}
		if((mode == DEBUG) &&(received_word != '\r'))
		{
			serial_debug_output();
		}
		if((mode == DEBUG) &&(received_word == '\r'))
		{
			mode = NORMAL;
		}
		if((received_word == '\r') && (mode == NORMAL))
		{
			/*enter to a programming mode*/
			mode = PROGRAMMING_SS;
			cprintf(__POSLINE);
			cprintf("Система контроля периметра <<РУБИКОН>>\n\r");
			cprintf("Версия программного обеспечения: ");
			cprintf(VERSION);
			cprintf("\n\r");
			cprintf("Для справки введите help и нажмите Enter\n\r");
			cprintf("Для выхода введите exit и нажмите Enter\n\r");
			cprintf(__POSLINE);
			reflection_byte = 0;
			received_word = 0;
		}
		if((mode == PROGRAMMING_SS)&& (reflection_byte != 0))
		{
			reflection_byte = 0;
			if(received_word != '\r')
			{
				/*input buffer oferflow error*/
				if(word_counter == COMMAND_BUF_SIZE)
				{
					word_counter = 0;
					memset(combuff,0,COMMAND_BUF_SIZE);
					cprintf(__NEWLINE);
					cprintf(__ERROR_MESSAGE);
					cprintf(__NEWLINE);
				}
				else
				{
					combuff[word_counter] = received_word;
					/*word counter increment*/
					word_counter++;
				}
			}
			else if(received_word == '\r')
			{
				word_counter = 0;
				input_command = command_processing(combuff);
				serial_command_executor(input_command);
				memset(combuff,0,COMMAND_BUF_SIZE);
				word_counter = 0;
			}
		}
		else if(reflection_byte != 0)
		{
			reflection_byte = 0;
		}
		/*send header message every 2s to terminal*/
		if((mode == NORMAL) && tick %40 == 0)
		{
			tick = 0;
			cprintf(__NEWLINE);
			cprintf(__HEADER_MESSAGE);
		}
		vTaskDelay(50);
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
		/*save current config to flash*/
		case C_SAVE:
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
