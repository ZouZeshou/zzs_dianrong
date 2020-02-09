#include "bsp_tim.h"
#include "tim.h"
#include "string.h"
#include "main.h"
#include "stdio.h"
#include "cap_ctrl.h"
#include "bsp_can.h"
#include "ANO_DT.h"
/**
 * @brief tim Callback
 * @param None
 * @return None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{//300KHZ ADC1
		FPS.timer3 ++;
	}
	else if(htim->Instance == TIM6)
	{//1HZ	print
		debug_ctrl();
	} 
	else if (htim->Instance == TIM5)
	{//1000hz  pid
		FPS.timer5 ++;
		relay_ctrl();
		current_ctrl();
//		if(cap_info.ChassisCur_Sum > 150.0f)
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
//		else
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	}
} 
/**
 * @brief debug control function
 * @param None
 * @return None
 */
void debug_ctrl(void)
{
	/* LED */
		HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);
		/* printf */
		printf("/*****************************************/\r\n");
		printf("pidout=%f\r\n", discharging_cur_pid.ctrOut);
		printf("FPS.ADC1_DMA = %d  FPS.timer3= %d  FPS.timer2= %d  FPS.timer5= %d\r\n",\
						FPS.adc1_dma, FPS.timer3, FPS.timer2, FPS.timer5);
		printf("err=%f  compare=%d\r\n", \
						discharging_cur_pid.errNow,\
						__HAL_TIM_GET_COMPARE(&htim5, TIM_CHANNEL_2));
		printf("ChargingCur_Sum=%f  ChassisCur_Sum=%f Chassis_Volt=%f\r\n",\
						cap_info.DisChargingCur_Sum,\
						cap_info.ChassisCur_Sum,\
						cap_info.Chassis_Volt); 
		printf( "Cap_Volt=%f   Chassis_Power=%f  charging_base=%f\r\n",\
						cap_info.Cap_Volt ,\
						cap_info.Chassis_Power,\
						cap_info.DisChargingCur_Base);
		printf("chassis_base=%f  Power_buff=%d  USE_cAP = %d\r\n",\
						cap_info.ChassisCur_Base,\
						cap_info.ChassisPower_Buff,\
						cap_info.use_Cap);	  
		/* monitor */
		memset(&FPS, 0, sizeof(OFFLINE_COUNT_STRUCT));
}
/**
 * @brief relay control function
 * @param None
 * @return None
 */
void relay_ctrl(void)
{
	
}
/**
 * @brief current  control function
 * @param None
 * @return None
 */
void current_ctrl(void)
{
	if(cap_info.use_Cap != 0)
	{
		if(cap_info.Chassis_Volt * cap_info.ChassisCur_Sum > 2000.0f && cap_info.Cap_Volt > 14.0f)
			DisChargingControl(&cap_info);
	}
	else
		__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, (uint16_t)0);
}