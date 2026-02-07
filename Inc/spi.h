

#ifndef SPI_H_
#define SPI_H_

#define SPI1EN			(1U<<12)
#define SPI3EN			(1U << 15)
#define GPIOAEN			(1U<<0)
#define GPIOCEN			(1U<<2)

#define SR_TXE			(1U<<1)
#define SR_RXNE			(1U<<0)

#define SR_BSY			(1U<<7)


#include "stm32f4xx.h"
#include "systick.h"
#include <stdint.h>

void spi_gpio_init(void);
void spi3_config(void);
void spi3_DMA_enable(void);
void spi3_transmit(uint8_t *data,uint32_t size);
void spi3_transmit16(uint16_t *data,uint32_t size);
void cs_enable(void);
void cs_disable(void);
void spi3_set8(void);
void spi3_set16(void);
char castNumToChar(uint8_t input);
#endif /* SPI_H_ */
