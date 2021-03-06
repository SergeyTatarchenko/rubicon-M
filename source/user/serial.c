/******************************************************************************
* File Name          : serial.c
* Author             : Tatarchenko S.
* Version            : v 1.1
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
	{__DEBUG},
	{__PING}
};

/*user args for array compare*/
static const char user_arguments[NUM_OF_ARGUMENTS][COMMAND_BUF_SIZE] = 
{
	{__MODE},
	{__ADDRESS},
	{__STATE},
	{__CONFIG},
	{__BAUDRATE},
	{__CTRESHOLD1},
	{__CTRESHOLD2},
	{__BTRESHOLD1},
	{__BTRESHOLD2},
	{__CTIMEINT1},
	{__CTIMEINT2},
	{__BTIMEINT1},
	{__BTIMEINT2},
	{__CTRIGLIMIT1},
	{__CTRIGLIMIT2},
	{__BTRIGLIMIT1},
	{__BTRIGLIMIT2},
	{__DEFCONFIG}
};

/*help textblock for print with terminal*/
static const char help_text[21][__STRLEN]=
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
	{__HELP_BLOCK_19},
	{__HELP_BLOCK_20},
	
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

uint8_t GetAddressFromBuf(char * buff)
{
	char temp[COMMAND_BUF_SIZE] = {0};
	int message_lenght = strlen(buff);
	int address_lenght = 0,address_int = 0;
	
	GetHwAdrState(&ADDRESS);
	address_int = atoi(buff);
	if((ADDRESS.byte == address_int)&&(ADDRESS.byte != 0))
	{
		for(; address_lenght < message_lenght; address_lenght++)
		{
			if(buff[address_lenght] == __WHITESPACE)
			{
				break;
			}
		}
		address_lenght++;
		memcpy(temp,&buff[address_lenght],message_lenght);
		memset(buff,0,COMMAND_BUF_SIZE);
		memcpy(buff,temp,message_lenght);
		return TRUE;
	}
	else
	{
		return FALSE;
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
		if(!strcmp(command,user_commands[i]))
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
			if(!strcmp(arg_1,user_arguments[i]))
			{
				out.argument = (ARGUMENTS)i;
				break;
			}
			else
			{
				out.argument = A_ERROR;
			}
		}
		/*copy command values*/
		if(out.command == C_SET && out.argument == A_CONFIG)
		{
			/*костыль для общего конфига*/
			for(int j = 0; j < 13; j++)
			{
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
					out.values[j] = atoi(arg_2);
					out.num_of_values++;
					memset(arg_2,0,sizeof(arg_2));
				}
			}
		}
		else
		{
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
				out.values[0] = atoi(arg_2);
				out.num_of_values = 1;
			}
		}
	}
	return out;
}
/* name: serial_command_executor
*  descriprion: execute commands from terminal in service serial task
*/
void serial_command_executor (TCmdTypeDef command, const char *buff)
{
	char answer[COMMAND_BUF_SIZE] = {0};
	uint8_t state = FALSE;
	if((command.command != C_PING)&&(command.command != C_EXIT)&&(command.command != C_SAVE))
	{
		sprintf(answer,"%d %s\r\n",ADDRESS.byte,buff);
		mprintf(answer);
	}
	else
	{
		sprintf(answer,"%d ok %s\r\n",ADDRESS.byte,buff);
		mprintf(answer);
	}
	switch(command.command)
	{
		/*exit from programming mode*/
		case C_EXIT:
			mode = NORMAL;
			break;
		/*print help textblock*/
		case C_HELP:
			mprintf(__POSLINE);
			terminal_print_txtblock(help_text,21);
			mprintf(__POSLINE);
			break;
		/*save current config to flash
		WARNING!!! CPU stop during flash programming*/
		case C_SAVE:
			flash_data_write(CONFIG_FLASH_ADDRESS,CONFIG_SECTOR_NUMBER,CONFIG.array,sizeof(CONFIG));
			break;
		/*show something (see argument)*/
		case C_SHOW:
			switch(command.argument)
			{
				case A_ADDRESS:
					serial_print_address();
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
					mprintf("error\r\n");
					break;
			}
			break;
		/*set value for some parameter (see argument)*/
		case C_SET:
			switch(command.argument)
			{
  				case A_CTIMEINT1:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_0_climb_timeint = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BTIMEINT1:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_0_cut_timeint = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_CTIMEINT2:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_1_climb_timeint = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BTIMEINT2:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_1_cut_timeint = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_CTRESHOLD1:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_0_climb_treshold = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BTRESHOLD1:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_0_cut_treshold = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_CTRESHOLD2:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_1_climb_treshold = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BTRESHOLD2:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_1_cut_treshold = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_CTRIGLIMIT1:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_0_climb_triglimit = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BTRIGLIMIT1:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_0_cut_triglimit = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_CTRIGLIMIT2:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_1_climb_triglimit = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BTRIGLIMIT2:
					if(command.values[0] != 0)
					{
						CONFIG.data.zone_1_cut_triglimit = command.values[0];
						mprintf("ok\r\n");
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_BAUDRATE:
					if(command.values[0] != 0)
					{
						state = SetupSerialSpeed(&CONFIG,command.values[0]);
						if(state == TRUE)
						{
							mprintf("ok\r\n");
						}
						else
						{
							mprintf("error\r\n");
						}
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_CONFIG:
					if(command.values[0] != 0)
					{
						SetConfig(command.values);
					}
					else
					{
						mprintf("error\r\n");
					}
					break;
				case A_DEFCONFIG:
					SetDefConfig();
					break;
				default:
					mprintf("error\r\n");
					break;
			}
			break;
		case C_DEBUG:
			mode = DEBUG;
			mprintf(__POSLINE);
			mprintf("current adc values : \r\n");
			break;
		case C_PING:
			break;
		/*error in input*/
		case C_ERROR:
			mprintf("error\r\n");
			break;
		default:
			break;
	}
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

void SetConfig( int *arr )
{
	for(int i = 0; i < 13; i++)
	{
		if(arr[i] == 0)
		{
			mprintf("error\r\n");
			return;
		}
	}
	CONFIG.data.serial_baudrate = arr[0];
	CONFIG.data.zone_0_climb_timeint = arr[1];
	CONFIG.data.zone_0_cut_timeint   = arr[2];
	CONFIG.data.zone_1_climb_timeint = arr[3];
	CONFIG.data.zone_1_cut_timeint   = arr[4];
	
	CONFIG.data.zone_0_climb_triglimit = arr[5];
	CONFIG.data.zone_0_cut_triglimit   = arr[6];
	CONFIG.data.zone_1_climb_triglimit = arr[7];
	CONFIG.data.zone_1_cut_triglimit   = arr[8];
	
	CONFIG.data.zone_0_climb_treshold = arr[9];
	CONFIG.data.zone_0_cut_treshold   = arr[10];
	CONFIG.data.zone_1_climb_treshold = arr[11];
	CONFIG.data.zone_1_cut_treshold   = arr[12];
	mprintf("ok\r\n");
}

/**********************
LOCAL PRINT FUNCTIONS
**********************/
/* name: serial_debug_output
*  descriprion: debug output from ADC channels
*/
void serial_debug_output( void )
{
	sprintf(ADC_CHANNELS.ch_1,"%d",ADC_VALUES.alrm_0);
	sprintf(ADC_CHANNELS.ch_2,"%d",ADC_VALUES.alrm_1);
	sprintf(ADC_CHANNELS.ch_3,"%d",ADC_VALUES.sign_0);
	sprintf(ADC_CHANNELS.ch_4,"%d",ADC_VALUES.sign_1);
	
	mprintf(" adc ch 1 : ");
	cprintf(ADC_CHANNELS.ch_1,4);
	mprintf(" adc ch 2 : ");
	cprintf(ADC_CHANNELS.ch_2,4);
	mprintf(" adc ch 4 : ");
	cprintf(ADC_CHANNELS.ch_3,4);
	mprintf(" adc ch 5 : ");
	cprintf(ADC_CHANNELS.ch_4,4);
	mprintf("\r");
	
}

void serial_print_address()
{
	const int array_size = MAX_VALUE_LENGHT + 1;
	char array[array_size];
	sprintf(array,"%d\r\n",ADDRESS.byte);
	mprintf(array);
	memset(array,0,array_size);
}

void SetDefConfig()
{
	CONFIG.data.zone_0_climb_timeint   = 3;
	CONFIG.data.zone_0_climb_treshold  = 2000;
	CONFIG.data.zone_0_climb_triglimit = 5;
	
	CONFIG.data.zone_1_climb_timeint   = 3;
	CONFIG.data.zone_1_climb_treshold  = 2000;
	CONFIG.data.zone_1_climb_triglimit = 5;
	
	CONFIG.data.zone_0_cut_timeint   = 3;
	CONFIG.data.zone_0_cut_treshold  = 2000;
	CONFIG.data.zone_0_cut_triglimit = 5;
	
	CONFIG.data.zone_1_cut_timeint   = 3;
	CONFIG.data.zone_1_cut_treshold  = 2000;
	CONFIG.data.zone_1_cut_triglimit = 5;
	
}


void serial_print_state( void )
{
}


void serial_print_config()
{
	const int array_size = MAX_VALUE_LENGHT + 1;
	char array[array_size];
	/********************************************/
	sprintf(array,"%d\r\n",CONFIG.data.serial_baudrate);
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	sprintf(array,"%d\r\n",CONFIG.data.zone_0_climb_timeint);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_0_cut_timeint);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_1_climb_timeint);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_1_cut_timeint);
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	sprintf(array,"%d\r\n",CONFIG.data.zone_0_climb_triglimit);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_0_cut_triglimit);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_1_climb_triglimit);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_1_cut_triglimit);
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	sprintf(array,"%d\r\n",CONFIG.data.zone_0_climb_treshold);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_0_cut_treshold);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_1_climb_treshold);
	mprintf(array);
	memset(array,0,array_size);
	sprintf(array,"%d\r\n",CONFIG.data.zone_1_cut_treshold);
	mprintf(array);
	memset(array,0,array_size);
}

void serial_print_mode()
{
	const int array_size = MAX_VALUE_LENGHT + 1;
	char array[array_size];
	/********************************************/
	sprintf(array,"%d\r\n",MODE.bit.zone0_enable);
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	sprintf(array,"%d\r\n",MODE.bit.zone1_enable);
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	sprintf(array,"%d\r\n",MODE.bit.zone0_mode);
	mprintf(array);
	memset(array,0,array_size);
	/********************************************/
	sprintf(array,"%d\r\n",MODE.bit.zone1_mode);
	mprintf(array);
	memset(array,0,array_size);
}

void serial_print_welcome()
{
	terminal_print_txtblock(welcome_text,8);
}
