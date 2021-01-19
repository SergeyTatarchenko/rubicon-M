/************************************************************************
* File Name          : user_tasks.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : Header for user_task.c
*************************************************************************/
#ifndef USER_TASKS_H_
#define	USER_TASKS_H_
/*----------------------------------------------------------------------*/
#include "global.h"
#include "RTOS.h"
#include "serial.h"
#include "text.h"
#include "rubicon_777.h"
/*----------------------------------------------------------------------*/
#define STATE_UPDATE_RATE	20
#define SERIAL_UPDATE_RATE	50

/*----------------------------------------------------------------------*/
void _task_led(void *pvParameters);
void _task_state_update(void *pvParameters);
void _task_service_serial(void *pvParameters);
void _task_rubicon_thread(void *pvParameters);
void _task_system_thread(void *pvParameters);
/*----------------------------------------------------------------------*/
DEVICE_STATE_TypeDef rubicon_zone_thread(CONFIG_TypeDef* configuration);

void serial_data_proc( char byte );
void def_data_proc   ( char byte );
void ModeSwitcher   ( char byte );

void zone_0_timer_handler (TimerHandle_t xTimer);
void zone_1_timer_handler (TimerHandle_t xTimer);

extern const unsigned portBASE_TYPE zone_0_timerID;
extern const unsigned portBASE_TYPE zone_1_timerID;

extern TimerHandle_t zone_0_timer;
extern TimerHandle_t zone_1_timer;


#define SWITCH_SEQ_LENGHT	4
#define SWITCH_BYTE			'\r'

#endif
/****************************end of file ********************************/
