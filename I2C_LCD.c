/*
 * I2C_LCD.c
 *
 *  Created on: Sep 14, 2023
 *      Author: user
 */

#include "I2C_LCD.h"

#define SLAVE_ADDRESS_LCD       0x27<<1

void lcd_send_cmd(char cmd)
{
  char data_u, data_l;
  uint8_t data_t[4];
  data_u = (cmd & 0xf0);
  data_l = ((cmd<<4) & 0xf0);
  data_t[0] = data_u | 0x0C;    // en=1, reset=0,
  data_t[1] = data_u | 0x08;
  data_t[2] = data_l | 0x0C;
  data_t[3] = data_l | 0x08;
  HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, data_t, sizeof(data_t), 100);

}

void lcd_clear(void)
{
  lcd_send_cmd(0x08);
  for(int i=0;i<32;i++)
  {
    lcd_send_data(' ');
  }
}

void lcd_put_cur(int row, int col)
{
  switch(row)
  {
    case 0:
      col |= 0x80;
      break;
    case 1:
      col |= 0xC0;
      break;
  }
  lcd_send_cmd(col);
}

void lcd_send_data(char data)
{
  char data_u, data_l;
  uint8_t data_t[4];
  data_u = (data & 0xf0);
  data_l = ((data <<4) & 0xf0);
  data_t[0] = data_u | 0x0D;
  data_t[1] = data_u | 0x09;
  data_t[2] = data_l | 0x0D;
  data_t[3] = data_l | 0x09;
  HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, data_t, sizeof(data_t), 100);
}

void lcd_send_string(char *str)
{
  while(*str) lcd_send_data(*str++);
}


// 4bit
void lcd_init(void)
{
  HAL_Delay(40);
  lcd_send_cmd(0x30);
  HAL_Delay(5);
  lcd_send_cmd(0x30);
  HAL_Delay(1);
  lcd_send_cmd(0x30);

  HAL_Delay(10);
  lcd_send_cmd(0x20); // set 4bits
  HAL_Delay(10);

  lcd_send_cmd(0x28); // number of display lines and character font : 2row, 5*8 font
  HAL_Delay(1);
  lcd_send_cmd(0x08); // display off
  HAL_Delay(1);
  lcd_send_cmd(0x01); // display clear
  HAL_Delay(1);
  lcd_send_cmd(0x0C); // display on
  HAL_Delay(1);
  lcd_send_cmd(0x06); // entry mode set
}

