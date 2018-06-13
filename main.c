#include <stdint.h>
#include "stm32f4xx.h"

void button_init(void);
void uart2_init(void);
void dma1_init(void);
void enable_dma1_stream6(void);
void dma1_interupt_configuration(void);
void HT_Complete_Callback(void);
void FT_Complete_Callback(void);
void TE_error_Callback(void);
void FE_error_Callback(void);
void DME_error_Callback(void);

	

#define BASE_ADDR_GPIOC GPIOC
#define BASE_ADDR_GPIOA GPIOA
char data_stream[] = "Hello World\r\n";


int main(void)
{
	button_init();
	uart2_init();
	dma1_init();
	dma1_interupt_configuration();
	enable_dma1_stream6();
	return 0;
}

void enable_dma1_stream6(void)
{
	DMA_Stream_TypeDef *pSTREAM6;					//pointer to the Stream structure
	pSTREAM6 = DMA1_Stream6;							//pointer to the base addr of the Stream6
	pSTREAM6->CR	|= (0x1 << 0);							//enable stream 6
}

void dma1_interupt_configuration(void)
{
	//configure all the interupt related to DMA
	DMA_Stream_TypeDef *pSTREAM6;					//pointer to the Stream structure
	pSTREAM6 = DMA1_Stream6;							//pointer to the base addr of the Stream6
	
	pSTREAM6->CR |= (1 << 3);							// Half transfer interupt
	pSTREAM6->CR |= (1 << 4);							//Transfer complete interupt
	pSTREAM6->CR |= (1 << 2);							// transfer Error interupt
	pSTREAM6->CR |= (1 << 1);							//Direct mode error interupt
	pSTREAM6->FCR |= (1 << 7);						//FIFO overrun/underrun interupt
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);		//enable the IRQ for DMA1 Stream 6 in NVIC register
}
void button_init(void)
{
	GPIO_TypeDef *pGPIOC;     //A pointer to yhe structure of the GPIO peripheral registers
	pGPIOC = BASE_ADDR_GPIOC; //Initialize the pointer to the base addr of the macro
	
	RCC_TypeDef *pRCC;				//Pointer to the RCC peripheral structure 
	pRCC = RCC;								//Pointer to base addr of RCC peripheral
	
	EXTI_TypeDef *pEXTI;      //pointer to EXTI structure
	pEXTI = EXTI;							//pointer to base addr of EXTI
	
	SYSCFG_TypeDef *pSYSCFG;	//pointer to sys configue struct
	pSYSCFG = SYSCFG;					//pointer to base addr

	pRCC->AHB1ENR |= (1<<2);  			//Enabling the clocks for PC
	pGPIOC->MODER &= ~(0x3<<26); 		//Making the GPIO pin as input 
	pEXTI->IMR = (1<<13);         	//Enable the interupt on corresponding pin
	pRCC->APB2ENR |= (1<<14);				//Enable the clock for SYSCFG block
	pSYSCFG->EXTICR[3] &= ~(0xF<<4);//Clering bits 20 to 23 of SYSCFG CR4 register
	pSYSCFG->EXTICR[3] |= (0x2<<4);	//Setting bits 20 to 23 of SYSCFG CR4 register
	pEXTI->FTSR = (1<<13);					//Configure Edge detection on GPIO pin
	NVIC_EnableIRQ(EXTI15_10_IRQn); //Enable the IRQ for PC13 in NVIC register
}


