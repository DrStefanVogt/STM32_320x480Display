/*
 * exti.h
 *
 *  Created on: Jan 9, 2026
 *      Author: stevo
 */

#ifndef EXTI_H_
#define EXTI_H_


#include <stdint.h>
#include "stm32f4xx.h"

#define 	LINE13		(1U<<13)

void exti_init(void);
void DMA1_Stream5_IRQHandler(void);



#endif /* EXTI_H_ */
