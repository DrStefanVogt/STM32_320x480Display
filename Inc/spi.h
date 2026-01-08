

#ifndef SPI_H_
#define SPI_H_

#define SPI1EN			(1U<<12)
#define GPIOAEN			(1U<<0)

#define SR_TXE			(1U<<1)
#define SR_RXNE			(1U<<0)

#define SR_BSY			(1U<<7)


#include "stm32f4xx.h"
#include "systick.h"
#include <stdint.h>

void spi_gpio_init(void);
void spi1_config(void);
void spi1_DMA_enable(void);
void spi1_transmit(uint8_t *data,uint32_t size);
void spi1_transmit_16(uint16_t *data,uint32_t size);
void spi1_receive(uint8_t *data,uint32_t size);
void cs_enable(void);
void cs_disable(void);

#endif /* SPI_H_ */
