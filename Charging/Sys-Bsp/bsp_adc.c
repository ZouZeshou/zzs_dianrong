#include "bsp_adc.h"
#include "stdio.h"
#include "cap_ctrl.h"
#include "bsp_can.h"
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		/* 处理数据 */
		/****************************************/
		static uint16_t count = 0;
		static uint16_t count1 = 0;
		static float ChargingCur_Sum = 0.0f;
		static float ChassisCur_Sum = 0.0f;
		static float sum1 = 0.0f;
		static float sum2 = 0.0f;

		/* 充电电流反馈 */
		cap_info.ChargingCur_Now = (float)cap_info.adc1_rx_buff[0] * 0.000805664f ;//- 74.40f) / 0.55f;//76.95 = ChargingCur_Base * 30;
		/* 底盘电流反馈 */
		cap_info.ChassisCur_Now = (float)cap_info.adc1_rx_buff[1] * 0.000805664f ;//- 73.05f) / 0.55f;
		
		if(count == 30)
		{//3333Hz
			#if	Cap_Board_ID == 3
			cap_info.ChargingCur_Sum = (ChargingCur_Sum - 75.33f) / 0.04;
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 75.48f) / 0.04;
			#elif Cap_Board_ID == 4
			cap_info.ChargingCur_Sum = (ChargingCur_Sum - 74.94f) / 0.04;
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 74.7f) / 0.04;
			#elif Cap_Board_ID == 5
			cap_info.ChargingCur_Sum = (ChargingCur_Sum - 75.51) / 0.04;
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 75.48f) / 0.04;
			#elif Cap_Board_ID == 6
			cap_info.ChargingCur_Sum = (ChargingCur_Sum - 75.12) / 0.04;
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 75.03) / 0.04;
			#endif
			//6 2.504 2.501
			count = 0;
			ChargingCur_Sum = 0.0f;
			ChassisCur_Sum = 0.0f;
		}
		if(count1 == 1000)
		{//100HZ
			cap_info.ChargingCur_Base = sum1 / 1000.0f;
			cap_info.ChassisCur_Base = sum2 / 1000.0f;
			count1 = 0;
			sum1 = 0.0f;
			sum2 = 0.0f;
		}
		ChargingCur_Sum += cap_info.ChargingCur_Now;
		ChassisCur_Sum += cap_info.ChassisCur_Now;
		sum1 += cap_info.ChargingCur_Now;
		sum2 += cap_info.ChassisCur_Now;
		
		count1++;
		count ++;
		/***************************************/
		FPS.adc1_dma++;
	}
	if(hadc->Instance == ADC2)
	{//100HZ
		#if Cap_Board_ID == 3
			cap_info.Cap_Volt = cap_info.adc2_rx_buff[0] * 0.008862304 ;//3.3/4096*11  11是计算分压之前的电压，也就是计算电容的真是电压
		#elif Cap_Board_ID == 4
//			cap_info.Cap_Volt = cap_info.adc2_rx_buff[0] * 0.010602539f - 0.13f;//3.3/4096*11  11是计算分压之前的电压，也就是计算电容的真是电压
			cap_info.Cap_Volt = cap_info.adc2_rx_buff[0] * 0.008862304f;//3.3/4096*11  11是计算分压之前的电压，也就是计算电容的真是电压
		#elif Cap_Board_ID == 5
			cap_info.Cap_Volt = cap_info.adc2_rx_buff[0] * 0.008862304f;//3.3/4096*11  11是计算分压之前的电压，也就是计算电容的真是电压
		#elif Cap_Board_ID == 6
			cap_info.Cap_Volt = cap_info.adc2_rx_buff[0] * 0.008862304f;//3.3/4096*11  11是计算分压之前的电压，也就是计算电容的真是电压

		#endif
		FPS.adc2_dma++;
	}
}