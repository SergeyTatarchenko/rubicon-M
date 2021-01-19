/************************************************************************
* File Name          : rubicon_777.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : заголовок для rubicon_777.c
*************************************************************************/
#ifndef RYBICON_777_H_
#define	RYBICON_777_H_

#include "serial.h"
#include "mcu_config.h"
#include "global.h"
/*----------------------------------------------------------------------*/
#define SYN (uint8_t)0xE6
#define STX (uint8_t)0x02

#define HEADER_LEN	3	/* заголовок  - 3 байта*/
#define CONTROL_LEN	3	/* управление - 3 байта*/
#define CRC_LEN		1	/* контрольная сумма - 1 байт*/

#define DATA_ANSWER_SIZE	18 /*18 байт фиксированная длина поля data при ответе*/
#define DATA_CONTROL_SIZE	12 /*12 байт фиксированная длина поля data при приеме*/

#define VPU_MAX_DATA_LENGHT	52	/*маскимальная длина поля data 52 байта */
/*----------------------------------------------------------------------*/

#pragma pack(push,1)
typedef struct
{
	struct
	{
		uint8_t network_id;
		uint8_t command;
		uint8_t data_lenght;
	}control;
	uint8_t array[DATA_CONTROL_SIZE];
	uint8_t __CRC;
	
}RUBICON_CONTROL_MESSAGE_TypeDef;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	struct
	{
		uint8_t zero_byte;
		uint8_t syn;
		uint8_t stx;
	}header;
	struct
	{
		uint8_t network_id;
		uint8_t command;
		uint8_t data_lenght;
	}control;
	uint8_t data[DATA_ANSWER_SIZE];
	uint8_t __CRC;
	
}RUBICON_ANSWER_MESSAGE_TypeDef;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	struct
	{
		uint8_t zero_byte;
		uint8_t syn;
		uint8_t stx;
	}header;
	struct
	{
		uint8_t network_id;
		uint8_t command;
		uint8_t data_lenght;
	}control;
	uint8_t data[VPU_MAX_DATA_LENGHT + 1];

}RUBICON_VPU_MESSAGE_TypeDef;
#pragma pack(pop)
/*----------------------------------------------------------------------*/
void rubicon_send_answ( void );
void rubicon_to_vpu( void );
#endif
/****************************end of file ********************************/
