/******************************************************************************/
/** @file bsp_usart.c
 *  @version 1.0
 *  @date 2018.12.8
 *
 *  @brief uart receive data from DBus/judge_system/NUC etc.
 *
 *  @author Echo
 *	@note 	i had try to use the uart idle interrupt and the DMA without 
 *					interrupt to receive the data, (use the unknown length of buff)
 *					but the performance is not weel, the fps maximum value is 60+
 *					So i think the DMA receive completely IT should be used , then
 *					the FPS maximum value is 140+.
 *	@atten  if you want to use the DMA to receive the DBUS OR DEBUG_UART data
 *					the MODE of the DMA must be set as "CIRCLE"
 */

/* Includes ------------------------------------------------------------------*/
#include "BSP_UART.h"
#include "usart.h" 
#include "stdio.h"
#include "stmgood.h"
/* Private typedef -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t uart2_rx_buff[50],\
				uart3_rx_buff[50],\
				uart3_tx_buff[50],\
				uart6_rx_buff[50];
				
uint8_t DBUSBuf[DBUS_BUF_LEN];

/* Private extern variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
/**
 * @brief Uart Init functions
 * @param None
 * @return None
 * @attention it must be called if you need to use USART
 */

void MY_UART_INIT(void)
{
	/* debug uart */
	HAL_UART_Receive_IT(&huart2,uart2_rx_buff,1);
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_ERR);	
}

void MY_UART_ENABLE_DMA_IDLE(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma, uint8_t *buff, uint8_t bufflen)
{
	HAL_DMA_Start_IT(hdma, (uint32_t)huart->Instance->DR,(uint32_t)buff, bufflen);
	huart->Instance->CR3 |= USART_CR3_DMAR;
	__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
	HAL_UART_Receive_DMA(huart, buff, bufflen);
	__HAL_UART_ENABLE_IT(huart,UART_IT_ERR);
}

/**
 * @brief DBUS Interrupt function
 * @param None
 * @return None
 * @attention Replace huart1 interrupt in stm32f4xx_it.c
 */
void UART_RX_IDLE_IRQ(UART_HandleTypeDef *huart){
	if(huart->Instance == USART2)
	{
			if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET){
			__HAL_UART_CLEAR_IDLEFLAG(&huart2);		
			HAL_UART_DMAStop(&huart2);
			HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,1);
		}
	}
}
/**
 * @brief redefinitte the call back function
 * @param huart_hangdle
 * @return None
 * @attention if the uart does not use the DMA to rec/transmit, the uart shoud use this call back function
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2){
		__HAL_UART_CLEAR_PEFLAG(&huart2);//清除错误标志位，清空SR、DR寄存器
		HAL_UART_Receive_IT(&huart2,uart2_rx_buff,1);//再次使能串口接受中断，如果不使能，则不会产生中断！！		
		Dealdata(uart2_rx_buff[0]);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	}
}
/**
 * @brief Error Callback function
 * @param None
 * @return None
 * @attention None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->ErrorCode == HAL_UART_ERROR_ORE)
	{
		__HAL_UART_CLEAR_OREFLAG(huart); //清除错误标志位，清空SR、DR寄存器
	}
}
/**
 * @brief Redirect function for printf
 * @param None
 * @return None
 * @attention  The printf function could not be usedwithout this function
 */

int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0); 
	USART2->DR = (uint8_t) ch;      
	return ch;
//    uint8_t temp[1]={ch};
//    HAL_UART_Transmit(&huart2,temp,1,2);
}
