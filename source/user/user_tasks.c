/************************************************************************
* File Name          : user_tasks.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : RTOS routines
*************************************************************************/

/*----------------------------------------------------------------------*/
#include "user_tasks.h"
/*----------------------------------------------------------------------*/

/*value from CONFIG struct converted from mV*/

static uint16_t Zone0ClimbTreshold = 0;
static uint16_t Zone1ClimbTreshold = 0;
static uint16_t Zone0CutTreshold   = 0;
static uint16_t Zone1CutTreshold   = 0;

/*value from CONFIG struct converted from s*/

static uint32_t Zone0ClimbTimeint = 0;
static uint32_t Zone1ClimbTimeint = 0;
static uint32_t Zone0CutTimeint   = 0;
static uint32_t Zone1CutTimeint   = 0;

/*software timers instance and id*/
const unsigned portBASE_TYPE zone_0_timerID = 1;
const unsigned portBASE_TYPE zone_1_timerID = 2;

TimerHandle_t zone_0_timer;
TimerHandle_t zone_1_timer;

static uint8_t zone_0_tigger = FALSE;
static uint8_t zone_1_tigger = FALSE;

/* 
* name: USART6_IRQHandler
* descriprion: interrupt handler for RS 485 port communication
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
				SerialDataProc(byte);
				break;
			case DEBUG:
				SerialDataProc(byte);
				break;
			case IDLE:
				SerialDataProc(byte);
				break;
			default:
				break;
		}
	}
}

/* name: TIM2_IRQHandler
*  descriprion: частота дискретизации сигнала по прорезанию на кабеле: 250 мкс;
   частота дискретизации сигнала по перелазу на кабеле: 1 мс;
*/

