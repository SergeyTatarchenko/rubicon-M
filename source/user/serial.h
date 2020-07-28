/******************************************************************************
* File Name          : serial.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : header for serial.c 
*******************************************************************************/
#ifndef SERIAL_H
#define SERIAL_H

/*-----------------------------includes---------------------------------------*/
#include "stdint.h"
#include "string.h"
#include "text.h"
/*-------------------------global defines-------------------------------------*/
/*terminal commands version 0.1*/

#define COMMAND_BUF_SIZE 32 /*32 byte */
#define NUM_OF_COMMANDS		6
#define NUM_OF_ARGUMENTS	7
#define MAX_VALUE_LENGHT	5

/*enum of user commands*/
typedef enum {
	C_EXIT,
	C_HELP,
	C_SAVE,
	C_SHOW,
	C_SET,
	C_DEBUG,
	C_ERROR
}COMMANDS;

/*enum of user arguments*/
typedef enum{
	A_MODE,
	A_ADDRESS,
	A_STATE,
	A_TRESHOLD1,
	A_TRESHOLD2,
	A_TIMEINT1,
	A_TIMEINT2,
	
	A_ERROR
}ARGUMENTS;

/*typedef output for user command for terminal */
typedef struct {
	COMMANDS command;
	ARGUMENTS argument;
	int value;
}TCmdTypeDef;



/*---------------------local function prototypes------------------------------*/

int cprintf (const char *format,...);
int mprintf (const char *format, int size);
uint32_t itoa(int i,char *buff,uint8_t MesSize);
uint32_t chatoi(char *buff,int size);

void serial_send_byte(const char byte);
void serial_send_array(const char *array,int size);

TCmdTypeDef command_processing(char* buff);
int command_parser(char* buff_1,const char* buff_2);


void terminal_print_txtblock( const char ( *buff )[__STRLEN], size_t strnum);

void serial_print_state( void );
void serial_print_adress( void );
void serial_print_mode( void );

extern char reflection_byte;
extern char combuff[COMMAND_BUF_SIZE];

#endif
/****************************end of file ********************************/
