/******************************************************************************/
/** @file cap_ctrl.c
 *  @version 1.0
 *  @date 2019.4.6
 *  @brief ctrl charging 
 *  @author Echo
 */

/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "cap_ctrl.h"
#include "string.h"
#include "tim.h"
#include "stmgood.h"

#define    DISCHARGING_POWER_THRESHOLD     2000  //功率限制到75W， 乘以30是因为做PID时都是30倍, 大于
#define    DISCHARGING_PID_DEBUG					 0

PID_AbsoluteType					discharging_cur_pid;
CHARGING_CTRL_STRUCT 			cap_info;

void CapInfo_Init(void){
	#if Cap_Board_ID == 3
		pid_abs_para_init(&discharging_cur_pid, 3.0f, 1.8f, 0.0f, 10000.0f, 8399.0f);
	#elif Cap_Board_ID == 4
		pid_abs_para_init(&discharging_cur_pid, 3.0f, 1.8f, 0.0f, 10000.0f, 8399.0f);
	#elif Cap_Board_ID == 5
		pid_abs_para_init(&discharging_cur_pid, 3.0f, 1.8f, 0.0f, 10000.0f, 8399.0f);
	#elif Cap_Board_ID == 6
		pid_abs_para_init(&discharging_cur_pid, 3.0f, 1.8f, 0.0f, 10000.0f, 8399.0f);
	#endif
	memset(&cap_info, 0, sizeof(CHARGING_CTRL_STRUCT));
	cap_info.Chassis_Volt = 25.0f;//初始值，如果can读取不到底盘电压，则用25V计算，不会超功率
}

/* 充电电流环PID控制
 * 总功率限制乘以30， 减去底盘现在的电压乘以电流，等于可用功率
 * 再除以充电的电压， 就是目标电流值
 * 返回电流值通过adc采样得到
 * 输出量是pwm的占空比
*/
void DisChargingControl(CHARGING_CTRL_STRUCT * cap_info)
{
	#if DISCHARGING_PID_DEBUG
		discharging_cur_pid.kp = p;
		discharging_cur_pid.ki = i;
		discharging_cur_pid.kd = d;
	#endif
	/* 计算目标充电电流 */
	cap_info->DisChargingCur_Target = cap_info->ChassisCur_Sum -\
																		DISCHARGING_POWER_THRESHOLD / cap_info->Chassis_Volt;
	/* 计算pid误差 */
	discharging_cur_pid.errNow = cap_info->DisChargingCur_Target -\
															 cap_info->DisChargingCur_Sum;
	/* 计算pid输出 */
	PID_AbsoluteMode(&discharging_cur_pid);
	/* 调节PWM占空比 */
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, (uint16_t)discharging_cur_pid.ctrOut);

}

