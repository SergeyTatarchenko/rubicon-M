/************************************************************************
* File Name          : mcu_config.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : mcu pins and peripheral low leve configuration
*************************************************************************/

#include "mcu_config.h"

/*
* low level microchip init function
*/
void sys_init()
{
    pin_config();
    peripheral_config();
}

/*
* microchip peripheral cinfiguration function
*/
void peripheral_config()
{
/*
 ADC channel configuration, 
*/

}

/*
* microchip pin configuration fuction
*/
void pin_config()
{

/*
 ADDRESS pins, PC0,PC1,PC2,PC3, PA0 
 configure as input floating/pullup, low speed
*/
    GPIOC->MODER &=   ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|
                        GPIO_MODER_MODER2|GPIO_MODER_MODER3);
    GPIOC->PUPDR |=    (GPIO_PUPDR_PUPDR0_0|GPIO_PUPDR_PUPDR1_0|
                        GPIO_PUPDR_PUPDR2_0|GPIO_PUPDR_PUPDR3_0);
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0|GPIO_OSPEEDER_OSPEEDR1|
                        GPIO_OSPEEDER_OSPEEDR2|GPIO_OSPEEDER_OSPEEDR3);
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0;
/*
 MODE pins PB0, PB1, PC10, PC11
 configure as input floating/pullup, low speed
*/
    GPIOB->MODER &= ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1);
    GPIOB->PUPDR |=  (GPIO_PUPDR_PUPDR0_0|GPIO_PUPDR_PUPDR1_0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0|GPIO_OSPEEDER_OSPEEDR1);
    GPIOC->MODER &= ~(GPIO_MODER_MODER10|GPIO_MODER_MODER11);
    GPIOC->PUPDR |=  (GPIO_PUPDR_PUPDR10_0|GPIO_PUPDR_PUPDR11_0);
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11);
/*
 ADC pins PA1, PA2, PA4, PA5, PA6, PA7
 configure in as analog mode, high speed
*/
    GPIOA->MODER |= (GPIO_MODER_MODER1|GPIO_MODER_MODER2|GPIO_MODER_MODER4|
                     GPIO_MODER_MODER5|GPIO_MODER_MODER6|GPIO_MODER_MODER7);
    GPIOA->OSPEEDR |=(GPIO_OSPEEDER_OSPEEDR1|GPIO_OSPEEDER_OSPEEDR2|GPIO_OSPEEDER_OSPEEDR4|
                      GPIO_OSPEEDER_OSPEEDR5|GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7); 
/*
 pins for LED control PA8, PA9, PA10, PA11, PC9
 configure as output PP, low speed
*/
    GPIOA->MODER |= (GPIO_MODER_MODER8_0|GPIO_MODER_MODER9_0|
                     GPIO_MODER_MODER10_0|GPIO_MODER_MODER11_0);
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR9|
                       GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11);
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
/*
pins for RELAY control PB4, PB5, PB6, PB7
configure as as output PP, low speed
*/
    GPIOB->MODER |= (GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0|
                     GPIO_MODER_MODER6_0|GPIO_MODER_MODER7_0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR4|GPIO_OSPEEDER_OSPEEDR5|
                        GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7);
/*
pins for RS485 (USART6) PC6, PC7, PC8
configure as alternative function (AF8)
*/
    GPIOC->MODER   |= (GPIO_MODER_MODER6_1|GPIO_MODER_MODER7_1|GPIO_MODER_MODER8_0);
    GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7|GPIO_OSPEEDER_OSPEEDR8);
    GPIOC->AFR[0]  |= (((uint32_t)0x08000000)|((uint32_t)0x80000000));
    
/*
pins for service serial port (USART3) PB10, PB11, PB12
configure as alternative function (AF7)
*/
    GPIOB->MODER   |= (GPIO_MODER_MODER10_1|GPIO_MODER_MODER11_1|GPIO_MODER_MODER12_0);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11|GPIO_OSPEEDER_OSPEEDR12);
    GPIOB->AFR[1]  |= (((uint32_t)0x08000700)|((uint32_t)0x80007000));
}


