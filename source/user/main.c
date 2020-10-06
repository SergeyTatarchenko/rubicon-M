/************************************************************************
* File Name          : main.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : main progran file
*************************************************************************/
#include "user_tasks.h"
/*----------------------------------------------------------------------*/

void SysClkUpd( void );
BaseType_t Init_( void );

static MEM_ALLOCATION_TypeDef MEM_ALLOCATION;

/*
* name: main
* description : main
*/
int main()
{
	
	BaseType_t _AllOk_;
	SysClkUpd();
	sys_init();
	
	_AllOk_ = Init_();
	if(_AllOk_ == pdTRUE)
	{
		/*enable global interrupt*/
		__enable_irq ();
		
		NVIC_EnableIRQ (USART6_IRQn);
		
		/*RTOS start here*/
		vTaskStartScheduler();
	}
	else
	{
		/* add fault handler*/
	}
	/*unrecheable*/
	while(1);
}

/*
* name: Init_
* description : tasks creating and sharing memory, return pdTRUE if success
*/
BaseType_t Init_()
{
	BaseType_t TaskCreation;
	MEM_ALLOCATION.system_priority = configMAX_PRIORITIES - 1;
	MEM_ALLOCATION.serial_priority = configMAX_PRIORITIES - 2;
	MEM_ALLOCATION.user_priority =   configMAX_PRIORITIES - 3;
	
	MEM_ALLOCATION.stack_system = 128;
	MEM_ALLOCATION.stack_user   = 512;
	MEM_ALLOCATION.stack_serial = 256;
	
	/*queue for service serial port*/
	service_serial_queue = xQueueCreate(SERVICE_COMMBUFF_SIZE,COMMAND_BUF_SIZE);
	/*queue for 777 protocol input commands*/
	kso_serial_queue = xQueueCreate(SERVICE_COMMBUFF_SIZE,sizeof(RUBICON_CONTROL_MESSAGE_TypeDef));
	/*usart save murex*/
	xMutex_serial_BUSY = xSemaphoreCreateMutex();
	/**/
	xSemph_state_UPDATE = xSemaphoreCreateCounting( 5, 0 );
	
	TaskCreation = xTaskCreate(&_task_led ,"led",configMINIMAL_STACK_SIZE, 
									NULL, MEM_ALLOCATION.user_priority , NULL );
	TaskCreation &= xTaskCreate(&_task_state_update ,"main routine",MEM_ALLOCATION.stack_user,
									NULL, MEM_ALLOCATION.user_priority , NULL );
	TaskCreation &= xTaskCreate(&_task_service_serial ,"serial",MEM_ALLOCATION.stack_serial,
									NULL, MEM_ALLOCATION.serial_priority , NULL );
	TaskCreation &= xTaskCreate(&_task_rubicon_thread ,"777",MEM_ALLOCATION.stack_serial,
									NULL, MEM_ALLOCATION.serial_priority , NULL );
	TaskCreation &= xTaskCreate(&_task_system_thread ,"system",MEM_ALLOCATION.stack_system,
									NULL, MEM_ALLOCATION.system_priority , NULL );
	return TaskCreation;
}

/*
* name: SysClkUpd
* description : clock update , 12 MHz HSE, PLL as SYSCLK, 120 MHz core clock
*/
void SysClkUpd( void )
{
    uint32_t StartUpCounter = 0, HSEStatus = 0;
    uint32_t M = 6,N = 120,P = 2,Q = 5;

    RCC->CR |= (uint32_t)0x00000001;
    RCC->CFGR = 0x00000000;
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    RCC->PLLCFGR = 0x24003010;
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    RCC->CIR = 0x00000000;
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS;

    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }
    if (HSEStatus == (uint32_t)0x01)
    {
    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
    /* Configure the main PLL */

    /*PLL configuration*/
    RCC->PLLCFGR = (M | (N << 6) | (((P >> 1) -1) << 16) | (Q << 24));
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; 
    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

     /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
    SCB->VTOR = FLASH_BASE;
}
/****************************end of file ********************************/
