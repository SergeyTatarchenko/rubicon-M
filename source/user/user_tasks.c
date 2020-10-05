/************************************************************************
* File Name          : user_tasks.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : RTOS high level routines
*************************************************************************/

/*----------------------------------------------------------------------*/
#include "user_tasks.h"
/*----------------------------------------------------------------------*/
/*value from CONFIG struct converted from mV*/
static uint16_t zone_0_treshold = 0;
static uint16_t zone_1_treshold = 0;
/*value from CONFIG struct converted from s*/
static uint16_t zone_0_timeint = 0;
static uint16_t zone_1_timeint = 0;

void serial_data_proc( char byte );
void def_data_proc( char byte );
void mode_switcher( char byte );

/* name: USART6_IRQHandler
*  descriprion: interrupt handler for RS 485 port communication
*/
void USART6_IRQHandler()
{
	char byte;
	/*data exist in DR register*/
	if(USART6->SR &= USART_SR_RXNE)
	{
		byte = USART6->DR;
		switch(mode)
		{
			case NORMAL:
				/*<mode switcher/>*/
				mode_switcher(byte);
				/*</mode switcher>*/
				/*777 data proc*/
				break;
			case ALARM:
				/*<mode switcher/>*/
				mode_switcher(byte);
				/*</mode switcher>*/
				/*777 data proc*/
				break;
			case PROGRAMMING_SS:
				serial_data_proc(byte);
				break;
			case DEBUG:
				serial_data_proc(byte);
				break;
			case IDLE:
				serial_data_proc(byte);
				break;
			
			default:
				break;
		}
	}
}

/* name: mode_switcher
*  descriprion: switch board mode via serial port 
*/
void mode_switcher( char byte )
{
	static int switch_mode_cnt = 0;
	if(byte == SWITCH_BYTE)
	{
		if((switch_mode_cnt == SWITCH_SEQ_LENGHT) &&
			(mode!= IDLE)&&(mode != PROGRAMMING_SS)&&
			(mode != DEBUG) )
		{
			mode = IDLE;
			switch_mode_cnt = 0;
		}
		else
		{
			switch_mode_cnt++;
		}
	}
	else
	{
		switch_mode_cnt = 0;
	}
}
/* name: def_data_proc
*  descriprion: data proc in NORMAL and ALARM mode
*/
void def_data_proc(char byte)
{
	//static int counter = 0;
	//static char buffer[sizeof(RUBICON_CONTROL_MESSAGE_TypeDef)] = {0};
	/*message header check*/
	
}

/* name: serial_data_proc
*  descriprion: data proc in PROGRAMMING,IDLE and DEBUG mode 
*/
void serial_data_proc(char byte)
{
	static int counter = 0;
	static char buffer[COMMAND_BUF_SIZE] = {0};
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
			case IDLE:
				mode = PROGRAMMING_SS;
				break;
			case DEBUG:
				mode = PROGRAMMING_SS;
				break;
			case PROGRAMMING_SS:
					/*send data to queue (command extracted in _task_service_serial function)*/
				xQueueSendFromISR(service_serial_queue,buffer,0);
				memset(buffer,0,COMMAND_BUF_SIZE);
				break;
			default:
				break;
		}
		counter = 0;
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
		if((mode == IDLE) && tick %40 == 0)
		{
			mprintf(__NEWLINE);
			mprintf(__HEADER_MESSAGE);
		}
		vTaskDelay(SERIAL_UPDATE_RATE);
	}
}

