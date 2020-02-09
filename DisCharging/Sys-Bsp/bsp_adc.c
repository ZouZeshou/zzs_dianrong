#include "bsp_adc.h"
#include "stdio.h"
#include "cap_ctrl.h"
#include "bsp_can.h"
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance == ADC1){
		/* 处理数据 */
		/****************************************/
		static uint16_t count = 0;
		static uint16_t count1 = 0;
		static float DisChargingCur_Sum = 0.0f;
		static float ChassisCur_Sum = 0.0f;
		static float sum1 = 0.0f;
		static float sum2 = 0.0f;
		
		/* 放电电流反馈 */
		cap_info.DisChargingCur_Now = (float)cap_info.adc1_rx_buff[1] * 0.000805664f ;//- 74.40f) / 0.55f;//76.95 = DisChargingCur_Base * 30;
		/* 底盘电流反馈 */
		cap_info.ChassisCur_Now = (float)cap_info.adc1_rx_buff[0] * 0.000805664f ;//- 73.05f) / 0.55f;
		
		if(count == 30){
			#if Cap_Board_ID == 3
			cap_info.DisChargingCur_Sum = (DisChargingCur_Sum - 75.72f) / 0.04;//0.155
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 75.54f) / 0.04;
			#elif Cap_Board_ID == 4
			cap_info.DisChargingCur_Sum = (DisChargingCur_Sum - 75.51f) / 0.04;//0.155
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 74.79f) / 0.04;
			#elif Cap_Board_ID == 5
			cap_info.DisChargingCur_Sum = (DisChargingCur_Sum - 75.3) / 0.04;
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 75.36f) / 0.04;
			#elif Cap_Board_ID == 6
			cap_info.DisChargingCur_Sum = (DisChargingCur_Sum - 75.66) / 0.04;
			cap_info.ChassisCur_Sum = (ChassisCur_Sum - 75.09f) / 0.04;
			#endif
			//6 75.66
			count = 0;
			DisChargingCur_Sum = 0.0f;
			ChassisCur_Sum = 0.0f;
		}
		if(count1 == 1000){
			cap_info.DisChargingCur_Base = sum1 / 1000.0f;
			cap_info.ChassisCur_Base = sum2 / 1000.0f;
			count1 = 0;
			sum1 = 0.0f;
			sum2 = 0.0f;
		}
		DisChargingCur_Sum += cap_info.DisChargingCur_Now;
		ChassisCur_Sum += cap_info.ChassisCur_Now;
		sum1 += cap_info.DisChargingCur_Now;
		sum2 += cap_info.ChassisCur_Now;
		
		count1++;
		count ++;

		/***************************************/
		FPS.adc1_dma++;
	}
}