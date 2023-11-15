/*
 * led.c
 *
 *  Created on: Sep 22, 2023
 *      Author: user
 */

#include "led.h"

int led_idx=-1;

void led_off(LED *led)
{
  for(int i=0;i<LED_UNITS;i++)
  {
    HAL_GPIO_WritePin(led[i].GPIOx, led[i].GPIO_Pin, RESET);
  }
}

void led_on(LED *led, uint8_t cnt)
{
  for(int i=0;i<cnt;i++)
  {
    HAL_GPIO_WritePin(led[i].GPIOx, led[i].GPIO_Pin, SET);
  }
}

void led_left_shift(LED *led)
{
  if(led_idx >= LED_UNITS-1) led_idx = -1;
  led_idx++;
  HAL_GPIO_WritePin(led[led_idx].GPIOx, led[led_idx].GPIO_Pin, SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(led[led_idx].GPIOx, led[led_idx].GPIO_Pin, RESET);
}

void led_right_shift(LED *led)
{
  if(led_idx <= 0) led_idx = LED_UNITS;
  led_idx--;
  HAL_GPIO_WritePin(led[led_idx].GPIOx, led[led_idx].GPIO_Pin, SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(led[led_idx].GPIOx, led[led_idx].GPIO_Pin, RESET);
}

