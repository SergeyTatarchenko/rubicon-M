/******************************************************************************
* File Name          : serial.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : serial port module for stm32
*******************************************************************************/
#include "serial.h"
#include "global.h"
 
/*terminal commands version 0.1*/

/*user commands for array compare*/
static const char user_commands[NUM_OF_COMMANDS][COMMAND_BUF_SIZE] = 
{
	{__EXIT},
	{__HELP},
	{__SAVE},
	{__SHOW},
	{__SET},
	{__DEBUG}
};

/*user args for array compare*/
static const char user_arguments[NUM_OF_ARGUMENTS][COMMAND_BUF_SIZE] = 
{
	{__MODE},
	{__ADDRESS},
	{__STATE},
	{__CONFIG},
	{__TRESHOLD1},
	{__TRESHOLD2},
	{__TIMEINT1},
	{__TIMEINT2},
	{__TRIGLIMIT1},
	{__TRIGLIMIT2}
};

/*help textblock for print with terminal*/
static const char help_text[20][__STRLEN]=
{
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
	{__HELP_BLOCK_14},
	{__HELP_BLOCK_15},
	{__HELP_BLOCK_16},
	{__HELP_BLOCK_17},
	{__HELP_BLOCK_18},
	{__HELP_BLOCK_19}
	
};
/*welcome textblock for print with terminal*/
static const char welcome_text[8][__STRLEN] = 
{
	{__WELCOME_BLOCK_0},
	{__WELCOME_BLOCK_1},
	{__WELCOME_BLOCK_2},
	{__WELCOME_BLOCK_3},
	{__WELCOME_BLOCK_4},
	{__WELCOME_BLOCK_5},
	{__WELCOME_BLOCK_6},
	{__WELCOME_BLOCK_7}
};
/*
* name : terminal_print_txtblock
* description : print text block from two-dimm array (STATIC only!!, lenght defined as __STRLEN)
*/
void terminal_print_txtblock(const char ( *buff )[__STRLEN], size_t strnum)
{
	for(int i = 0; i < strnum ; i++)
	{
		mprintf(buff[i]);
	}
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
	return out;
}
/* name: serial_command_executor
*  descriprion: execute commands from terminal in service serial task
*/
void serial_command_executor (TCmdTypeDef command)
{
	switch(command.command)
	{
		/*exit from programming mode*/
		case C_EXIT:
			mode = NORMAL;
			mprintf(__OUT_MESSAGE);
			mprintf(__POSLINE);
		break;
		/*print help textblock*/
		case C_HELP:
			mprintf(__POSLINE);
			terminal_print_txtblock(help_text,20);
			mprintf(__POSLINE);
			break;
		/*save current config to flash
		WARNING!!! CPU stop during flash programming*/
		case C_SAVE:
			flash_data_write(CONFIG_FLASH_ADDRESS,CONFIG_SECTOR_NUMBER,CONFIG.array,sizeof(CONFIG));
			mprintf("ok\r\n");
			break;
		/*show something (see argument)*/
		case C_SHOW:
			switch(command.argument)
			{
				case A_ADDRESS:
					serial_print_adress();
					break;
				case A_MODE:
					serial_print_mode();
					break;
				case A_STATE:
					serial_print_state();
					break;
				case A_CONFIG:
					serial_print_config();
					break;
				default:
					mprintf(__ERROR_MESSAGE);
					mprintf(__NEWLINE);
					break;
			}
			break;
		/*set value for some parameter (see argument)*/
		case C_SET:
			switch(command.argument)
			{
				case A_TIMEINT1:
					if(command.value != 0)
					{
						CONFIG.data.zone_0_timeint = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TIMEINT2:
					if(command.value != 0)
					{
						CONFIG.data.zone_1_timeint = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRESHOLD1:
					if(command.value != 0)
					{
						CONFIG.data.zone_0_treshold = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRESHOLD2:
					if(command.value != 0)
					{
						CONFIG.data.zone_1_treshold = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRIGLIMIT1:
					if(command.value != 0)
					{
						CONFIG.data.zone_0_triglimit = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				case A_TRIGLIMIT2:
					if(command.value != 0)
					{
						CONFIG.data.zone_1_triglimit = command.value;
						mprintf("ok\r\n");
					}
					else
					{
						mprintf(__ERROR_MESSAGE);
						mprintf(__NEWLINE);
					}
					break;
				default:
					mprintf(__ERROR_MESSAGE);
					mprintf(__NEWLINE);
					break;
			}
			break;
		case C_DEBUG:
			mode = DEBUG;
			mprintf(__POSLINE);
			mprintf("current adc values : \r\n");
			break;
		/*error in input*/
		case C_ERROR:
			mprintf(__ERROR_MESSAGE);
			mprintf(__NEWLINE);
			break;
		default:
			break;
	}
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
void __attribute__((weak)) serial_send_byte(USART_TypeDef* port,const char byte)
{
}

/* name: serial_send_byte
*  descriprion: template for serial port send array for io implementation
*/
void __attribute__((weak)) serial_send_array(const char *array,int size)
{
}


/**********************
LOCAL PRINT FUNCTIONS
**********************/
/* name: serial_debug_output
*  descriprion: debug output from ADC channels
*/
void serial_debug_output( void )
{
	itoa(ADC_VALUES.alrm_0,ADC_CHANNELS.ch_1,4);
	itoa(ADC_VALUES.alrm_1,ADC_CHANNELS.ch_2,4);
	itoa(ADC_VALUES.sign_0,ADC_CHANNELS.ch_4,4);
	itoa(ADC_VALUES.sign_1,ADC_CHANNELS.ch_5,4);
	for(int i = 0;i < 4; i++)
	{
		ADC_CHANNELS.ch_1[i]+=0x30;
		ADC_CHANNELS.ch_2[i]+=0x30;
		ADC_CHANNELS.ch_4[i]+=0x30;
		ADC_CHANNELS.ch_5[i]+=0x30;
	}
	mprintf(" adc ch 1 : ");
	cprintf(ADC_CHANNELS.ch_1,4);
	mprintf(" adc ch 2 : ");
	cprintf(ADC_CHANNELS.ch_2,4);
	mprintf(" adc ch 4 : ");
	cprintf(ADC_CHANNELS.ch_4,4);
	mprintf(" adc ch 5 : ");
	cprintf(ADC_CHANNELS.ch_5,4);
	mprintf("\r");
	
}

void serial_print_adress()
{
	char address_txt[3];
	itoa(ADDRESS.byte,address_txt,3);
	mprintf(__POSLINE);
	mprintf("Текущий адрес устройства в сети : ");
	
	for(int i = 0; i < 3; i++)
	{
		address_txt[i]+=0x30;
	}
	mprintf(address_txt);
	mprintf(__NEWLINE);
	mprintf(__POSLINE);
}

void serial_print_state( void )
{
	mprintf(__POSLINE);
	mprintf("Текущее состояние зон:\r\n");
	mprintf(" Зона 1 тревога : ");
	
	if(OUTPUTS.bit.zone_0_alarm == 1)
	{
		mprintf("Да");
	}
	else
	{
		mprintf("Нет");
	}
	mprintf(__NEWLINE);
	mprintf(" Зона 1 ошибка  : ");
	if(OUTPUTS.bit.zone_0_error == 1)
	{
		mprintf("Да");
	}
	else
	{
		mprintf("Нет");
	}
	mprintf(__NEWLINE);
	mprintf(" Зона 2 тревога : ");
	
	if(OUTPUTS.bit.zone_1_alarm == 1)
	{
		mprintf("Да");
	}
	else
	{
		mprintf("Нет");
	}
	mprintf(__NEWLINE);
	mprintf(" Зона 2 ошибка  : ");
	if(OUTPUTS.bit.zone_1_error == 1)
	{
		mprintf("Да");
	}
	else
	{
		mprintf("Нет");
	}
	mprintf(__NEWLINE);
	mprintf(__POSLINE);
}


void serial_print_config()
{
	const int array_size = 5;
	char array[array_size];
	mprintf(__NEWLINE);
	mprintf(__POSLINE);
	mprintf("ТЕКУЩАЯ КОНФИГУРАЦИЯ СИСТЕМЫ :\r\n");
	/********************************************/
	mprintf(__NEWLINE);
	itoa(CONFIG.data.zone_0_timeint,array,array_size);
	mprintf("Временное окно зоны 1, с : ");
	for(int i = 0; i < array_size; i++)
	{
		array[i]+=0x30;
	}
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	mprintf(__NEWLINE);
	itoa(CONFIG.data.zone_1_timeint,array,array_size);
	mprintf("Временное окно зоны 2, с : ");
	for(int i = 0; i < array_size; i++)
	{
		array[i]+=0x30;
	}
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	mprintf(__NEWLINE);
	/********************************************/
	mprintf(__NEWLINE);
	itoa(CONFIG.data.zone_0_treshold,array,array_size);
	mprintf("Порог срабатывания зоны 1, мВ : ");
	for(int i = 0; i < array_size; i++)
	{
		array[i]+=0x30;
	}
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	mprintf(__NEWLINE);
	itoa(CONFIG.data.zone_1_treshold,array,array_size);
	mprintf("Порог срабатывания зоны 2, мВ : ");
	for(int i = 0; i < array_size; i++)
	{
		array[i]+=0x30;
	}
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	mprintf(__NEWLINE);
	/********************************************/
	mprintf(__NEWLINE);
	itoa(CONFIG.data.zone_0_triglimit,array,array_size);
	mprintf("Предельное число срабатываний за интервал зоны 1 : ");
	for(int i = 0; i < array_size; i++)
	{
		array[i]+=0x30;
	}
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	mprintf(__NEWLINE);
	itoa(CONFIG.data.zone_1_triglimit,array,array_size);
	mprintf("Предельное число срабатываний за интервал зоны 2 : ");
	for(int i = 0; i < array_size; i++)
	{
		array[i]+=0x30;
	}
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	mprintf(__NEWLINE);
	mprintf(__POSLINE);
}

void serial_print_mode()
{
	mprintf(__POSLINE);
	mprintf("Текущее состояние системы:\r\n");
	mprintf(" Контроль зоны 1 : ");
	
	if(MODE.bit.zone0_enable == 1)
	{
		mprintf("Активен");
	}
	else
	{
		mprintf("Отключен");
	}
	mprintf(__NEWLINE);
	
	mprintf(" Режим работы зоны 1 : ");
	if(MODE.bit.zone0_mode == 0)
	{
		mprintf("Легкий тип ограждения");
	}
	else
	{
		mprintf("Тяжелый тип ограждения");
	}
	mprintf(__NEWLINE);
	mprintf(" Контроль зоны 2 : ");
	
	if(MODE.bit.zone1_enable == 1)
	{
		mprintf("Активен");
	}
	else
	{
		mprintf("Отключен");
	}
	mprintf(__NEWLINE);
	
	mprintf(" Режим работы зоны 2 : ");
	if(MODE.bit.zone1_mode == 0)
	{
		mprintf("Легкий тип ограждения");
	}
	else
	{
		mprintf("Тяжелый тип ограждения");
	}
	mprintf(__NEWLINE);
	mprintf(__POSLINE);
}

void serial_print_welcome()
{
	terminal_print_txtblock(welcome_text,8);
}
