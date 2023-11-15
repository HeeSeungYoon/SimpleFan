/*
 * button.c
 *
 *  Created on: Sep 6, 2023
 *      Author: user
 */

#include "button.h"

void Button_Init(Button *btn, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  btn->GPIOx = GPIOx;
  btn->GPIO_Pin = GPIO_Pin;
  btn->preState = RELEASED;
}

uint8_t Button_getState(Button *btn)
{
  uint8_t curState = HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_Pin);
  if(curState == PUSHED && btn->preState == RELEASED)
  {
    btn->preState = PUSHED;
    return ACT_PUSH;
  }
  else if((curState != PUSHED) && (btn->preState == PUSHED))
  {
    btn->preState = RELEASED;
    return ACT_RELEASE;
  }
  return NO_ACT;
}
