/*
 * systick.h
 *
 *  Created on: Sep 10, 2025
 *      Author: stevo
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_


#include <stdint.h>
#include "stm32f4xx.h"
#include "sbc_lcd01.h"

void systick_msec_delay(uint32_t delay);
void systick_msec_sleep(uint32_t time);
void systick_init(void);
void SysTick_Handler(void);
#endif /* SYSTICK_H_ */
