/******************************************************************************/
/** @file bsp_io.c
 *  @version 1.0
 *  @date 2018.12.8
 *
 *  @brief some driver functions for io peripherals
 *
 *  @author Echo
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_io.h"
//#include "tim.h"

/* Functions ------------------------------------------------------------------*/

void turn_on_laser(void)
{
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}

void turn_off_laser(void)
{
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

//void MY_PWM_ENABLE(void)
//{
//  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1); // beep
//}

void MY_VCCOUT_ENABLE(void)
{
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET);
}

