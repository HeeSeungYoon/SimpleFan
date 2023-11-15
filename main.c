/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "led.h"
#include "button.h"
#include "delay.h"
#include "I2C_LCD.h"
#include "stdio.h"
#include "time.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

LED speed_led[LED_UNITS]={
    {GPIOC, GPIO_PIN_8},
    {GPIOC, GPIO_PIN_6},
    {GPIOC, GPIO_PIN_5}
};

enum {STOP, WEAK, MID, STRONG};
Button fan_speed[4];
Button rotate_btn;
uint8_t rotate_flag = 0;

uint8_t speed = STOP;

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

char date[16];
char timer[9];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if(GPIO_Pin == GPIO_PIN_1)
//  {
//    // ...
//  }
//  else if(GPIO_Pin == GPIO_PIN_15)
//  {
//    // ...
//  }
//  else if(GPIO_Pin == GPIO_PIN_14)
//  {
//    // ...
//  }
//  else if(GPIO_Pin == GPIO_PIN_13)
//  {
//    // ...
//  }
//}

void date_init(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec)
{
  sTime.Hours = ((hour/10)<<4)+(hour%10);
  sTime.Minutes = ((min/10)<<4) + (min%10);
  sTime.Seconds = ((sec/10)<<4) + (sec%10);

  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

  sDate.Year = ((year/10)<<4)+(year%10);
  sDate.Month = ((month/10)<<4)+(month%10);
  sDate.Date = ((date/10)<<4) + (date%10);

  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

void buff_clear(char* buff)
{
  for(int i=0;i<sizeof(buff);i++)
  {
    buff[i] = '\0';
  }
}

void rotate()
{
  if(rotate_flag == 1)
  {
    for(int i=50;i<250;i+=1){
      TIM4->CCR1 = i;
      HAL_Delay(25);
    }
    for(int i=250;i>=50;i-=1){
      TIM4->CCR1 = i;
      HAL_Delay(25);
    }
  }
  else
  {
    TIM4->CCR1 = 0;
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  Button_Init(&fan_speed[STOP], GPIOB, GPIO_PIN_1);
  Button_Init(&fan_speed[WEAK], GPIOB, GPIO_PIN_15);
  Button_Init(&fan_speed[MID], GPIOB, GPIO_PIN_14);
  Button_Init(&fan_speed[STRONG], GPIOB, GPIO_PIN_13);
  Button_Init(&rotate_btn, GPIOB, GPIO_PIN_2);

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  TIM3->CCR1 = 0;

  date_init(23,10,4,9,20,0);

  lcd_init();
  sprintf(date,"%02x%02x%02x %02x:%02x:%02x", sDate.Year,sDate.Month,sDate.Date,sTime.Hours,sTime.Minutes,sTime.Seconds);
  lcd_put_cur(0, 0);
  lcd_send_string(date);
  lcd_put_cur(1, 0);
  lcd_send_string("STOP  ");
  lcd_put_cur(1, 8);
  sprintf(timer, "01:00");
  lcd_send_string(timer);

  uint8_t min = sTime.Minutes+1;
  uint8_t sec = sTime.Seconds;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

    buff_clear(date);
    sprintf(date,"%02x%02x%02x %02x:%02x:%02x", sDate.Year,sDate.Month,sDate.Date,sTime.Hours,sTime.Minutes,sTime.Seconds);
    lcd_put_cur(0, 0);
    lcd_send_string(date);

    if(Button_getState(&fan_speed[STOP]) == ACT_PUSH)
    {
      led_off(speed_led);
      speed = STOP;
      lcd_put_cur(1, 0);
      lcd_send_string("STOP  ");
    }
    if(Button_getState(&fan_speed[WEAK]) == ACT_PUSH)
    {
      led_off(speed_led);
      led_on(speed_led, WEAK);
      speed = WEAK;
      lcd_put_cur(1, 0);
      lcd_send_string("WEAK  ");
    }
    if(Button_getState(&fan_speed[MID]) == ACT_PUSH)
    {
      led_off(speed_led);
      led_on(speed_led, MID);
      speed = MID;
      lcd_put_cur(1, 0);
      lcd_send_string("MIDDLE");
    }
    if(Button_getState(&fan_speed[STRONG]) == ACT_PUSH)
    {
      led_off(speed_led);
      led_on(speed_led, STRONG);
      speed = STRONG;
      lcd_put_cur(1, 0);
      lcd_send_string("STRONG");
    }
    if(Button_getState(&rotate_btn) == ACT_PUSH)
    {
      (rotate_flag++) %=2;
      rotate();
    }

    /* Timer Implementation */

//    TIM3->CCR1 = (float)TIM3->ARR * (0.333 * speed);
//
//    buff_clear(timer);
//    uint8_t diff = sec - sTime.Seconds;
//    if((min == sTime.Minutes) &&(sec==sTime.Seconds))
//    {
//      speed = STOP;
//      lcd_put_cur(1, 0);
//      lcd_send_string("STOP  ");
//      lcd_put_cur(1, 8);
//      lcd_send_string("01:00");
//    }
//    else
//    {
//      if(diff == 0)
//      {
//        sprintf(timer,"01:00");
//      }
//      else
//      {
//        sprintf(timer,"00:%02d",60+diff);
//      }
//      lcd_put_cur(1, 8);
//      lcd_send_string(timer);
//    }
//  }

    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
