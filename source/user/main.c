/************************************************************************
* File Name          : main.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : main progran file
*************************************************************************/

#include "mcu_config.h"
#include "RTOS.h"
#include "user_tasks.h"
/*----------------------------------------------------------------------*/


int main()
{
    /*
     init pins,  peripherals, setup initital values and states of modules and memory 
     */
    sys_init();

	
	
    while(1);
}
