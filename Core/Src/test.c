// #include "stm32f051x8.h"
// #include "stm32f0xx_ll_gpio.h"
// #include "stm32f0xx_ll_usart.h"
// #include "stm32f0xx_ll_bus.h"

// #define BUFFER_LENGTH 2u
// uint8_t buffer[BUFFER_LENGTH] = {0};

// void DMA1_Channel2_3_IRQHandler(void)
// {
// 	if (DMA1->ISR & DMA_ISR_TCIF3)
// 	{
// 		// 10.6.2 DMA interrupt flag clear register (DMA_IFCR)
// 		// Bit 9 CTCIF3: transfer complete flag clear for channel 3
// 		DMA1->IFCR |= DMA_IFCR_CTCIF3;
		
// 		// 27.8.9 USART interrupt flag clear register (USART_ICR)
// 		// Bit 6 TCCF: Transmission complete clear flag
// 		// Clear the TC flag in the USART_ISR register by setting the TCCF bit in the USART_ICR register.
// 		USART1->ICR |= USART_ICR_TCCF;

// 		buffer[0] = 0;
// 		buffer[1] = 0;

// 		// Manual implementation of circular mode START
//   		DMA1_Channel3->CCR &= ~DMA_CCR_EN;  // Disable DMA to update CNDTR value
// 		DMA1_Channel3->CNDTR = BUFFER_LENGTH;
//     		DMA1_Channel3->CCR |= DMA_CCR_EN;  // Enable DMA again for next transfer
// 		// Manual implementation of circular mode END
// 	}
// }

// int main(void)
// {
// 	// 6.4.6 AHB peripheral clock enable register (RCC_AHBENR)
// 	// Bit 18 IOPBEN: I/O port B clock enable
// 	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

// 	// 6.4.7 APB peripheral clock enable register 2 (RCC_APB2ENR)
// 	// Bit 14 USART1EN: USART1 clock enable
// 	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

// 	// 8.4.1 GPIO port mode register (GPIOx_MODER) (x =A to F)
// 	// Bits 31:0 MODER[15:0][1:0]: Port x configuration I/O pin y (y = 15 to 0)
// 	// (2<<14) - 10: Alternate function mode
// 	GPIOB->MODER |= GPIO_MODER_MODER7_1; // PB7
// 	// (2<<12) - 10: Alternate function mode
// 	GPIOB->MODER |= GPIO_MODER_MODER6_1; // PB6

// 	// 8.4.9 GPIO alternate function low register (GPIOx_AFRL) (x = A to F)
// 	// Bits 31:0 AFSELy[3:0]: Alternate function selection for port x pin y (y = 0..7)
// 	// https://www.st.com/resource/en/datasheet/stm32f303vc.pdf
// 	// Table 15. Alternate functions for port B
// 	GPIOB->AFR[0] |= (0<<24); // Set AF0 for PB7 
// 	GPIOB->AFR[0] |= (0<<28); // Set AF0 for PB6
 	
// 	// 27.8.4 USART baud rate register (USART_BRR)
// 	// Bits 15:4 BRR[15:4]
// 	// Bits 3:0 BRR[3:0]
// 	LL_USART_SetBaudRate(USART1, 8*1000000, LL_USART_OVERSAMPLING_16, 115200);

// 	// Enable NVIC
// 	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

// 	// 6.4.6 AHB peripheral clock enable register (RCC_AHBENR)
// 	// Bit 0 DMAEN: DMA clock enable
// 	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
		
// 	// USART1_TX uses DMA Channel 2 (memory -> USART1)
// 	// USART1_RX uses DMA Channel 3 (USART1 -> memory)

// 	// 10.6.4 DMA channel x number of data to transfer register (DMA_CNDTRx)
// 	// Bits 15: 0NDT[15:0]: number of data to transfer (0 to 216 - 1)
// 	DMA1_Channel3->CNDTR |= BUFFER_LENGTH;
	
// 	// 10.6.3 DMA channel x configuration register (DMA_CCRx)
// 	// Bit 7 MINC: memory increment mode
// 	DMA1_Channel3->CCR |= DMA_CCR_MINC;
	
// 	// 10.6.3 DMA channel x configuration register (DMA_CCRx)
// 	// Bit 4 DIR: data transfer direction
// 	// We have 2 options:
// 	// 	0: read from peripheral
// 	// 	1: read from memory

// 	// We want to send data from peripheral to memory, so we "read from peripheral"
// 	DMA1_Channel3->CCR &= ~DMA_CCR_DIR;
	
// 	// 10.6.3 DMA channel x configuration register (DMA_CCRx)
// 	// Bit 1 TCIE: transfer complete interrupt enable
// 	DMA1_Channel3->CCR |= DMA_CCR_TCIE;

// 	// Bit 5 CIRC: circular mode
// 	// Enable circular mode to continuously transfer data into the buffer
// 	// DMA1_Channel3->CCR |= DMA_CCR_CIRC;
	
// 	// 10.6.5 DMA channel x peripheral address register (DMA_CPARx)
// 	// Bits 31:0 PA[31:0]: peripheral address
// 	// If Rx - USART1->RDR
// 	// If Tx - USART1->TDR
// 	DMA1_Channel3->CPAR |= (uint32_t)&USART1->RDR;
	
// 	// 10.6.6 DMA channel x memory address register (DMA_CMARx)
// 	// Bits 31:0 MA[31:0]: peripheral address
// 	DMA1_Channel3->CMAR |= (uint32_t)buffer;

// 	// 27.8.1 USART control register 1 (USART_CR1)
// 	// Bit 3 TE: Transmitter enable
// 	USART1->CR1 |= USART_CR1_TE; // Tx
// 	// Bit 2 RE: Receiver enable
// 	USART1->CR1 |= USART_CR1_RE; // Rx

// 	// 27.8.3 USART control register 3 (USART_CR3)
// 	// Bit 6 DMAT: DMA enable receiver
// 	// If Rx - USART_CR3_DMAR
// 	// If Tx - USART_CR3_DMAT
// 	USART1->CR3 |= USART_CR3_DMAR;

// 	// 27.8.1 USART control register 1 (USART_CR1)
// 	// Bit 0 UE: USART enable
// 	USART1->CR1 |= USART_CR1_UE; // UART1

// 	// 10.6.3 DMA channel x configuration register (DMA_CCRx)
// 	// Bit 0 EN: channel enable
// 	DMA1_Channel3->CCR |= DMA_CCR_EN;

// 	while(1)
// 	{
// 	}
// }
