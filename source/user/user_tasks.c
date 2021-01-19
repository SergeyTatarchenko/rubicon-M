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
static uint32_t zone_0_timeint = 0;
static uint32_t zone_1_timeint = 0;
/*software timers instance and id*/
const unsigned portBASE_TYPE zone_0_timerID = 1;
const unsigned portBASE_TYPE zone_1_timerID = 2;

TimerHandle_t zone_0_timer;
TimerHandle_t zone_1_timer;

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
				ModeSwitcher(byte);
				/*</mode switcher>*/
				/*777 data proc*/
				break;
			case ALARM:
				/*<mode switcher/>*/
				ModeSwitcher(byte);
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

/* name: ModeSwitcher
*  descriprion: switch board mode via serial port 
*/
void ModeSwitcher( char byte )
{
	static int switch_mode_cnt = 0;
	if(byte == SWITCH_BYTE)
	{
		if((switch_mode_cnt == SWITCH_SEQ_LENGHT) &&
			((mode== NORMAL)||(mode == ALARM)))
		{
			mode_update(IDLE);
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
				mode_update(PROGRAMMING_SS);
				break;
			case DEBUG:
				mode_update(PROGRAMMING_SS);
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
		/*command buffer not empty*/
		if(uxQueueMessagesWaiting( service_serial_queue ) != 0)
		{
			xQueueReceive(service_serial_queue,&buffer,portMAX_DELAY);
			if(GetAddressFromBuf(buffer))
			{
				input_command = command_processing(buffer);
				serial_command_executor(input_command,buffer);
			}
				memset(buffer,0,COMMAND_BUF_SIZE);
		}
		vTaskDelay(SERIAL_UPDATE_RATE);
	}
}

/* name: _task_system_thread
*  descriprion: system management
*/
void _task_system_thread(void *pvParameters)
{
	static DEVICE_MODE local_mode = NORMAL;
	
	while(TRUE)
	{
		xSemaphoreTake(xSemph_state_UPDATE,portMAX_DELAY);
		
		/* тут управление выходами реле */
		if(mode != ALARM)
		{
			switch(global_state)
			{
				/*сработка зоны 1*/
				case S_ALARM_ZONE1:
					/*вкл. индикацию на светодиод, откл реле на 2с*/
					led_zone_0_alrm_on;
					relay_z0_alrm_off;
					/*добавить занесение триггера в ответное сообщение для КСО*/
					/*старт таймера возврата реле*/
					xTimerStart(zone_0_timer, 0);
					break;
				/*обрыв кабеля зоны 1*/
				case S_ERROR_ZONE1:
					/*вкл. индикацию на светодиод, откл реле ошибки до восстановления */
					led_zone_0_err_on;
					relay_z0_err_off;
					/*добавить занесение триггера в ответное сообщение для КСО*/
					break;
				/*сработка зоны 2*/
				case S_ALARM_ZONE2:
					/*вкл. индикацию на светодиод, откл реле на 2с*/
					led_zone_1_alrm_on;
					relay_z1_alrm_off;
					/*добавить занесение триггера в ответное сообщение для КСО*/
					/*старт таймера возврата реле*/
					xTimerStart(zone_1_timer, 0);
					break;
				/*обрыв кабеля зоны 2*/
				case S_ERROR_ZONE2:
					/*вкл. индикацию на светодиод, откл реле ошибки до восстановления */
					led_zone_1_err_on;
					relay_z1_err_off;
					/*добавить занесение триггера в ответное сообщение для КСО*/
					break;
				/*сработка обоих зон*/
				case S_ALARM_ALL:
					led_zone_0_alrm_on;
					relay_z0_alrm_off;
					led_zone_1_alrm_on;
					relay_z1_alrm_off;
					
					xTimerStart(zone_0_timer, 0);
					xTimerStart(zone_1_timer, 0);
					break;
				/*обрыв кабеля обоих зон*/
				case S_ERROR_ALL:
					relay_z0_err_off;
					relay_z1_err_off;
					break;
				/*дежурный режим*/
				case S_NORMAL:
					/*возврат реле ошибки*/
					relay_z0_err_on;
					relay_z1_err_on;
					/*возврат в дежурный режим выходов реле сработки */
					if(local_mode == ALARM)
					{
						local_mode = NORMAL;
						led_zone_0_alrm_off;
						led_zone_1_alrm_off;
						
						relay_z0_alrm_on;
						relay_z1_alrm_on;
					}
					break;
				default:
					break;
			}
		}
		else
		{
			/* Сработка тампера, откл. всех реле, ожидание семафора на смену состояния */
			local_mode = ALARM;
			relay_z0_err_off;
			relay_z1_err_off;
			relay_z0_alrm_off;
			relay_z1_alrm_off;
		}
	}
}

/* name: zone_0_timer_handler
*  descriprion: handler for zone 0 relay timer
*/
void zone_0_timer_handler (TimerHandle_t xTimer)
{
	if(mode != ALARM)
	{
		led_zone_0_alrm_off;
		relay_z0_alrm_on;
	}
}

/* name: zone_1_timer_handler
*  descriprion: handler for zone 1 relay timer
*/
void zone_1_timer_handler (TimerHandle_t xTimer)
{
	if(mode != ALARM)
	{
		led_zone_1_alrm_off;
		relay_z1_alrm_on;
	}
}

/* name: _task_rubicon_tread
*  descriprion: indication control procedure
*/
void _task_rubicon_thread(void *pvParameters)
{
	char buffer[COMMAND_BUF_SIZE] = {0};

	while(TRUE)
	{
		
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
	
	static DEVICE_STATE_TypeDef local_state = S_NORMAL;
	static DEVICE_MODE local_mode = NORMAL;
	
	/* convert flash treshold value from mV to int*/
	zone_0_treshold = adc_covert_from_mv(CONFIG.data.zone_0_treshold);
	zone_1_treshold = adc_covert_from_mv(CONFIG.data.zone_1_treshold);
	
	/*convert flash timeint value from S to mS*/
	zone_0_timeint = CONFIG.data.zone_0_timeint * 1000;
	zone_1_timeint = CONFIG.data.zone_1_timeint * 1000;
	
	relay_z0_err_on;
	relay_z1_err_on;
	relay_z0_alrm_on;
	relay_z1_alrm_on;
	
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
		if((mode == NORMAL)||(mode == DEBUG)||(mode == PROGRAMMING_SS))
		{
			/*В нормальном режиме работы вызов функции каждую ~1мс*/
			local_state = rubicon_zone_thread(&CONFIG);
			
			/* переключение задачи на _task_system_thread при смене состояния */
			if(local_state != global_state )
			{
				global_state = local_state;
				xSemaphoreGive(xSemph_state_UPDATE);
			}
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
					mode_update(ALARM);
				}
				
				led_zone_0_err_on;
				led_zone_1_err_on;
				
				led_zone_0_alrm_off;
				led_zone_1_alrm_off;
				
				local_mode = ALARM;
				/* переключение задачи на _task_system_thread при отрыве тампера */
				xSemaphoreGive(xSemph_state_UPDATE);
			}
			else
			{
				if((mode != PROGRAMMING_SS)&&(mode != DEBUG)&&(mode != IDLE))
				{
					mode_update(NORMAL);
				}
				if(local_mode == ALARM)
				{
					local_mode = NORMAL;
					global_state = S_NORMAL;
					
					/* переключение задачи на _task_system_thread при возврате тампера */
					xSemaphoreGive(xSemph_state_UPDATE);
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
	
	uint8_t updater = 0;
	DEVICE_STATE_TypeDef state = S_NORMAL;
	
	/*zone 0*/
	if(MODE.bit.zone0_enable == 1)
	{
		/*добавить проверку кабеля на обрыв, опрос сигнала кабеля только при отсутствии обрыва */
		/*trigger SET*/
		if((zone_0_trigger == 0)&&(ZONE_0_F > zone_0_treshold))
		{
			zone_0_trigger = 1;
			zone_0_counter_down = zone_0_timeint;
		}
		/* начало дискретизации сигнала в течение заданного интервала после первого превышения порога*/
		if(zone_0_trigger == 1)
		{
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
			if(ZONE_0_F > zone_0_treshold)
			{
				zone_0_counter_up++;
			}
			/*накопленное значение интеграла превышает заданное в пределе интервала (сработка зоны X)*/
			if((zone_0_counter_up>=configuration->data.zone_0_triglimit )&&(zone_0_counter_down > 0))
			{
				zone_0_counter_down = 0;
				zone_0_counter_up = 0;
				state = S_ALARM_ZONE1;
				updater++;
			}
		}
	}
	/*zone 1*/
	if(MODE.bit.zone1_enable == 1)
	{
		/*добавить проверку кабеля на обрыв, опрос сигнала кабеля только при отсутствии обрыва */
		/*trigger SET*/
		if((zone_1_trigger == 0)&&(ZONE_1_F > zone_1_treshold))
		{
			zone_1_trigger = 1;
			zone_1_counter_down = zone_1_timeint;
		}
		/* начало дискретизации сигнала в течение заданного интервала после первого превышения порога*/
		if(zone_1_trigger == 1)
		{
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
			if(ZONE_1_F > zone_1_treshold)
			{
				zone_1_counter_up++;
			}
			/*накопленное значение интеграла превышает заданное в пределе интервала (сработка зоны X)*/
			if((zone_1_counter_up>=configuration->data.zone_1_triglimit )&&(zone_1_counter_down > 0))
			{
				zone_1_counter_down = 0;
				zone_1_counter_up = 0;
				state = S_ALARM_ZONE2;
				updater++;
			}
		}
	}
	if(updater > 1)
	{
		state = S_ALARM_ALL;
	}
	return state;
}


/****************************end of file ********************************/
