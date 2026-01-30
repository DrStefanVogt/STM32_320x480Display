/*
 * gpio.h
 *
 *  Created on: Sep 12, 2025
 *      Author: stevo
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include "stm32f4xx.h"


void led_init(void);
void led_on(void);
void led_off(void);
void button_init(void);
bool get_btn_state(void);
void A0_init(void);
void A0_on(void);
void A0_off(void);
void gpio_test(void);

#endif /* GPIO_H_ */
