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
void mode_switcher   ( char byte );

#define SWITCH_SEQ_LENGHT	4
#define SWITCH_BYTE			'\r'

#endif
/****************************end of file ********************************/
