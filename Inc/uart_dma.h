#ifndef __UART_DMA_H__
#define __UART_DMA_H__
#include <stdint.h>
#include "stm32f4xx.h"

#define UART_DATA_BUFF_SIZE		128
#define UART_TRANSFER_BUFFER_SIZE 128

void uart1_rx_tx_init(void);
void dma2_init(void);
void dma2_stream2_uart_rx_config(void);
void dma2_stream7_uart_tx_config(uint32_t msg_to_snd, uint32_t msg_len);
void DMA2_Stream7_IRQHandler(void);
void DMA2_Stream2_IRQHandler(void);
void USART2_IRQHandler(void);

#endif
