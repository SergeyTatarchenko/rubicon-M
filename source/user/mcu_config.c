/************************************************************************
* File Name          : mcu_config.c
* Author             : Tatarchenko S.
* Version            : v 1.0
* Description        : mcu pins and peripheral low level configuration
*************************************************************************/

#include "mcu_config.h"
#include "global.h"

/*
* low level microchip init function
*/
void sys_init()
{
	pin_config();
	
	led_zone_0_alrm_off;
	led_zone_0_err_off;
	led_zone_1_alrm_off;
	led_zone_1_err_off;
	sync_led_off;

	peripheral_config();
	ADC_START;
}

/*
* microchip peripheral cinfiguration function
*/
void peripheral_config()
{
	/*DMA2 clock*/
	RCC->AHB1ENR|= RCC_AHB1ENR_DMA2EN;
	/* 
	stream 0 - channel 0 ADC, high priority,
	16 bit data transfer, peripheral to memory mode,
	memory increment mode,circular mode;
	*/
	DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL;
	DMA2_Stream0->CR |= DMA_SxCR_PL_1;
	DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0|DMA_SxCR_PSIZE_0;
	DMA2_Stream0->CR &= ~DMA_SxCR_DIR;
	DMA2_Stream0->CR |= DMA_SxCR_MINC|DMA_SxCR_CIRC;
	/*setup number of conversions*/
	DMA2_Stream0->NDTR = num_of_adc_conversion;
	/*peripheral address*/
	DMA2_Stream0->PAR = (uint32_t) (&(ADC1->DR));
	/*memory address*/
	DMA2_Stream0->M0AR = (uint32_t)&ADC_VALUES;
	/*enable stream*/
	DMA2_Stream0->CR |= DMA_SxCR_EN;
	/***********************************************************************/
	/*ADC channel configuration*/
	/*ch 1,2,4,5,6,7*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	/*scan mode*/
	ADC1->CR1 |= ADC_CR1_SCAN;
	/*cont mode,conversion started from SWSTART bit*/
	ADC1->CR2 |= ADC_CR2_CONT;
	/*DMA mode enable*/
	ADC1->CR2 |= (ADC_CR2_DMA|ADC_CR2_DDS);
	
	/*ADC sample time  13.5 cycles */
	ADC1->SMPR2 |= (ADC_SMPR2_SMP0_1|ADC_SMPR2_SMP1_1|ADC_SMPR2_SMP2_1|ADC_SMPR2_SMP3_1|
					ADC_SMPR2_SMP4_1|ADC_SMPR2_SMP5_1|ADC_SMPR2_SMP6_1|ADC_SMPR2_SMP7_1);
	/*lenght of channel sequence*/
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR1 |= (num_of_adc_conversion - 1)<<20;
	/*channel poll sequence*/
	ADC1->SQR3 |= ((1<<0)|(2<<5)|(4<<10)|(5<<15)|(6<<20)|(7<<25));
	/*ADC1 on*/
	ADC1->CR2 |= ADC_CR2_ADON;
	/***********************************************************************/
	/*USART3 configuration (serial in/out)*/
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	/*1)enable USART*/
	USART3->CR1 |= (USART_CR1_UE|USART_CR1_TE|USART_CR1_RE|USART_CR1_RXNEIE);
	/*2)Program the M bit 1 start bit, 8 data bits*/
	USART3->CR1 &= ~USART_CR1_M;
	/*3)Program the number of stop bits (2 stop bits)*/
	USART3->CR2 &= ~USART_CR2_STOP;
	USART3->CR1 |=USART_CR2_STOP_1;
	/*19200 baudrate, APB1 clock is 30 MHz*/
	USART3->BRR = 0x61A;
	/***********************************************************************/
	/*USART6 configuration (RS-485)*/
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	/*1)enable USART*/
	USART6->CR1 |= (USART_CR1_UE|USART_CR1_TE|USART_CR1_RE|USART_CR1_RXNEIE);
	/*2)Program the M bit 1 start bit, 8 data bits*/
	USART6->CR1 &= ~USART_CR1_M;
	/*3)Program the number of stop bits (2 stop bits)*/
	USART6->CR2 &= ~USART_CR2_STOP;
	USART6->CR1 |=USART_CR2_STOP_1;
	/*19200 baudrate, APB1 clock is 30 MHz*/
	USART6->BRR = 0x61A;
}


/* name: serial_send_byte
*  descriprion: template for serial port send byte for io implementation
*/
void serial_send_byte(USART_TypeDef* port,const char byte)
{
	while(!(port->SR & USART_SR_TXE)); // if flag set "Transmit data register empty"
	port->DR = byte;
}

