/******************************************************************************
* File Name          : serial.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : serial port module for stm32
*******************************************************************************/
#include "serial.h"
#include "global.h"
 
/*terminal commands version 0.1*/

char reflection_byte;
char combuff[COMMAND_BUF_SIZE];

/*user commands for array compare*/
static const char user_commands[NUM_OF_COMMANDS][COMMAND_BUF_SIZE] = {
{__EXIT},
{__HELP},
{__SAVE},
{__SHOW},
{__SET},
{__DEBUG}
};

/*user args for array compare*/
static const char user_arguments[NUM_OF_ARGUMENTS][COMMAND_BUF_SIZE] = {
{__MODE},
{__ADDRESS},
{__STATE},
{__TRESHOLD1},
{__TRESHOLD2},
{__TIMEINT1},
{__TIMEINT2}
};

/*help textblock for print with terminal*/
static const char help_text[14][__STRLEN]={
{__HELP_BLOCK_0},
{__HELP_BLOCK_1},
{__HELP_BLOCK_2},
{__HELP_BLOCK_3},
{__HELP_BLOCK_4},
{__HELP_BLOCK_5},
{__HELP_BLOCK_6},
{__HELP_BLOCK_7},
{__HELP_BLOCK_8},
{__HELP_BLOCK_9},
{__HELP_BLOCK_10},
{__HELP_BLOCK_11},
{__HELP_BLOCK_12},
{__HELP_BLOCK_13},
}; 

/*
* name : terminal_print_txtblock
* description : print text block from two-dimm array (STATIC only!!, lenght defined as __STRLEN)
*/
void terminal_print_txtblock(const char ( *buff )[__STRLEN], size_t strnum)
{
	for(int i = 0; i < strnum ; i++)
	{
		cprintf(buff[i]);
	}
}


void serial_print_adress()
{
	char address_txt[3];
	itoa(ADRESS.byte,address_txt,3);
	cprintf(__POSLINE);
	cprintf("Текущий адрес устройства в сети : ");
	for(int i = 0; i < 3; i++)
	{
		serial_send_byte(address_txt[i] +0x30);
	}
	cprintf(__NEWLINE);
	cprintf(__POSLINE);
}

void serial_print_state( void )
{
	cprintf(__POSLINE);
	cprintf("Текущее состояние зон:\r\n");
	cprintf(" Зона 1 тревога : ");
	
	if(OUTPUTS.bit.zone_0_alarm == 1)
	{
		cprintf("Да");
	}
	else
	{
		cprintf("Нет");
	}
	cprintf(__NEWLINE);
	cprintf(" Зона 1 ошибка  : ");
	if(OUTPUTS.bit.zone_0_error == 1)
	{
		cprintf("Да");
	}
	else
	{
		cprintf("Нет");
	}
	cprintf(__NEWLINE);
	cprintf(" Зона 2 тревога : ");
	
	if(OUTPUTS.bit.zone_1_alarm == 1)
	{
		cprintf("Да");
	}
	else
	{
		cprintf("Нет");
	}
	cprintf(__NEWLINE);
	cprintf(" Зона 2 ошибка  : ");
	if(OUTPUTS.bit.zone_1_error == 1)
	{
		cprintf("Да");
	}
	else
	{
		cprintf("Нет");
	}
	cprintf(__NEWLINE);
	cprintf(__POSLINE);
}

void serial_print_mode()
{
	cprintf(__POSLINE);
	cprintf("Текущее состояние системы:\r\n");
	cprintf(" Контроль зоны 1 : ");
	
	if(MODE.bit.zone0_enable == 0)
	{
		cprintf("Активен");
	}
	else
	{
		cprintf("Отключен");
	}
	cprintf(__NEWLINE);
	
	cprintf(" Режим работы зоны 1 : ");
	if(MODE.bit.zone0_mode == 1)
	{
		cprintf("Легкий тип ограждения");
	}
	else
	{
		cprintf("Тяжелый тип ограждения");
	}
	cprintf(__NEWLINE);
	cprintf(" Контроль зоны 2 : ");
	
	if(MODE.bit.zone1_enable == 0)
	{
		cprintf("Активен");
	}
	else
	{
		cprintf("Отключен");
	}
	cprintf(__NEWLINE);
	
	cprintf(" Режим работы зоны 2 : ");
	if(MODE.bit.zone1_mode == 1)
	{
		cprintf("Легкий тип ограждения");
	}
	else
	{
		cprintf("Тяжелый тип ограждения");
	}
	cprintf(__NEWLINE);
	cprintf(__POSLINE);
}

