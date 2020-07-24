/************************************************************************
* File Name          : text.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : text message templates for terminal
*************************************************************************/
#ifndef TEXT_H_
#define	TEXT_H_

#define __WHITESPACE 0x20
#define __STRLEN	128

#define __HEADER_MESSAGE	"Нажмите Enter для входа в режим конфигурации...\r\n"
#define __ERROR_MESSAGE		"Неверная команда, попробуйте еще раз"
#define __OUT_MESSAGE		"Выход из режима конфигурации...\r\n"

#define __HELP_BLOCK_0		"Используемые команды : \r\n"
#define __HELP_BLOCK_1		"exit - выход из режима программирования; \r\n"
#define __HELP_BLOCK_2		"help - вывод справки по командам терминала; \r\n"
#define __HELP_BLOCK_3		"show - показать значение аргумента; \r\n"
#define __HELP_BLOCK_4		"set  - установить значение аргумента; \r\n"
#define __HELP_BLOCK_5		"save - сохранение в ПЗУ текущих значений параметров; \r\n"
#define __HELP_BLOCK_6		"Используемые аргументы : \r\n"
#define __HELP_BLOCK_7		"mode      - текущий режим работы устройства; \r\n"
#define __HELP_BLOCK_8		"address   - текущий адрес устройства для КСО; \r\n"
#define __HELP_BLOCK_9		"state     - текущее состояние устройства; \r\n"
#define __HELP_BLOCK_10		"treshold1 - порог срабатывания зоны 1; \r\n"
#define __HELP_BLOCK_11		"treshold2 - порог срабатывания зоны 2; \r\n"
#define __HELP_BLOCK_12		"timeint1  - временной интервал срабатывания зоны 1;\r\n"
#define __HELP_BLOCK_13		"timeint2  - временной интервал срабатывания зоны 2;\r\n"



#define __POSLINE			"***************************************************\n\r" 
#define __NEWLINE			"\r\n"

/*commands*/
#define __EXIT	'e','x','i','t'
#define __HELP	'h','e','l','p'
#define __SAVE	's','a','v','e'
#define __SHOW	's','h','o','w'
#define __SET	's','e','t'

/*arguments*/
#define __MODE		'm','o','d','e'
#define __ADDRESS	'a','d','d','r','e','s','s'
#define __STATE		's','t','a','t','e'
#define __TRESHOLD1	't','r','e','s','h','o','l','d','1'
#define __TRESHOLD2	't','r','e','s','h','o','l','d','2'
#define __TIMEINT1	't','i','m','e','i','n','t','1'
#define __TIMEINT2	't','i','m','e','i','n','t','2'


#endif
/****************************end of file ********************************/
