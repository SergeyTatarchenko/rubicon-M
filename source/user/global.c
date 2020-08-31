/************************************************************************
* File Name          : global.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : File with global defines and function prototypes
*************************************************************************/
#include "global.h"

/*----------------------------------------------------------------------*/
/*extern used*/
ADDRESS_TypeDef ADDRESS;
MODE_TypeDef MODE;
OUTPUTS_TypeDef OUTPUTS;
CONFIG_TypeDef CONFIG;
ADC_VALUES_TypeDef ADC_VALUES;
ADC_CHANNELS_TypeDef ADC_CHANNELS;

xQueueHandle service_serial_queue;
xQueueHandle kso_serial_queue;
xSemaphoreHandle xMutex_serial_BUSY;

DEVICE_MODE mode = NORMAL;

char sp_buff[__STRLEN] = {0};

/*default serial transmit is USART3*/
//USART_TypeDef * serial_pointer = USART3;
USART_TypeDef * serial_pointer = USART6;

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

/*
* check TAPMER input, return 0 if signal OFF
*/
int CheckTamperPin()
{
	int signal = (GPIOA->IDR & GPIO_IDR_IDR_15);
	if(signal != 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
/* name: adc_covert_from_mv
*  descriprion: convert value in mV to ADC output format
*/
uint16_t adc_covert_from_mv(uint16_t value)
{
	uint16_t data = (ADC_DEPTH*value)/INT_ADC_REF;
	return data;
}

/* name: adc_covert_to_mv
*  descriprion: convert value from ADC DR to mV
*/
uint16_t adc_covert_to_mv(uint16_t value)
{
	uint16_t data = (INT_ADC_REF*value)/ADC_DEPTH;
	return data;
}
/****************************end of file ********************************/
