/************************************************************************
* File Name          : text.h
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : text message templates for terminal
*************************************************************************/
#ifndef TEXT_H_
#define	TEXT_H_

#define __WHITESPACE 0x20
#define __STRLEN	256

#define __POSLINE			"***************************************************\n\r" 
#define __NEWLINE			"\r\n"


#define __HEADER_MESSAGE	"Нажмите Enter для входа в режим конфигурации...\r\n"
#define __ERROR_MESSAGE		"Неверная команда, введите help для справки"
#define __OUT_MESSAGE		"Выход из режима конфигурации...\r\n"

#define __HELP_BLOCK_0		"Используемые команды : \r\n"
#define __HELP_BLOCK_1		"exit - выход из режима программирования; \r\n"
#define __HELP_BLOCK_2		"help - вывод справки по командам терминала; \r\n"
#define __HELP_BLOCK_3		"show - показать значение аргумента; \r\n"
#define __HELP_BLOCK_4		"set  - установить значение аргумента; \r\n"
#define __HELP_BLOCK_5		"save - сохранение в ПЗУ текущих значений параметров; \r\n"
#define __HELP_BLOCK_6		"Используемые аргументы : \r\n"
#define __HELP_BLOCK_7		"mode       - текущий режим работы устройства; \r\n"
#define __HELP_BLOCK_8		"address    - текущий адрес устройства для КСО; \r\n"
#define __HELP_BLOCK_9		"state      - текущее состояние устройства; \r\n"
#define __HELP_BLOCK_10		"config     - текущая конфигурация устройства; \r\n"
#define __HELP_BLOCK_11		"treshold1  - порог срабатывания зоны 1, мВ; \r\n"
#define __HELP_BLOCK_12		"treshold2  - порог срабатывания зоны 2, мВ; \r\n"
#define __HELP_BLOCK_13		"timeint1   - временной интервал срабатывания зоны 1, с;\r\n"
#define __HELP_BLOCK_14		"timeint2   - временной интервал срабатывания зоны 2, c;\r\n"
#define __HELP_BLOCK_15		"triglimit1 - граничное значение превышений порога за временной интервал зоны 1;\r\n"
#define __HELP_BLOCK_16		"triglimit2 - граничное значение превышений порога за временной интервал зоны 2;\r\n"

#define __WELCOME_BLOCK_0 	__POSLINE
#define __WELCOME_BLOCK_1 	"Система контроля периметра <<РУБИКОН>>\n\r"
#define __WELCOME_BLOCK_2 	"Версия программного обеспечения: "
#define __WELCOME_BLOCK_3 	VERSION
#define __WELCOME_BLOCK_4 	__NEWLINE
#define __WELCOME_BLOCK_5 	"Для справки введите help и нажмите Enter\n\r"
#define __WELCOME_BLOCK_6 	"Для выхода введите exit и нажмите Enter\n\r"
#define __WELCOME_BLOCK_7 	__POSLINE




/*commands*/
#define __EXIT	'e','x','i','t'
#define __HELP	'h','e','l','p'
#define __SAVE	's','a','v','e'
#define __SHOW	's','h','o','w'
#define __SET	's','e','t'
#define __DEBUG	'd','e','b','u','g'


/*arguments*/
#define __MODE			'm','o','d','e'
#define __ADDRESS		'a','d','d','r','e','s','s'
#define __STATE			's','t','a','t','e'
#define __CONFIG		'c','o','n','f','i','g'
#define __TRESHOLD1		't','r','e','s','h','o','l','d','1'
#define __TRESHOLD2		't','r','e','s','h','o','l','d','2'
#define __TIMEINT1		't','i','m','e','i','n','t','1'
#define __TIMEINT2		't','i','m','e','i','n','t','2'
#define __TRIGLIMIT1	't','r','i','g','l','i','m','i','t','1'
#define __TRIGLIMIT2	't','r','i','g','l','i','m','i','t','2'




#endif
/****************************end of file ********************************/
