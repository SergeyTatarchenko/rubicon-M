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

/*value from CONFIG struct converted from mV*/
static uint16_t zone_0_treshold = 0;
static uint16_t zone_1_treshold = 0;

static uint8_t test_buffer[COMMAND_BUF_SIZE] = {0};
/* name: USART6_IRQHandler
*  descriprion: interrupt handler for RS 485 port communication
*/
void USART6_IRQHandler()
{
	static uint8_t counter = 0;
	static char buffer[COMMAND_BUF_SIZE] = {0};
	char byte;
	/*data exist in DR register*/
	if(USART6->SR &= USART_SR_RXNE)
	{
		byte = USART6->DR;
		if(byte != '\r')
		{
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
				case ALARM:
					mode = PROGRAMMING_SS;
					break;
				case PROGRAMMING_SS:
					/*send data to queue (command extracted in _task_service_serial function)*/
					memcpy(test_buffer,buffer,COMMAND_BUF_SIZE);
					xQueueSendFromISR(service_serial_queue,buffer,0);
					memset(buffer,0,COMMAND_BUF_SIZE);
					break;
				default:
					break;
			}
			counter = 0;
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
	static int a = 0;
	const int tick_overload = 3600;
	static uint8_t trigger = FALSE;
	static int tick = 0;
	TCmdTypeDef input_command;
	char buffer[COMMAND_BUF_SIZE] = {0};
	xMutex_serial_BUSY = xSemaphoreCreateMutex();
	
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
			mprintf("input command: ");
			mprintf(buffer);
			mprintf(__NEWLINE);
			input_command = command_processing(buffer);
			
			if(input_command.command == C_EXIT)
			{
				trigger = FALSE;
			}
			serial_command_executor(input_command);
		}
		/*send header message every 2s to terminal*/
		if((mode == NORMAL ||mode == ALARM) && tick %40 == 0)
		{
			mprintf(__NEWLINE);
			mprintf(__HEADER_MESSAGE);
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
	static int tick = 0;
	static const int tick_overload = 99999;
	
	/*load configuration data*/
	flash_data_read(CONFIG_FLASH_ADDRESS,(uint32_t*)(&CONFIG),sizeof(CONFIG));
	
	zone_0_treshold = adc_covert_from_mv(CONFIG.data.zone_0_treshold);
	zone_1_treshold = adc_covert_from_mv(CONFIG.data.zone_1_treshold);
	
	while(TRUE)
    {
		if(tick > tick_overload )
		{
			tick = 0;
		}
		else
		{
			tick++;
		}
		
		if(tick%20 == 0)
		{
			/*update all hw states*/
			GetHwAdrState(&ADRESS);
			GetHwModeState(&MODE);
			GetHwOutState(&OUTPUTS);
			rubicon_zone_thread(&CONFIG);
			/*if TAPMER off set mode ALARM, swicth all outputs OFF, all fault LED ON*/
			if(!CheckTamperPin())
			{
				if((mode != PROGRAMMING_SS)&&(mode != DEBUG))
				{
					mode = ALARM;
				}
				
				led_zone_0_err_on;
				led_zone_1_err_on;
			}
			else
			{
				if((mode != PROGRAMMING_SS)&&(mode != DEBUG))
				{
					mode = NORMAL;
				}
				led_zone_0_err_off;
				led_zone_1_err_off;
			}
			
		}
		vTaskDelay(1);
	}
}

/* name: rubicon_zone_thread
*  descriprion: main routine thread, adc channel digitization return enum of states
*/
DEVICE_STATE_TypeDef rubicon_zone_thread(CONFIG_TypeDef* configuration)
{
	static int tick_zone_0 = 0,
			   tick_zone_1 = 0,
			   zone_0_trigger_counter = 0,
			   zone_1_trigger_counter = 0,
			   zone_0_trigger = 0,
			   zone_1_trigger = 0;
	
	DEVICE_STATE_TypeDef state = S_NORMAL;
	/*zone 0 counter down*/
	if(tick_zone_0 > 0)
	{
		tick_zone_0 --;
	}
	else
	{
		/*trigger counter reset*/
		zone_0_trigger_counter = 0;
		zone_0_trigger = 0;
	}
	
	/*zone 1 counter down*/
	if(tick_zone_1 > 0)
	{
		tick_zone_1 --;
	}
	else
	{
		/*trigger counter reset*/
		zone_1_trigger_counter = 0;
		zone_1_trigger = 0;
	}
	
	/*ZONE 0*/
	if(!MODE.bit.zone0_enable)
	{
		switch(MODE.bit.zone0_mode)
		{
			case TRUE:
				/*zone 0 treshold exceed*/
				if(ZONE_0_F1 > zone_0_treshold)
				{
					if(zone_0_trigger == 0)
					{
						zone_0_trigger = 1;
					}
				}
				break;
			case FALSE:
				if(ZONE_0_F2 > zone_0_treshold)
				{
					if(zone_0_trigger == 0)
					{
						zone_0_trigger = 1;
					}
				}
				break;
		}
	}
	
	/*ZONE 1*/
	if(!MODE.bit.zone1_enable)
	{
		switch(MODE.bit.zone0_mode)
		{
			case TRUE:
				/*zone 1 treshold exceed*/
				if(ZONE_1_F1 > zone_1_treshold)
				{
					
				}
			break;
		case FALSE:
				if(ZONE_1_F2 > zone_1_treshold)
				{
					
				}
			break;
		}
	}
	
	return state;
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
			mprintf(__OUT_MESSAGE);
			mprintf(__POSLINE);
		break;
		/*print help textblock*/
		case C_HELP:
			break;
		/*save current config to flash
		WARNING!!! CPU stop during flash programming*/
		case C_SAVE:
			flash_data_write(CONFIG_FLASH_ADDRESS,CONFIG_SECTOR_NUMBER,CONFIG.array,sizeof(CONFIG));
			mprintf("ok\r\n");
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
					mprintf(__ERROR_MESSAGE);
					mprintf(__NEWLINE);
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
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TIMEINT2:
					if(command.value != 0)
					{
						CONFIG.data.zone_1_timeint = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRESHOLD1:
					if(command.value != 0)
					{
						CONFIG.data.zone_0_treshold = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRESHOLD2:
					if(command.value != 0)
					{
						CONFIG.data.zone_1_treshold = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRIGLIMIT1:
					if(command.value != 0)
					{
						CONFIG.data.zone_0_triglimit = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRIGLIMIT2:
					if(command.value != 0)
					{
						CONFIG.data.zone_1_triglimit = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				default:
					mprintf(__ERROR_MESSAGE);
					mprintf(__NEWLINE);
					break;
			}
			break;
		case C_DEBUG:
			mode = DEBUG;
			mprintf(__POSLINE);
			mprintf("current adc values : \r\n");
			break;
		/*error in input*/
		case C_ERROR:
			mprintf(__ERROR_MESSAGE);
			mprintf(__NEWLINE);
			break;
		default:
			break;
	}
}

/* name: adc_covert_from_mv
*  descriprion: convert value in mV to ADC output format
*/
uint16_t adc_covert_from_mv(uint16_t value)
{
	uint16_t data = (ADC_DEPTH*value)/INT_ADC_REF;
	return data;
}

/* name: adc_covert_to_mv
*  descriprion: convert value from ADC DR to mV
*/
uint16_t adc_covert_to_mv(uint16_t value)
{
	uint16_t data = (INT_ADC_REF*value)/ADC_DEPTH;
	return data;
}

/* name: serial_debug_output
*  descriprion: debug output from ADC channels
*/
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
	mprintf(" adc ch 1 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_1[i]);
	}
	mprintf(" adc ch 2 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_2[i]);
	}
	mprintf(" adc ch 4 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_4[i]);
	}
	mprintf(" adc ch 5 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_5[i]);
	}
	mprintf(" adc ch 6 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_6[i]);
	}
	mprintf(" adc ch 7 : ");
	for(int i = 0; i < 4; i++)
	{
		serial_send_byte(serial_pointer,ADC_CHANNELS.ch_7[i]);
	}
	mprintf("\r");
	
}

/****************************end of file ********************************/
