#include "bsp_tim.h"
#include "tim.h"
#include "string.h"
#include "main.h"
#include "stdio.h"
#include "cap_ctrl.h"
#include "bsp_can.h"
#include "ANO_DT.h"
static bool count = 0;
/**
 * @brief tim Callback
 * @param None
 * @return None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
	if(htim->Instance == TIM2)
	{//10KHZ 用来做电流环pid
		cap_ctrl();
	}
	else if(htim->Instance == TIM3)
	{//300KHZ 产生中断，出发ADC1采集数据
		FPS.timer3 ++;
	}
	else if(htim->Instance == TIM6)
	{//1HZ	离线检测， 计算帧率， 调试打印
		printf_function();
	} 
	else if (htim->Instance == TIM8)
	{//100HZ 产生中断，触发ADC2的采集
		ADC2_ctrl();
	}
}
/**
 * @brief current control function
 * @param None
 * @return None
 */
void cap_ctrl(void)
{
	static uint16_t init_cnts = 0;
	if(init_cnts > 300)
	{//初始化完成后先判断电容电压，是否插上电容
		if(cap_info.Cap_Volt < 5.0f)
		{
			cap_info.can_use_Cap = 0;
		}
		else if(cap_info.Cap_Volt > 5.0f)
		{//如果电容电压一开始大于5.0V，说明电容连接上，可以使用
			relay_ctrl();
			current_spd_ctrl();
		}
	}
	else
		init_cnts++;
	FPS.timer2 ++;
}
/**
 * @brief printf function for debug
 * @param None
 * @return None
 */
void printf_function(void)
{
	  HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);
		cap_info.judge_fresh_time = FPS.judge;
		/* printf */
		printf("/*****************************************/\r\n");
		printf("cap_info.judge_fresh_time = %d\r\n",  cap_info.judge_fresh_time);
		printf("FPS.ADC1_DMA = %d    FPS.ADC2_DMA = %d    FPS.judge = %d\r\n", \
								FPS.adc1_dma, FPS.adc2_dma, FPS.judge);
		printf("currentfps = %d  adc1fps = %d adc2fps =%d\r\n",FPS.timer2,FPS.timer3,FPS.timer8);
		printf("Cur_Target = %f    errNow = %f   ctrOut = %d\r\n",\
						cap_info.ChargingCur_Target,\
						charging_cur_pid.errNow,\
						__HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_1) /*charging_cur_pid.ctrOut*/);
#if !Current_Sensor_Cali
		printf("ChargingCur_Sum=%f,ChassisCur_Sum=%f,Chassis_Volt=%f\r\n",\
						cap_info.ChargingCur_Sum,
						cap_info.ChassisCur_Sum,\
						cap_info.Chassis_Volt);
		printf("Cap_Volt=%f,Chassis_Power=%f ,charging_base=%f \r\n",\
						cap_info.Cap_Volt ,\
						cap_info.Chassis_Power,\
						cap_info.ChargingCur_Base);
		printf("chassis_base=%f ,r_power_buffer=%d ,can_use_Cap=%d\r\n",\
						cap_info.ChassisCur_Base,\
						cap_info.ChassisPower_Buff,\
						cap_info.can_use_Cap);										
#else
		printf("ChargingCur_Now=%f   ChassisCur_Now=%f   \rChargingCur_Base=%f   ChassisCur_Base=%f   \rChassis_Volt=%f      Cap_Volt=%f    Chassis_Power=%f\r\n", cap_info.ChargingCur_Now, cap_info.ChassisCur_Now, cap_info.ChargingCur_Base, cap_info.ChassisCur_Base, cap_info.Chassis_Volt, cap_info.Cap_Volt , cap_info.Chassis_Power);
#endif
		/* monitor */
		memset(&FPS, 0, sizeof(OFFLINE_COUNT_STRUCT));
}
/**
 * @brief ADC2 control
 * @param None
 * @return None
 */
void ADC2_ctrl(void)
{
	FPS.timer8 ++;
	static  	wl4data			      float_bytes;
	static    CAN_TX_TYPEDEF		can1_Tx;
	float_bytes.f = cap_info.Cap_Volt;
	can1_Tx.data[0] = float_bytes.c[0] ;
	can1_Tx.data[1] = float_bytes.c[1] ;
	can1_Tx.data[2] = float_bytes.c[2] ;
	can1_Tx.data[3] = float_bytes.c[3] ;
	can1_Tx.data[4] = cap_info.can_use_Cap;
	memset(&can1_Tx.data[5],0,3);
//		ANO_DT_Send_Senser((float)cap_info.ChassisCur_Now/20.0f, (float)cap_info.Chassis_Power/cap_info.Chassis_Volt, 0, 0, 0, 0, 0, 0, 0, 0);
	CAN1_Send_bytes(0x308,can1_Tx.data);	
}
/**
 * @brief relay control
 * @param None
 * @return None
 */
void relay_ctrl(void)
{
	if(cap_info.ChassisPower_Buff <= 40&&Judge_RobotHurtData.hurt_type == 4)
	{//如果电容使用过程中，功率缓冲池下降到40以下，立马切断超级电容，不再使用	
		cap_info.can_use_Cap = 0;
		HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);//turn off
	}		
	else 
	{
		HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_RESET);//turn on
		if(cap_info.Cap_Volt > 12.0)
			cap_info.can_use_Cap = 1;
		//如果超级电容电压11.0V，则超级电容不可以用
		if(cap_info.Cap_Volt < 12.0)
			cap_info.can_use_Cap = 0;
	}
}
/**
 * @brief current speed control
 * @param None
 * @return None
 */
void current_spd_ctrl(void)
{
	if(cap_info.judge_fresh_time >= 3 &&\
		 cap_info.Chassis_Volt * cap_info.ChassisCur_Sum < 2100.0f)
	{//if the judgesystem is connected and power is less than 70w
			ChargingControl(&cap_info);
	}
	else
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,(uint16_t)0);
}