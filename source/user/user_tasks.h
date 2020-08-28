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
#define STATE_UPDATE_RATE	20
#define SERIAL_UPDATE_RATE	50


/*----------------------------------------------------------------------*/

void _task_led(void *pvParameters);
void _task_state_update(void *pvParameters);
void _task_service_serial(void *pvParameters);
void _task_service_mirror(void *pvParameters);
/*----------------------------------------------------------------------*/

uint16_t adc_covert_from_mv(uint16_t value);
uint16_t adc_covert_to_mv(uint16_t value);

DEVICE_STATE_TypeDef rubicon_zone_thread(CONFIG_TypeDef* configuration);

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
