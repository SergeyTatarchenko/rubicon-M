/******************************************************************************
* File Name          : serial.h
* Author             : Tatarchenko S.
* Version            : v 1.1
* Description        : header for serial.c 
*******************************************************************************/
#ifndef SERIAL_H
#define SERIAL_H

/*-----------------------------includes---------------------------------------*/
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "text.h"
#include "stm32f2xx.h"
/*-------------------------global defines-------------------------------------*/

/*terminal commands version 0.1*/

#define COMMAND_BUF_SIZE		32	/*32 byte */
#define SERVICE_COMMBUFF_SIZE	5	/*two commands in queue*/
#define NUM_OF_COMMANDS			7
#define NUM_OF_ARGUMENTS		18
#define MAX_VALUE_LENGHT		5

/*enum of user commands*/
typedef enum {
	C_EXIT,
	C_HELP,
	C_SAVE,
	C_SHOW,
	C_SET,
	C_DEBUG,
	C_PING,
	C_ERROR
}COMMANDS;

/*enum of user arguments*/
typedef enum{
	A_MODE,		/* запрос текущего режима устройства */
	A_ADDRESS,
	A_STATE,
	A_CONFIG,
	A_BAUDRATE,
	A_CTRESHOLD1,
	A_CTRESHOLD2,
	A_BTRESHOLD1,
	A_BTRESHOLD2,
	A_CTIMEINT1,
	A_CTIMEINT2,
	A_BTIMEINT1,
	A_BTIMEINT2,
	A_CTRIGLIMIT1,
	A_CTRIGLIMIT2,
	A_BTRIGLIMIT1,
	A_BTRIGLIMIT2,
	A_DEFCONFIG,
	A_ERROR
}ARGUMENTS;

/*typedef output for user command for terminal */
typedef struct {
	COMMANDS command;
	ARGUMENTS argument;
	int value;
}TCmdTypeDef;

/*---------------------local function prototypes------------------------------*/

void serial_send_byte(USART_TypeDef* port,const char byte);
void serial_send_array(const char *array,int size);

TCmdTypeDef command_processing(char* buff);
int command_parser(char* buff_1,const char* buff_2);
uint8_t GetAddressFromBuf(char * buff);

void terminal_print_txtblock( const char ( *buff )[__STRLEN], size_t strnum);
void serial_print_state( void );
void serial_print_address( void );
void SetDefConfig( void );
void serial_print_config( void );
void serial_print_mode( void );
void serial_print_welcome( void );
void serial_debug_output( void );
void serial_command_executor ( TCmdTypeDef command , const char *buff );

#endif
/****************************end of file ********************************/