void TIM2_IRQHandler()
{
	static uint32_t counter = 0;
	static const uint32_t overload = 3600000;
	static portBASE_TYPE xTaskWoken = pdFALSE;
	
	static uint8_t zone_0_cable_break_trigger = FALSE,
	               zone_1_cable_break_trigger = FALSE;
	static uint32_t zone_0_cable_break_counter = 0,
	                zone_1_cable_break_counter = 0;
	static uint32_t local_state = (uint32_t)S_NORMAL;
	/*если значение контроля кабеля 5 секунд > ZONE_CABLE_BREAK_VALUE
	  тогда сработка ошибки - кабель поврежден.*/
	static const uint32_t cable_break_overload = 12000;
	
	DEVICE_STATE_TypeDef ret_value = S_NORMAL;
	
	if(TIM2->SR&TIM_SR_UIF)
	{
		TIM2->SR &= ~TIM_SR_UIF;
	}
	
	if(counter == overload)
	{
		counter =0;
	}
	else
	{
		counter++;
	}
	/*контроль зоны 1*/
	if(MODE.bit.zone0_enable && zone_0_tigger)
	{
		if( (ZONE_0_C > ZONE_CABLE_BREAK_VALUE) && (zone_0_cable_break_trigger != TRUE) )
		{
			zone_0_cable_break_counter++;
			if(zone_0_cable_break_counter > cable_break_overload)
			{
				/*кабель поврежден*/
				zone_0_cable_break_trigger = TRUE;
			}
		}
		else
		{
			zone_0_cable_break_trigger = FALSE;
			zone_0_cable_break_counter = 0;
		}
		if(zone_0_cable_break_trigger != TRUE)
		{
			/*триггер прорезания зоны 1*/
			ret_value = Zone1CutThread(&CONFIG);
			/*триггер перелаза зоны 1*/
			if((counter %4 == 0)&&(ret_value == S_NORMAL))
			{
				ret_value = Zone1ClimbThread(&CONFIG);
			}
			if(ret_value != S_NORMAL)
			{
				local_state |= S_ALARM_ZONE1;
				zone_0_tigger = FALSE;
			}
		}
	}
	else
	{
		zone_0_cable_break_trigger = FALSE;
		local_state &= ~(uint32_t)(S_ERROR_ZONE1|S_ALARM_ZONE1);
	}
	if(zone_0_cable_break_trigger == TRUE)
	{
		local_state |= S_ERROR_ZONE1;
	}
		/*контроль зоны 2*/
	if(MODE.bit.zone1_enable && zone_1_tigger)
	{
		if( (ZONE_1_C > ZONE_CABLE_BREAK_VALUE) && (zone_1_cable_break_trigger != TRUE) )
		{
			zone_1_cable_break_counter++;
			if(zone_1_cable_break_counter > cable_break_overload)
			{
				/*кабель поврежден*/
				zone_1_cable_break_trigger = TRUE;
			}
		}
		else
		{
			zone_1_cable_break_trigger = FALSE;
			zone_1_cable_break_counter = 0;
		}
		if(zone_1_cable_break_trigger != TRUE)
		{
			/*триггер прорезания зоны 2*/
			ret_value = Zone2CutThread(&CONFIG);
			/*триггер перелаза зоны 2*/
			if((counter %4 == 0)&&(ret_value == S_NORMAL))
			{
				ret_value = Zone2ClimbThread(&CONFIG);
			}
			if(ret_value != S_NORMAL)
			{
				local_state |= S_ALARM_ZONE2;
				zone_1_tigger = FALSE;
			}
		}
	}
	else
	{
		zone_1_cable_break_trigger = FALSE;
		local_state &= ~(uint32_t)(S_ERROR_ZONE2|S_ALARM_ZONE2);
	}
	if(zone_1_cable_break_trigger == TRUE)
	{
		local_state |= S_ERROR_ZONE2;
	}
	
	if((local_state != global_state) && (mode != ALARM) )
	{
		global_state = local_state;
		/* переключение задачи на _task_system_thread при смене состояния */
		xSemaphoreGiveFromISR(xSemph_state_UPDATE,&xTaskWoken);
	}
	if(xTaskWoken == pdTRUE)
	{
		taskYIELD();
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
			ModeUpdate(IDLE);
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

/* name: DefDataProc
*  descriprion: data proc in NORMAL and ALARM mode
*/
void DefDataProc(char byte)
{
	//static int counter = 0;
	//static char buffer[sizeof(RUBICON_CONTROL_MESSAGE_TypeDef)] = {0};
	/*message header check*/
	
}

/* name: SerialDataProc
*  descriprion: data proc in PROGRAMMING,IDLE and DEBUG mode 
*/
void SerialDataProc(char byte)
{
	static int counter = 0;
	static char buffer[COMMAND_BUF_SIZE] = {0};
	if(byte != SWITCH_BYTE)
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
	else if(byte == SWITCH_BYTE)
	{
		/*mode switching*/
		switch(mode)
		{
			case IDLE:
				ModeUpdate(PROGRAMMING_SS);
				break;
			case DEBUG:
				ModeUpdate(PROGRAMMING_SS);
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
	TCmdTypeDef input_command;
	char buffer[COMMAND_BUF_SIZE] = {0};
	while(TRUE)
	{
		/*debug ADC output*/
		if(mode == DEBUG)
		{
			serial_debug_output();
		}
		/*command buffer not empty*/
		if(uxQueueMessagesWaiting( service_serial_queue ) != 0)
		{
			xQueueReceive(service_serial_queue,&buffer,portMAX_DELAY);
			
			input_command = command_processing(buffer);
			if(input_command.command == C_EXIT)
			{
				mode = NORMAL;
			}
			else if(GetAddressFromBuf(buffer))
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
			if(global_state & (uint32_t)S_ALARM_ZONE1)
			{
				/*вкл. индикацию на светодиод, откл реле на 2с*/
				led_zone_0_alrm_on;
				relay_z0_alrm_off;
				/*добавить занесение триггера в ответное сообщение для КСО*/
				/*старт таймера возврата реле*/
				xTimerStart(zone_0_timer, 0);
			}
			if(global_state & (uint32_t)S_ERROR_ZONE1)
			{
				/*вкл. индикацию на светодиод, откл реле ошибки до восстановления */
				led_zone_0_err_on;
				relay_z0_err_off;
			}
			else
			{
				led_zone_0_err_off;
				relay_z0_err_on;
			}
			if(global_state & (uint32_t)S_ALARM_ZONE2)
			{
				/*вкл. индикацию на светодиод, откл реле на 2с*/
				led_zone_1_alrm_on;
				relay_z1_alrm_off;
				/*добавить занесение триггера в ответное сообщение для КСО*/
				/*старт таймера возврата реле*/
				xTimerStart(zone_1_timer, 0);
			}
			if(global_state & (uint32_t)S_ERROR_ZONE2)
			{
				/*вкл. индикацию на светодиод, откл реле ошибки до восстановления */
				led_zone_1_err_on;
				relay_z1_err_off;
			}
			else
			{
				led_zone_1_err_off;
				relay_z1_err_on;
			}
			if(global_state == S_NORMAL)
			{
				/*возврат реле ошибки*/
				relay_z0_err_on;
				relay_z1_err_on;
				led_zone_0_err_off;
				led_zone_1_err_off;
			}
			/*возврат в дежурный режим выходов реле сработки */
			if(local_mode == ALARM)
			{
				local_mode = NORMAL;
				led_zone_0_alrm_off;
				led_zone_1_alrm_off;
				relay_z0_alrm_on;
				relay_z1_alrm_on;
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
			led_zone_0_err_on;
			led_zone_1_err_on;
			led_zone_0_alrm_on;
			led_zone_1_alrm_on;
		}
		vTaskDelay(100);
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
		zone_0_tigger = TRUE;
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
		zone_1_tigger = TRUE;
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
	
	static DEVICE_MODE local_mode = NORMAL;
	
	/* convert flash treshold value from mV to int*/
	Zone0ClimbTreshold = adc_covert_from_mv(CONFIG.data.zone_0_climb_treshold);
	Zone1ClimbTreshold = adc_covert_from_mv(CONFIG.data.zone_1_climb_treshold);
	Zone0CutTreshold   = adc_covert_from_mv(CONFIG.data.zone_0_cut_treshold);
	Zone1CutTreshold   = adc_covert_from_mv(CONFIG.data.zone_1_cut_treshold);
	
	/*climb trigger tick every 1 ms*/
	Zone0ClimbTimeint = CONFIG.data.zone_0_climb_timeint * 1000;
	Zone1ClimbTimeint = CONFIG.data.zone_1_climb_timeint * 1000;
	
	/*cut trigger tick every 250 us*/
	Zone0CutTimeint   = CONFIG.data.zone_0_cut_timeint * 1000 * 4;
	Zone1CutTimeint   = CONFIG.data.zone_1_cut_timeint * 1000 * 4;
	
	relay_z0_err_on;
	relay_z1_err_on;
	relay_z0_alrm_on;
	relay_z1_alrm_on;
	
	zone_0_tigger = TRUE;
	zone_1_tigger = TRUE;
	
	vTaskDelay(100);
	
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
			TIM2_START;
		}
		else
		{
			TIM2_STOP;
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
					ModeUpdate(ALARM);
				}
				local_mode = ALARM;
				/* переключение задачи на _task_system_thread при отрыве тампера */
				xSemaphoreGive(xSemph_state_UPDATE);
			}
			else
			{
				if((mode != PROGRAMMING_SS)&&(mode != DEBUG)&&(mode != IDLE))
				{
					ModeUpdate(NORMAL);
				}
				if(local_mode == ALARM)
				{
					local_mode = NORMAL;
					global_state = S_NORMAL;
					
					/* переключение задачи на _task_system_thread при возврате тампера */
					xSemaphoreGive(xSemph_state_UPDATE);
				}
			}
		}
		vTaskDelay(1);
	}
}

/*----------------------------------------------------------------*/

DEVICE_STATE_TypeDef Zone1ClimbThread(CONFIG_TypeDef* configuration)
{
	/*counter_down - счетчик от timeint до 0,counter_up - счетчик от 0 до triglimit */
	static int trigger = 0,cnt_up = 0,cnt_down = 0;
	DEVICE_STATE_TypeDef state = S_NORMAL;
	/*trigger SET*/
	if((trigger == 0)&&(ZONE_0_F > Zone0ClimbTreshold))
	{
		trigger = 1;
		cnt_down = Zone0ClimbTimeint;
	}
	/* начало дискретизации сигнала в течение заданного интервала после первого превышения порога*/
	if(trigger == 1)
	{
		if(cnt_down > 0)
		{
			cnt_down--;
		}
		/*trigger RESET*/
		else
		{
			trigger = 0;
			cnt_up = 0;
		}
		if(ZONE_0_F > Zone0ClimbTreshold)
		{
			cnt_up++;
		}
		/*накопленное значение интеграла превышает заданное в пределе интервала (сработка зоны X)*/
		if((cnt_up>=configuration->data.zone_0_climb_triglimit )
			&&(cnt_down > 0))
		{
			trigger = 0;
			cnt_down = 0;
			cnt_up = 0;
			state = S_ALARM_ZONE1;
		}
	}
	return state;
}

/*----------------------------------------------------------------*/

DEVICE_STATE_TypeDef Zone2ClimbThread(CONFIG_TypeDef* configuration)
{
	/*counter_down - счетчик от timeint до 0,counter_up - счетчик от 0 до triglimit */
	static int trigger = 0,cnt_up = 0,cnt_down = 0;
	DEVICE_STATE_TypeDef state = S_NORMAL;
	/*trigger SET*/
	if((trigger == 0)&&(ZONE_1_F > Zone1ClimbTreshold))
	{
		trigger = 1;
		cnt_down = Zone1ClimbTimeint;
	}
	/* начало дискретизации сигнала в течение заданного интервала после первого превышения порога*/
	if(trigger == 1)
	{
		if(cnt_down > 0)
		{
			cnt_down--;
		}
		/*trigger RESET*/
		else
		{
			trigger = 0;
			cnt_up = 0;
		}
		if(ZONE_1_F > Zone1ClimbTreshold)
		{
			cnt_up++;
		}
		/*накопленное значение интеграла превышает заданное в пределе интервала (сработка зоны X)*/
		if((cnt_up>=configuration->data.zone_1_climb_triglimit )
			&&(cnt_down > 0))
		{
			trigger = 0;
			cnt_down = 0;
			cnt_up = 0;
			state = S_ALARM_ZONE2;
		}
	}
	return state;
}

/*----------------------------------------------------------------*/

DEVICE_STATE_TypeDef Zone1CutThread(CONFIG_TypeDef* configuration)
{
	/*counter_down - счетчик от timeint до 0,counter_up - счетчик от 0 до triglimit */
	static int trigger = 0,cnt_up = 0,cnt_down = 0;
	DEVICE_STATE_TypeDef state = S_NORMAL;
	/*trigger SET*/
	if((trigger == 0)&&(ZONE_0_F > Zone0CutTreshold))
	{
		trigger = 1;
		cnt_down = Zone0CutTimeint;
	}
	/* начало дискретизации сигнала в течение заданного интервала после первого превышения порога*/
	if(trigger == 1)
	{
		if(cnt_down > 0)
		{
			cnt_down--;
		}
		/*trigger RESET*/
		else
		{
			trigger = 0;
			cnt_up = 0;
		}
		if(ZONE_0_F > Zone0CutTreshold)
		{
			cnt_up++;
		}
		/*накопленное значение интеграла превышает заданное в пределе интервала (сработка зоны X)*/
		if((cnt_up>=configuration->data.zone_0_cut_triglimit )
			&&(cnt_down > 0))
		{
			trigger = 0;
			cnt_down = 0;
			cnt_up = 0;
			state = S_ALARM_ZONE1;
		}
	}
	return state;
}

/*----------------------------------------------------------------*/

DEVICE_STATE_TypeDef Zone2CutThread(CONFIG_TypeDef* configuration)
{
	/*counter_down - счетчик от timeint до 0,counter_up - счетчик от 0 до triglimit */
	static int trigger = 0,cnt_up = 0,cnt_down = 0;
	DEVICE_STATE_TypeDef state = S_NORMAL;
	/*trigger SET*/
	if((trigger == 0)&&(ZONE_1_F > Zone1CutTreshold))
	{
		trigger = 1;
		cnt_down = Zone1CutTimeint;
	}
	/* начало дискретизации сигнала в течение заданного интервала после первого превышения порога*/
	if(trigger == 1)
	{
		if(cnt_down > 0)
		{
			cnt_down--;
		}
		/*trigger RESET*/
		else
		{
			trigger = 0;
			cnt_up = 0;
		}
		if(ZONE_1_F > Zone1CutTreshold)
		{
			cnt_up++;
		}
		/*накопленное значение интеграла превышает заданное в пределе интервала (сработка зоны X)*/
		if((cnt_up>=configuration->data.zone_1_cut_triglimit )
			&&(cnt_down > 0))
		{
			trigger = 0;
			cnt_down = 0;
			cnt_up = 0;
			state = S_ALARM_ZONE2;
		}
	}
	return state;
}


/****************************end of file ********************************/