/* name: flash_data_write
*  descriprion: write data to stm32 flash memory
*/
void flash_data_write(const uint32_t address,int sector, const uint32_t *data,int size)
{
	uint32_t *memory_pointer = (uint32_t*)address;
	while(FLASH->SR&FLASH_SR_BSY);
	
	/*flash unlock*/
	FLASH->KEYR = FLASH_UNLOCK_WORD_1;
	FLASH->KEYR = FLASH_UNLOCK_WORD_2;
	/*32 bit parallelism*/
	FLASH->CR  |= FLASH_CR_PSIZE_1;
	FLASH->CR  |= FLASH_CR_SER;
	FLASH->CR  &= ~((uint32_t)0x78);
	FLASH->CR |= (sector<<3);
	FLASH->CR |= FLASH_CR_STRT;
	while(FLASH->SR&FLASH_SR_BSY);
	FLASH->CR  &= ~FLASH_CR_SER;
	FLASH->CR  |= FLASH_CR_PG;
	for(int i = 0;i < size/4; i++)
	{
		*(memory_pointer+i) = data[i];
		while(FLASH->SR&FLASH_SR_BSY);
	}
	FLASH->CR  &= ~FLASH_CR_PG;
	/*flash lock*/
	FLASH->CR  |= FLASH_CR_LOCK;
}

/* name: flash_data_read
*  descriprion: copy data from flash to RAM 
*/
void flash_data_read(const uint32_t address, uint32_t *data,int size)
{
	uint32_t *memory_pointer = (uint32_t*)address;
	
	for(int i = 0;i < size/4; i++)
	{
		data[i] = *(memory_pointer+i);
	}
}

/*
* microchip pin configuration fuction
*/
void pin_config()
{
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOCEN;

/*
 ADDRESS pins, PC0,PC1,PC2,PC3, PA0 
 configure as input floating/pullup, low speed
*/
    GPIOC->MODER &=   ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|
                        GPIO_MODER_MODER2|GPIO_MODER_MODER3);
    GPIOC->PUPDR |=    (GPIO_PUPDR_PUPDR0_0|GPIO_PUPDR_PUPDR1_0|
                        GPIO_PUPDR_PUPDR2_0|GPIO_PUPDR_PUPDR3_0);
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0|GPIO_OSPEEDER_OSPEEDR1|
                        GPIO_OSPEEDER_OSPEEDR2|GPIO_OSPEEDER_OSPEEDR3);
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0;
/*
 MODE pins PB0, PB1, PC10, PC11
 configure as input floating/pullup, low speed
*/
    GPIOB->MODER &= ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1);
    GPIOB->PUPDR |=  (GPIO_PUPDR_PUPDR0_0|GPIO_PUPDR_PUPDR1_0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0|GPIO_OSPEEDER_OSPEEDR1);
    GPIOC->MODER &= ~(GPIO_MODER_MODER10|GPIO_MODER_MODER11);
    GPIOC->PUPDR |=  (GPIO_PUPDR_PUPDR10_0|GPIO_PUPDR_PUPDR11_0);
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11);

/*
 TAMPER input PA15
 configure as input floating/pullup, low speed
*/
    GPIOA->MODER &= ~GPIO_MODER_MODER15;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR15_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15;

/*
 ADC pins PA1, PA2, PA4, PA5, PA6, PA7
 configure in as analog mode, high speed
*/
    GPIOA->MODER |= (GPIO_MODER_MODER1|GPIO_MODER_MODER2|GPIO_MODER_MODER4|
                     GPIO_MODER_MODER5|GPIO_MODER_MODER6|GPIO_MODER_MODER7);
    GPIOA->OSPEEDR |=(GPIO_OSPEEDER_OSPEEDR1|GPIO_OSPEEDER_OSPEEDR2|GPIO_OSPEEDER_OSPEEDR4|
                      GPIO_OSPEEDER_OSPEEDR5|GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7); 
/*
 pins for LED control PA8, PA9, PA10, PA11, PC9
 configure as output PP, low speed
*/
    GPIOA->MODER |= (GPIO_MODER_MODER8_0|GPIO_MODER_MODER9_0|
                     GPIO_MODER_MODER10_0|GPIO_MODER_MODER11_0);
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR9|
                       GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11);
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
/*
pins for RELAY control PB4, PB5, PB6, PB7
configure as as output PP, low speed
*/
    GPIOB->MODER |= (GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0|
                     GPIO_MODER_MODER6_0|GPIO_MODER_MODER7_0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR4|GPIO_OSPEEDER_OSPEEDR5|
                        GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7);
/*
pins for RS485 (USART6) PC6, PC7, PC8
configure as alternative function (AF8)
*/
    GPIOC->MODER   |= (GPIO_MODER_MODER6_1|GPIO_MODER_MODER7_1|GPIO_MODER_MODER8_0);
    GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7|GPIO_OSPEEDER_OSPEEDR8);
    GPIOC->AFR[0]  |= (((uint32_t)0x08000000)|((uint32_t)0x80000000));
    
/*
pins for service serial port (USART3) PB10, PB11, PB12
configure as alternative function (AF7)
*/
    GPIOB->MODER   |= (GPIO_MODER_MODER10_1|GPIO_MODER_MODER11_1|GPIO_MODER_MODER12_0);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11|GPIO_OSPEEDER_OSPEEDR12);
    GPIOB->AFR[1]  |= (((uint32_t)0x08000700)|((uint32_t)0x80007000));
}


