/************************************************************************
* File Name          : global.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : Header with global defines and function prototypes
*************************************************************************/
#ifndef GLOBAL_H_
#define	GLOBAL_H_

/*----------------------------------------------------------------------*/
#include "mcu_config.h"


#define TRUE 1
#define FALSE 0

#define VERSION	"0.1"
/*----------------------------------------------------------------------*/
/* union for manual ADDRESS configuration                               */
typedef union
{
    uint8_t byte;
    struct
    {
        unsigned ADDR_bit0:1;                          /* address bit 0 */
        unsigned ADDR_bit1:1;                          /* address bit 1 */
        unsigned ADDR_bit2:1;                          /* address bit 2 */
        unsigned ADDR_bit3:1;                          /* address bit 3 */
        unsigned ADDR_bit4:1;                          /* address bit 4 */
        unsigned ADDR_bit5:1;                          /* address bit 5 */
        unsigned ADDR_bit6:1;                          /* address bit 6 */
        unsigned ADDR_bit7:1;                          /* address bit 7 */
    }bit;
}ADDRESS_TypeDef;

/* struct for device MODE configuration                                 */
typedef union
{
    uint8_t byte;
    struct
    {
        unsigned zone0_enable:1;     /* 0 - zone 0 disabled, 1 - enabled*/
        unsigned zone1_enable:1;     /* 0 - zone 1 disabled, 1 - enabled*/
        unsigned zone0_mode:1;/* 0 - default mode for zone 0,1 - special*/
        unsigned zone1_mode:1;/* 0 - default mode for zone 1,1 - special*/
    }bit;
}MODE_TypeDef;

/* struct for OUTPUTS state implemention                                */
typedef union
{
    uint8_t byte;
    struct
    {
        unsigned zone_0_alarm:1;
        unsigned zone_0_error:1;
        unsigned zone_1_alarm:1;
        unsigned zone_1_error:1;
    }bit;
}OUTPUTS_TypeDef;
/* struct for device STATE implemention                                */
typedef struct
{
	union
	{
		uint8_t byte;
		struct
		{
			unsigned RS_485_connected:1;
			unsigned zone_0_alarm:1;
			unsigned zone_0_error:1;
			unsigned zone_1_alarm:1;
			unsigned zone_1_error:1;
		}bit;
	}status;
}STATE_TypeDef;

/* struct for device general mode and configuration                     */
typedef union
{
	struct{
		union{
			uint16_t byte;
			struct{
				unsigned first_startup:1;
				
			}bit;
		}status;
	uint16_t zone_0_treshold;
	uint16_t zone_1_treshold;
	uint16_t zone_0_timeint;
	uint16_t zone_1_timeint;
	uint16_t zone_0_triglimit;
	uint16_t zone_1_triglimit;
	}data;
	uint32_t array[8]; /*4*8 data config size */
}CONFIG_TypeDef;

/* struct for adc values from zone amplifiers                           */
typedef struct{
	
	uint16_t alrm_0; /*ch 1*/
	uint16_t alrm_1; /*ch 2*/
	uint16_t sign_0; /*ch 4*/
	uint16_t sign_1; /*ch 5*/
	uint16_t sign_2; /*ch 6*/
	uint16_t sign_3; /*ch 7*/
	
}ADC_VALUES_TypeDef;

/* struct for adc debug output through serial port                      */
typedef struct {
	
	char ch_1[4];
	char ch_2[4];
	char ch_4[4];
	char ch_5[4];
	char ch_6[4];
	char ch_7[4];
	
}ADC_CHANNELS_TypeDef;
/*----------------------------------------------------------------------*/
extern ADDRESS_TypeDef ADRESS;
extern MODE_TypeDef MODE;
extern OUTPUTS_TypeDef OUTPUTS;
extern CONFIG_TypeDef CONFIG;
extern ADC_VALUES_TypeDef ADC_VALUES;
extern ADC_CHANNELS_TypeDef ADC_CHANNELS;
/*----------------------------------------------------------------------*/
void GetHwAdrState( ADDRESS_TypeDef* state );
void GetHwModeState( MODE_TypeDef *state );
void GetHwOutState ( OUTPUTS_TypeDef* state);
#endif
/****************************end of file ********************************/