/*
* name : command_processing
* description : user input command parser;
* structure : [COMMAND] [ARGUMENT] [VALUE]
*/
TCmdTypeDef command_processing(char* buff)
{
	int num_of_symbols = 0;
	int CmdWithArg = 0;
	char command[COMMAND_BUF_SIZE] = {0},
	arg_1[COMMAND_BUF_SIZE] = {0},
	arg_2[COMMAND_BUF_SIZE] = {0};
	int counter = 0;
	TCmdTypeDef out;
	
	out.command = C_ERROR;
	out.argument = A_ERROR;
	
	/*whitespace check*/
	for(int i = 0; i < COMMAND_BUF_SIZE; i++)
	{
		/*input command has an args*/
		if(buff[i] == __WHITESPACE)
		{
			CmdWithArg = 1;
			break;
		}
		num_of_symbols ++;
	}
	/*copy command word to buffer*/
	for(int i = 0; i < COMMAND_BUF_SIZE; i++)
	{
		if((buff[i] == 0) ||(buff[i] == ' '))
		{
			break;
		}
		else
		{
			command[i] = buff[i];
		}
	}
	/*get command number from enum*/
	for(int i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if(command_parser(command,user_commands[i]))
		{
			out.command = (COMMANDS)i;
			break;
		}
		else
		{
			out.command = C_ERROR;
		}
	}
	/*if command has args*/
	if(CmdWithArg)
	{
		while(counter < COMMAND_BUF_SIZE)
		{
			if(buff[counter] == ' ')
			{
				break;
			}
			else
			{
				counter++;
			}
		}
		counter++;
		/*copy command argument 1*/
		for(int i = 0; counter < COMMAND_BUF_SIZE; i++)
		{
			if((buff[counter] == ' ')||(buff[counter] == 0))
			{
				break;
			}
			else
			{
				arg_1[i] = buff[counter];
				counter++;
			}
		}
		/*get argument number from enum*/
		for(int i = 0; i < NUM_OF_ARGUMENTS; i++)
		{
			if(command_parser(arg_1,user_arguments[i]))
			{
				out.argument = (ARGUMENTS)i;
				break;
			}
			else
			{
				out.argument = A_ERROR;
			}
		}
		/*copy command argument 2*/
		if((counter < COMMAND_BUF_SIZE) && (buff[counter]!= 0))
		{
			counter++;
			for(int i = 0; counter < COMMAND_BUF_SIZE; i++)
			{
				if((buff[counter] == ' ')||(buff[counter] == 0))
				{
					break;
				}
				else
				{
					arg_2[i] = buff[counter];
					counter++;
				}
			}
			out.value = chatoi(arg_2,COMMAND_BUF_SIZE);
		}
	}
	switch(out.command)
	{
		case C_HELP:
			cprintf(__POSLINE);
			terminal_print_txtblock(help_text,14);
			cprintf(__POSLINE);
			break;
		
		default:
			break;
	}
	return out;
}

/*
* name : command_parser
* description : compare user string with array, return 1 if it match
*/
int command_parser(char* buff_1,const char* buff_2)
{
	int command_matced = 1;
	for(int i = 0; i < COMMAND_BUF_SIZE ; i ++)
	{
		if(buff_1[i] != buff_2[i])
		{
			command_matced = 0;
			break;
		}
	}
	return command_matced;
}

/*
* name : cprintf
* description : serial port write, use CPU, single byte interrupt
*/
int cprintf (const char *format,...)
{
    uint8_t i = 0;
    do
    {
        serial_send_byte(format[i]);
        i++;
    }
    while(format[i]!= '\0');
    
    return 0; 
}

/*
* name : mprintf
* description : serial port write without CPU (DMA mode,etc) write array without interrupt
*/
int mprintf (const char *format, int size)
{
	serial_send_array(format,size);
	return 0; 
}

/*
* name : itoa
* description : convert integer to array
*/
uint32_t itoa(int i,char *buff,uint8_t MesSize)
{
	int temp = i,counter = 0,offset;
	uint32_t array_size;
	if( i > 0)
	{
		while(temp != 0)
		{
			temp/=10;
			counter++;
		}
		array_size = (uint32_t)counter;
		temp = i;
		offset = MesSize - counter;
		do
		{
			buff[(counter-1)+offset] = temp % 10;
			counter--;
			temp/=10;
		}
		while(counter!=0);
		if(array_size < MesSize)
		{
			for(counter = 0; counter < (MesSize-array_size); counter++)
			{
			buff[counter] = 0;
			}
		}
	}
	else if(i == 0)
	{
		for(counter = 0; counter < MesSize; counter++)
		{
			buff[counter] = 0;
		}
	} 
	return array_size;
}

/*
* name : atoi
* description : convert array to integer
*/
uint32_t chatoi(char *buff,int size)
{
	uint32_t number = 0;
	int counter = 0;
	for(int i = 0; i < size; i++)
	{
		if(buff[i]!=0)
		{
			if((buff[i] > 0x39)||(buff[i] < 0x30))
			{
				number = 0;
				return number;
			}
			else
			{
				buff[i] = buff[i] - 0x030;
				counter++;
			}
		}
	}
	number = buff[0]*10000+buff[1]*1000+buff[2]*100+buff[3]*10+buff[4];
	for(int i = 0;i < MAX_VALUE_LENGHT - counter; i++)
	{
		number /=10;
	}
	return number;
}

/* name: serial_send_byte
*  descriprion: template for serial port send byte for io implementation
*/
void __attribute__((weak)) serial_send_byte(const char byte)
{
}

/* name: serial_send_byte
*  descriprion: template for serial port send array for io implementation
*/
void __attribute__((weak)) serial_send_array(const char *array,int size)
{
}

