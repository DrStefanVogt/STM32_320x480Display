/*
 * spi_dma.h
 *
 *  Created on: Jan 6, 2026
 *      Author: stevo
 */

#ifndef SPI_DMA_H_
#define SPI_DMA_H_
#include <stdint.h>
#include "stm32f4xx.h"
#include "sbc_lcd01.h"

#define NUM_OF_CHANNELS		1

void spi_dma_init(uint16_t *buffer);
void spi1_transmit_DMA(uint32_t size);
void spi_dma_setSingleColorStatus(bool on);
#endif /* SPI_DMA_H_ */