void uart2_init(void)
{
	//Uart 2 is used specificall because it is used as a virtual com port in the Nucleo Board
	
	GPIO_TypeDef *pGPIOA;     //A pointer to yhe structure of the GPIO peripheral registers
	pGPIOA = BASE_ADDR_GPIOA; //Initialize the pointer to the base addr of the macro
	
	RCC_TypeDef *pRCC;				//Pointer to the RCC peripheral structure 
	pRCC = RCC;								//Pointer to base addr of RCC peripheral
	
	USART_TypeDef *pUART2;		//pointer to USART peripheral structure
	pUART2 = USART2;					//pointer to basse addr of USART2
	
	pRCC->APB1ENR |= (1<<17);							//Enable UART 2 peripheral clock
	pRCC->AHB1ENR |= (1<<0);  						//Enable clock for port A
	pGPIOA->MODER &= ~(0x2 << 4); 				//Clearing before selecting the mode
	pGPIOA->MODER |= (0x2 << 4);					//Eneable alternate function mode for PA2 
	pGPIOA->AFR[0] &= ~(0xF << 8);				//Clear reg before configuring
	pGPIOA->AFR[0] |= (0x7 << 8);					//Configure PA2 as TX pin
	pGPIOA->PUPDR &= ~(0xF <<4);					//clear before confguring
	pGPIOA->PUPDR |= (0x1 <<4);						//Enble the pull up for uart comm PA-2
	pGPIOA->MODER &= ~(0x3 << 6); 				//Clearing before selecting the mode
	pGPIOA->MODER |= (0x2 << 6);					//Eneable alternate function mode for PA3
	pGPIOA->AFR[0] &= ~(0xF << 12);				//Clear reg before configuring
	pGPIOA->AFR[0] |= (0x7 << 12);				//Configure PA2 as RX pin
	pGPIOA->PUPDR &= ~(0xF << 6);					//clear before confguring
	pGPIOA->PUPDR |= (0x1 << 6);					//Enble the pull up for uart comm PA-3
	pUART2->BRR = 0x8B;   								//Configure the baud rate to be 113200
	pUART2->CR1 |= (1<<3);    						//Enable the Tx engine
	pUART2->CR1 |= (1<<13);								//ENABLE the uart2_init perpheral
}
void dma1_init(void)
{
	uint32_t len = sizeof(data_stream);  	//detremines the length of data
	RCC_TypeDef *pRCC;										//Pointer to the RCC peripheral structure 
	pRCC = RCC;														//Pointer to base addr of RCC peripheral
	
	DMA_TypeDef *pDMA;										//pointer to DMA structure
	pDMA = DMA1;													//pointer to the base addr of DMA
	
	DMA_Stream_TypeDef *pSTREAM6;					//pointer to the Stream structure
	pSTREAM6 = DMA1_Stream6;							//pointer to the base addr of the Stream6
	
	USART_TypeDef *pUART2;		//pointer to USART peripheral structure
	pUART2 = USART2;					//pointer to basse addr of USART2
	
	pRCC->AHB1ENR |= (1 << 21);								//enable the clock for DMA 1
	pSTREAM6->M0AR = (uint32_t)data_stream;		//Configure the source address
	pSTREAM6->PAR =  (uint32_t)&pUART2->DR;		//Configure the destination address
	pSTREAM6->CR &= ~(0x7 << 25);    					//clear brfore configuring the channel
	pSTREAM6->CR |= (0x4 << 25);							//Configure the channel to be used = 4
	pSTREAM6->NDTR = len;											//configure the number of data items to be sent
	pSTREAM6 ->CR &= ~(0x1 << 6);							//CLEAR before configuring
	pSTREAM6 ->CR |= (0x1 << 6);							//Direction set to memory to peripheral
	pSTREAM6->CR  &= ~(0x3<<13);							//program memory(source) data size
	pSTREAM6->CR  &= ~(0x3<<11);							//program peripheral(destination) data size
	pSTREAM6->CR |= (1 << 10);								//Set memory auto increment
	pSTREAM6->FCR |= (1<<2);									//Enable the FIFO mode
	pSTREAM6->FCR &= ~(0x3 << 0);							//CLEAR before configuring the threshold
	pSTREAM6->FCR |= (0x3 << 0);							//Set the FIFO threshold to FUL
}

void HT_Complete_Callback(void)
{
	
}

void FT_Complete_Callback(void)
{
	uint32_t len = sizeof(data_stream);  	//detremines the length of data
	USART_TypeDef *pUART2;		//pointer to USART peripheral structure
	pUART2 = USART2;					//pointer to basse addr of USART2
	
	DMA_Stream_TypeDef *pSTREAM6;					//pointer to the Stream structure
	pSTREAM6 = DMA1_Stream6;							//pointer to the base addr of the Stream6
	
	//again load the ndtr bcoz one executed it gets disabled
	pSTREAM6->NDTR = len;											//configure the number of data items to be sent
	
	pUART2->CR3 &= ~(1 << 7);
	
	enable_dma1_stream6();

}

void TE_error_Callback(void)
{
	while(1);
}

void FE_error_Callback(void)
{
	while(1);
}

void DME_error_Callback(void)
{
	while(1);
}
	