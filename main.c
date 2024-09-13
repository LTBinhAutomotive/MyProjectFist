#include"stm32f4xx.h"


void config_Clock(void);
void config_GPIO(void);
void interrupt_set(void);
void delayyy(void);


void EXTI0_IRQHandler(void);	
	

void delayyy (void)
{
	volatile unsigned int i;
	for(i = 0 ; i < 1000000 ; i++){}
}

void config_Clock (void)
{
	
	/*1.enable HSE and wait HSE ready*/
	RCC->CR &= ~(1u << 16);													//clear bit 16
	RCC->CR |=  (1u << 16);   											//enable HSE ON
	while ( !((RCC->CR) & (1u << 17)) );  					// wait HSE ready
	
	/*2.set POWER enable clock and volture regulator*/
	RCC->APB1ENR &= ~(1u << 28);										//clear bit 28
	RCC->APB1ENR |=  (1u << 28);										//set enable POWER
	PWR->CR      &= ~(3u << 14);										//clear bit 14 15 VOS
	PWR->CR      |=  (2u << 14);										//set reset value <= 84MHz
	
	/*3.configure FLASH PREFETCH and the LATENCY setting*/
	FLASH->ACR &= ~( (15u << 0)|(7u << 8) );  			//clear bit 0-3 and 8-10
	FLASH->ACR |=  ( (15u << 0)|(7u << 8) );		  	//set 5ws
	
	/*4.configure HCLK, PCLK1, PCLK2*/
	RCC->CFGR &= ~(15u << 4);												//clear bit 4-7 AHB and set AHB divide 1
	RCC->CFGR &= ~(0x3Fu << 10);										//clear bit 10-15 APB
	RCC->CFGR |=  ( (5u << 10)|(4u << 13) );				//set APB1 div 4, APB2 div 2
	
	/*5.configrate MAIN PLL*/
	RCC->PLLCFGR &= ~( (0xFFu << 0)|(0x1FFu << 6)|(3u << 16)|(1u << 22) );	//clear bit PLLM(0-5), PLLN(6-14), PLLP(16-17),PLLSCR(22)
	RCC->PLLCFGR |=  ( (4u << 0)|(10u << 6)|(0u << 16)|(1u << 22) );	      //div 4 and mult 10 and div 2
	
	/*6.enable PLL and wait ready*/
	RCC->CR &= ~(1u << 24);													//clear bit 24 25 
	RCC->CR |=  (1u << 24);													//enable PLL
	while( !((RCC->CR) & (1u << 25)) );							//wait PLL ready

	/*7.select clock soure and wait it set*/
	RCC->CFGR &= ~(3u << 0);												//clear bit 0-1
	RCC->CFGR |=  (2u << 0);												//select PLL
	while ( (RCC->CFGR & (2u << 2)) == 0 );
}

void config_GPIO(void)
{
	//1.enable clock PORTA ,D
	RCC->AHB1ENR &= ~( (1u << 3)|(1u << 0) );				//clear bit 0 3
	RCC->AHB1ENR |=  ( (1u << 3)|(1u << 0) );				//enable PORTA PORTD
	//2.MORDER
	 //2.1 button
	GPIOA->MODER &= ~(3u << 0);											//clear bit 0 set input
	 //2.2 LED
	GPIOD->MODER &= ~(0xFFu << 24);									//clear bit 24
	GPIOD->MODER |=  (85u << 24);									  //set bit 24
	//3.OTYPER
	GPIOD->OTYPER &= ~(15u << 12);									//clear bit 12 push pull							
	//4.PUPDR
	GPIOA->PUPDR  &= ~(1u << 0);										//clear bit 0
}

void interrupt_set (void)
{
	volatile unsigned int * NVIC_ISER0        = (unsigned int *)0xE000E100;
	volatile unsigned int * SYSCFG_EXTI_CR1   = (unsigned int *)0x40013808;

	/* enable CYSCFG bit 14 in register RCC*/
	RCC->APB2ENR |=  (1u << 14);
	/* config EXTI in register SYSCFG*/
	* SYSCFG_EXTI_CR1 &=  ~(15u << 0);											//PA[x] PIN		
	/* Config falling or rising register EXTI*/
	EXTI->FTSR   &= ~(1u << 0);											//clear bit 0
	EXTI->FTSR   |=  (1u << 0);											//set bit 0 falling
	/* disable IMR in register EXTI*/
	EXTI->IMR    &= ~(1u << 0);											//clear bit 0
	EXTI->IMR    |=  (1u << 0);											//set sofware
	/* enable NVIC*/
	*NVIC_ISER0   |=  (1u << 6);											//set EXTI0 of NVIC_ISER0
}

//static volatile uint32_t zcount = 0;

int main (void)
{
	config_Clock ();
	config_GPIO ();
	interrupt_set ();
	
	while(1)
	{
		
	}
}

void EXTI0_IRQHandler(void)
{
	if( EXTI->PR & (1u << 0)) 
	{
		
		EXTI->PR &= ~(1u << 0);
		
		while(1)
		{
			GPIOD->ODR  &= ~(1u << 15);
			GPIOD->ODR  |=  (1u << 12);
			delayyy();
			GPIOD->ODR  &= ~(1u << 12);
			GPIOD->ODR  |=  (1u << 13);
			delayyy();
			GPIOD->ODR  &= ~(1u << 13);
			GPIOD->ODR  |=  (1u << 14);
			delayyy();
			GPIOD->ODR  &= ~(1u << 14);
			GPIOD->ODR  |=  (1u << 15);
			delayyy();
		}
	}
}

