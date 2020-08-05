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
/*----------------------------------------------------------------------*/
void _task_led(void *pvParameters);
void _task_state_update(void *pvParameters);
void _task_service_serial(void *pvParameters);
void _task_service_mirror(void *pvParameters);
/*----------------------------------------------------------------------*/


void serial_command_executor ( TCmdTypeDef command );
void serial_debug_output( void );

typedef enum
{
	NORMAL,
	PROGRAMMING_SS,
	PROGRAMMING_RS,
	ALARM,
	DEBUG,
	FAULT
}DEVICE_MODE;

#endif
/****************************end of file ********************************/
