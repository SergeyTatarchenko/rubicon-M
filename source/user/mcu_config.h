/************************************************************************
* File Name          : mcu_config.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : Header for mcu_config.c
*************************************************************************/
#ifndef MCU_CONFIG_H_
#define MCU_CONFIG_H_

#include "stm32f2xx.h"
/*----------------------------------------------------------------------*/

/*
RUBICON revision A0 pinout description:
  
    ADDRESS pins:
        ADDR0 - PC0
        ADDR1 - PC1
        ADDR2 - PC2
        ADDR3 - PC3
        ADDR4 - PA0

    MODE pins:
        ZONE 0 EN   - PB0
        ZONE 1 EN   - PB1
        ZONE 0 MODE - PC10
        ZONE 1 MODE - PC11
    
    ADC pinout:
        ALARM0 - PA1
        ALARM1 - PA2
        SIGN0  - PA4
        SIGN1  - PA5
        SIGN2  - PA6
        SIGN3  - PA7
    
    LED pinout:
        ZONE 0 LED ALARM - PA8
        ZONE 0 LED ERROR - PA9
        ZONE 1 LED ALARM - PA10
        ZONE 1 LED ERROR - PA11
        RS-485 connect   - PC9
    
    OUTPUTS pinout:
        RELAY0 - PB4
        RELAY1 - PB5
        RELAY2 - PB6
        RELAY3 - PB7
    
    RS485 pinout:
        TX  - PC6 
        RX  - PC7
        RTS - PC8

    service serial port pinout:
        TX  - PB10
        RX  - PB11
        RTS - PB12

*/

/*----------------------------------------------------------------------*/
void pin_config(void);
void peripheral_config(void);
void sys_init(void);

#endif
