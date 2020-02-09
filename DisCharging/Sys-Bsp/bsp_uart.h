#ifndef __bsp_uart__
#define __bsp_uart__
#include "stm32f4xx_hal.h"

#define DEBUG_MAX_LEN					50
#define DEBUG_BUF_LEN					1
#define DBUS_BUF_LEN 					18
#define DBUS_MAX_LEN					50
#define NUC_REC_LEN					  23

extern uint8_t uart2_rx_buff[50],\
							 uart3_rx_buff[50],\
							 uart3_tx_buff[50],\
							 uart6_rx_buff[50];

void 				MY_UART_INIT(void);
void 				MY_UART_ENABLE_DMA_IDLE(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma, uint8_t *buff, uint8_t bufflen);

void 				UART_RX_IDLE_IRQ(UART_HandleTypeDef *huart);

void 				send_to_DM90(int16_t power);
void 				get_data_fromDM90(void);

#endif
