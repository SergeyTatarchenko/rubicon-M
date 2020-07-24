/************************************************************************
* File Name          : mcu_config.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : Header for mcu_config.c
*************************************************************************/
#ifndef MCU_CONFIG_H_
#define MCU_CONFIG_H_

#include "stm32f2xx.h"
#include "serial.h"
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
#define led_zone_0_alrm_off  (GPIOA->BSRR |=GPIO_BSRR_BS_8)
#define led_zone_0_alrm_on (GPIOA->BSRR |=GPIO_BSRR_BR_8)

#define led_zone_0_err_off  (GPIOA->BSRR |=GPIO_BSRR_BS_9)
#define led_zone_0_err_on (GPIOA->BSRR |=GPIO_BSRR_BR_9)

#define led_zone_1_alrm_off  (GPIOA->BSRR |=GPIO_BSRR_BS_10)
#define led_zone_1_alrm_on (GPIOA->BSRR |=GPIO_BSRR_BR_10)

#define led_zone_1_err_off  (GPIOA->BSRR |=GPIO_BSRR_BS_11)
#define led_zone_1_err_on (GPIOA->BSRR |=GPIO_BSRR_BR_11)

#define sync_led_off (GPIOC->BSRR |=GPIO_BSRR_BS_9)
#define sync_led_on (GPIOC->BSRR |=GPIO_BSRR_BR_9)

#define service_rts_on (GPIOB->BSRR |=GPIO_BSRR_BS_12)
#define service_rts_off (GPIOB->BSRR |=GPIO_BSRR_BR_12)

#define num_of_adc_conversion 6
/*----------------------------------------------------------------------*/
void pin_config(void);
void peripheral_config(void);
void sys_init(void);

#endif
