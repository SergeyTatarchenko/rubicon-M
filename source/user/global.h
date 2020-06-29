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
        unsigned out_0:1;
        unsigned out_1:1;
        unsigned out_2:1;
        unsigned out_3:1;
    }bit;	
}OUTPUTS_TypeDef;
/*----------------------------------------------------------------------*/
extern ADDRESS_TypeDef ADRESS;
extern MODE_TypeDef MODE;
extern OUTPUTS_TypeDef OUTPUTS;
/*----------------------------------------------------------------------*/
void GetHwAdrState( ADDRESS_TypeDef* state );
void GetHwModeState( MODE_TypeDef *state );
void GetHwOutState (OUTPUTS_TypeDef* state);
#endif
/****************************end of file ********************************/
