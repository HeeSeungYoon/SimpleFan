/*
 * led.h
 *
 *  Created on: Sep 22, 2023
 *      Author: user
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32f4xx_hal.h"

#define LED_UNITS       3

typedef struct
{
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;

}LED;

void led_off(LED *led);
void led_on(LED *led, uint8_t cnt);
void led_left_shift(LED *led);
void led_right_shift(LED *led);

#endif /* INC_LED_H_ */