/* name: _task_rubicon_tread
*  descriprion: indication control procedure
*/
void _task_rubicon_tread(void *pvParameters)
{
	const int tick_overload = 3600;
	static int tick = 0;
	char buffer[COMMAND_BUF_SIZE] = {0};
	while(TRUE)
	{
		if(tick > tick_overload)
		{
			tick = 0;
		}
		else
		{
			tick++; /*up every SERIAL_UPDATE_RATE ms */
		}
		if(uxQueueMessagesWaiting( kso_serial_queue ) != 0)
		{
			xQueueReceive(kso_serial_queue,&buffer,portMAX_DELAY);
		}
	vTaskDelay(SERIAL_UPDATE_RATE);
	}
}
/* name: _task_led
*  descriprion: indication control procedure
*/
void _task_led(void *pvParameters)
{
	while(TRUE)
	{
		switch(mode)
		{
			case NORMAL:
				sync_led_on;
				vTaskDelay(100);
				sync_led_off;
				vTaskDelay(900);
				break;
			case IDLE:
				sync_led_off;
				vTaskDelay(200);
				sync_led_on;
				vTaskDelay(200);
				break;
			case PROGRAMMING_SS:
				sync_led_off;
				vTaskDelay(100);
				sync_led_on;
				vTaskDelay(100);
				break;
			case ALARM:
				sync_led_off;
			vTaskDelay(10);
				break;
			case DEBUG:
				sync_led_on;
			vTaskDelay(10);
				break;
			case FAULT:
				sync_led_off;
			vTaskDelay(10);
				break;
			default:
				sync_led_off;
			vTaskDelay(10);
				break;
		}
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
	/* convert flash treshold value from mV to int*/
	zone_0_treshold = adc_covert_from_mv(CONFIG.data.zone_0_treshold);
	zone_1_treshold = adc_covert_from_mv(CONFIG.data.zone_1_treshold);
	
	/*convert flash timeint value from S to mS*/
	zone_0_timeint = CONFIG.data.zone_0_timeint * 1000;
	zone_1_timeint = CONFIG.data.zone_1_timeint * 1000;
	
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
		if((mode == NORMAL)||(mode == DEBUG))
		{
			rubicon_zone_thread(&CONFIG);
		}
		if(tick%STATE_UPDATE_RATE == 0)
		{
			/*update all hw states*/
			GetHwAdrState(&ADDRESS);
			GetHwModeState(&MODE);
			
			/*if TAPMER off set mode ALARM, swicth all outputs OFF, all fault LED ON*/
			if(!CheckTamperPin())
			{
				if((mode != PROGRAMMING_SS)&&(mode != DEBUG)&&(mode != IDLE))
				{
					mode = ALARM;
				}
				
				led_zone_0_err_on;
				led_zone_1_err_on;
			}
			else
			{
				if((mode != PROGRAMMING_SS)&&(mode != DEBUG)&&(mode != IDLE))
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
	/*counter_down - счетчик от timeint до 0,counter_up - счетчик от 0 до triglimit */
	static int zone_0_trigger = 0,zone_0_counter_up = 0,zone_0_counter_down = 0;
	static int zone_1_trigger = 0,zone_1_counter_up = 0,zone_1_counter_down = 0;
	
	DEVICE_STATE_TypeDef state = S_NORMAL;
	
	/*zone 0*/
	if(MODE.bit.zone0_enable == 1)
	{
		/*trigger SET*/
		if((zone_0_trigger == 0)&&(ZONE_0_F > zone_0_treshold))
		{
			zone_0_trigger = 1;
			zone_0_counter_down = zone_0_timeint;
		}
		
		if(zone_0_counter_down > 0)
		{
			zone_0_counter_down--;
		}
		/*trigger RESET*/
		else
		{
			zone_0_trigger = 0;
			zone_0_counter_up = 0;
		}
		/*if trigger enable*/
		if((zone_0_trigger == 1)&&(ZONE_0_F > zone_0_treshold))
		{
			zone_0_counter_up++;
		}
		
		/*sensor triggering for zone 0!!!*/
		if((zone_0_counter_up>=configuration->data.zone_0_triglimit )&&(zone_0_counter_down > 0))
		{
			state = S_ALARM_ZONE1;
		}
	}
	/*zone 1*/
	if(MODE.bit.zone1_enable == 1)
	{
		/*trigger SET*/
		if((zone_1_trigger == 0)&&(ZONE_1_F > zone_1_treshold))
		{
			zone_1_trigger = 1;
			zone_1_counter_down = zone_1_timeint;
		}
		
		if(zone_1_counter_down > 0)
		{
			zone_1_counter_down--;
		}
		/*trigger RESET*/
		else
		{
			zone_1_trigger = 0;
			zone_1_counter_up = 0;
		}
		/*if trigger enable*/
		if((zone_1_trigger == 1)&&(ZONE_1_F > zone_1_treshold))
		{
			zone_1_counter_up++;
		}
		
		/*sensor triggering for zone 0!!!*/
		if((zone_1_counter_up>=configuration->data.zone_1_triglimit )&&(zone_1_counter_down > 0))
		{
			state = S_ALARM_ZONE2;
		}
	}
	return state;
}


/****************************end of file ********************************/
