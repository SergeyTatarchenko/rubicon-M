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
#include "RTOS.h"
/*----------------------------------------------------------------------*/

#define TRUE 1
#define FALSE 0

#define VERSION	"1.0"

/*----------------------------------------------------------------------*/
/* enum for ADC channels check function                                 */
typedef enum
{
	S_NORMAL,
	S_ERROR_ZONE1,
	S_ERROR_ZONE2,
	S_ALARM_ZONE1,
	S_ALARM_ZONE2,
	S_ALARM_ALL,
	S_ERROR_ALL
}DEVICE_STATE_TypeDef;

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


/* struct for device general mode and configuration                     */
typedef union
{
	struct
	{
	uint32_t serial_baudrate;                /*скорость RS-485          */
	uint16_t zone_0_treshold;                /*порог срабатывания зоны 1*/
	uint16_t zone_1_treshold;                /*порог срабатывания зоны 2*/
	uint16_t zone_0_timeint;                 /*временной интервал зоны 1*/
	uint16_t zone_1_timeint;                 /*временной интервал зоны 2*/
	uint16_t zone_0_triglimit;               /*граничное значение 
                                               превышений порога за 
                                               временной интервал зоны 1*/
	uint16_t zone_1_triglimit;               /*граничное значение 
                                               превышений порога за 
                                               временной интервал зоны 2*/
	}data;
	uint32_t array[4];                                /* блок 16 байт   */
}CONFIG_TypeDef;

/* struct for adc values from zone amplifiers                           */
typedef struct{
	
	uint16_t alrm_0; /*ch 1*/
	uint16_t alrm_1; /*ch 2*/
	uint16_t sign_0; /*ch 4*/
	uint16_t sign_1; /*ch 5*/
	
}ADC_VALUES_TypeDef;

/* struct for adc debug output through serial port                      */
typedef struct {
	
	char ch_1[4];
	char ch_2[4];
	char ch_3[4];
	char ch_4[4];
	
}ADC_CHANNELS_TypeDef;

typedef struct{
	uint8_t system_priority;	/*system layer tasks                    */
	uint8_t user_priority;		/*user layer tasks                      */
	uint8_t serial_priority;	/*transmit data tasks                   */
	uint_least16_t stack_system;/*stack size for system tasks           */
	uint_least16_t stack_serial;/*stack size for data transmit/receive  */
	uint_least16_t stack_user;	/*stack for user layer tasks and threads*/
}MEM_ALLOCATION_TypeDef;


typedef enum
{
	NORMAL,
	IDLE,
	PROGRAMMING_SS,
	ALARM,
	DEBUG,
	FAULT
}DEVICE_MODE;
/*----------------------------------------------------------------------*/
#define ZONE_0_F	ADC_VALUES.sign_0
#define ZONE_1_F	ADC_VALUES.sign_1

#define ZONE_0_RELAY_DELAY	2000
#define ZONE_1_RELAY_DELAY	2000

/*----------------------------------------------------------------------*/

extern ADDRESS_TypeDef ADDRESS;
extern MODE_TypeDef MODE;
extern OUTPUTS_TypeDef OUTPUTS;
extern CONFIG_TypeDef CONFIG;
extern ADC_VALUES_TypeDef ADC_VALUES;
extern ADC_CHANNELS_TypeDef ADC_CHANNELS;

/* global pointer to serial port peripheral module */
extern USART_TypeDef * serial_pointer;

/*queue for service stream input*/
extern xQueueHandle service_serial_queue;
/*queue for kso stream input*/
extern xQueueHandle kso_serial_queue;
/*mutex for save serial transmit function*/
extern xSemaphoreHandle xMutex_serial_BUSY;
/*semaphore for updating the status of relay outputs and indication*/
extern xSemaphoreHandle xSemph_state_UPDATE;

/*----------------------------------------------------------------------*/
extern DEVICE_MODE mode;
extern DEVICE_STATE_TypeDef global_state;
/*----------------------------------------------------------------------*/
int CheckTamperPin( void );
void GetHwAdrState( ADDRESS_TypeDef* state );
void GetHwModeState( MODE_TypeDef *state );
void GetHwOutState ( OUTPUTS_TypeDef* state );
uint16_t adc_covert_from_mv( uint16_t value );
uint16_t adc_covert_to_mv( uint16_t value );
uint8_t Crc8( uint8_t *pcBlock, uint8_t len );
void ModeUpdate( DEVICE_MODE new_mode );
uint8_t setup_serial_speed( CONFIG_TypeDef* config, uint32_t speed );
#endif
/****************************end of file ********************************/
