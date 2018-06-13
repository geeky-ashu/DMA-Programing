/*************************************IRQ Handler**********************************************************/

#include "stm32f4xx.h"
#define is_it_HT() DMA1->HISR & (1 << 20)		//Half transfer condition is true when bit 20 of HISR reg is set
#define is_it_FT() DMA1->HISR & (1 << 21)		//Full transfer condition is true when bit 21 of HISR reg is set
#define is_it_TE() DMA1->HISR & (1 << 19)		//Transfer Eoorr condition is true when bit 19 of HISR reg is set
#define is_it_FE() DMA1->HISR & (1 << 16)		//FIFO Error condition is true when bit 16 of HISR reg is set
#define is_it_DME() DMA1->HISR & (1 << 18)		//DMA Error condition is true when bit 18 of HISR reg is set

extern void HT_Complete_Callback(void);
extern void FT_Complete_Callback(void);
extern void TE_error_Callback(void);
extern void FE_error_Callback(void);
extern void DME_error_Callback(void);


//This FunctionalState will check if the bit for ccorresponding is set in PR register an dit clears it
void clear_exti_pending_bit(void)
{

	EXTI_TypeDef *pEXTI;      
	pEXTI = EXTI;
	
	if (EXTI->PR &(1<<13))
	{
		EXTI->PR |= (1<<13);    //Clear the pending bit in PR register by setting it to 1
	}
}




//IRQ handler for button interupt
void EXTI15_10_IRQHandler(void)
{
	
	USART_TypeDef *pUART2;		//pointer to USART peripheral structure
	pUART2 = USART2;					//pointer to basse addr of USART2
	
	//Send UART2 TX DMA request
	pUART2->CR3 |= (1 << 7);
	
	
	clear_exti_pending_bit();
}


//IRQ handler for DMA1 Stream 6 Global interupt
void DMA1_Stream6_IRQHandler(void)
{
	if (is_it_HT())
	{
		DMA1->HIFCR |= (1 << 20);    //Clear the interupt 
		HT_Complete_Callback();
	}
	else if (is_it_FT())
	{
		DMA1->HIFCR |= (1 << 21);    //Clear the interupt 
		FT_Complete_Callback();
	}
	else	if (is_it_TE())
	{
		DMA1->HIFCR |= (1 << 19);    //Clear the interupt 
		TE_error_Callback();
	}
	else	if (is_it_FE())
	{
		DMA1->HIFCR |= (1 << 16);    //Clear the interupt 
		FE_error_Callback();
	}
	else	if (is_it_DME())
	{
		DMA1->HIFCR |= (1 << 18);    //Clear the interupt 
		DME_error_Callback();
	}
	
}