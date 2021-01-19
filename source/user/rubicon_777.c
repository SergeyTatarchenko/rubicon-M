/************************************************************************
* File Name          : rubicon_777.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : Структуры и функции для обмена информацией с КСО
*************************************************************************/
#include "rubicon_777.h"
/*----------------------------------------------------------------------*/


/* name: rubicon_send_answ
*  descriprion: заполнение и отправка через RS-485 ответа на КСО
*/
void rubicon_send_answ( void )
{
	
}

void rubicon_to_vpu()
{
	char test[7] = {'r','u','b','i','c','o','n'};
	RUBICON_VPU_MESSAGE_TypeDef mes;
	uint8_t crc;
	mes.header.zero_byte = 0;
	mes.header.syn = SYN;
	mes.header.stx = STX;
	
	mes.control.command = 6;
	mes.control.network_id  = 3;
	mes.control.data_lenght = 28;
	mes.data[0] = 0x80;
	mes.data[1] = 1;
	mes.data[2] = 1;
	mes.data[3] = 1;
	memcpy(&mes.data[4],test,sizeof(test));
	crc = Crc8(&mes.control.network_id,14);
	mes.data[28] = crc;
	mprintf((char*)&mes);
}
/****************************end of file ********************************/
