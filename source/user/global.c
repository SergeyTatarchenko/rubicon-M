/************************************************************************
* File Name          : global.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : File with global defines and function prototypes
*************************************************************************/
#include "global.h"

/*----------------------------------------------------------------------*/
/*extern used*/
ADDRESS_TypeDef ADRESS;
MODE_TypeDef MODE;
OUTPUTS_TypeDef OUTPUTS;
GONFIG_TypeDef GONFIG;
ADC_VALUES_TypeDef ADC_VALUES;
/*----------------------------------------------------------------------*/

/* 
* bind pin current hardware state with global union ADDRESS
*/
void GetHwAdrState( ADDRESS_TypeDef* state )
{
    uint32_t  temp = GPIOC->IDR;
    temp &= 0x0F; /* from PC0 to PC3 bit mask*/
    state->byte = temp;
    temp = GPIOA->IDR;
    temp &= 0x01; /* bit mask for PA0*/
    state->bit.ADDR_bit5 = temp;
}

/*
* bind pin current hardware state with global union MODE
*/
void GetHwModeState( MODE_TypeDef *state )
{
    uint32_t  temp = GPIOB->IDR;
    temp &= 0x03; /* from PB0 to PB1 bit mask*/
    state->byte = temp;
    temp = GPIOC->IDR;
    temp>>=10;/*move to PC10 - PC11*/
    temp &= 0x03;/*bit mask for PC10 - PC11*/
    temp<<=2;
    state->byte &= ~temp;
    state->byte |= temp;
}

/*
* bind current  output hardware state with global union OUTPUTS
*/
void GetHwOutState ( OUTPUTS_TypeDef* state )
{

}
/****************************end of file ********************************/
