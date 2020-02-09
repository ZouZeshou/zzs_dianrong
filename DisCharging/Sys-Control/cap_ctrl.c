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

#define    DISCHARGING_POWER_THRESHOLD     2000  //�������Ƶ�75W�� ����30����Ϊ��PIDʱ����30��, ����
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
	cap_info.Chassis_Volt = 25.0f;//��ʼֵ�����can��ȡ�������̵�ѹ������25V���㣬���ᳬ����
}

/* ��������PID����
 * �ܹ������Ƴ���30�� ��ȥ�������ڵĵ�ѹ���Ե��������ڿ��ù���
 * �ٳ��Գ��ĵ�ѹ�� ����Ŀ�����ֵ
 * ���ص���ֵͨ��adc�����õ�
 * �������pwm��ռ�ձ�
*/
void DisChargingControl(CHARGING_CTRL_STRUCT * cap_info)
{
	#if DISCHARGING_PID_DEBUG
		discharging_cur_pid.kp = p;
		discharging_cur_pid.ki = i;
		discharging_cur_pid.kd = d;
	#endif
	/* ����Ŀ������� */
	cap_info->DisChargingCur_Target = cap_info->ChassisCur_Sum -\
																		DISCHARGING_POWER_THRESHOLD / cap_info->Chassis_Volt;
	/* ����pid��� */
	discharging_cur_pid.errNow = cap_info->DisChargingCur_Target -\
															 cap_info->DisChargingCur_Sum;
	/* ����pid��� */
	PID_AbsoluteMode(&discharging_cur_pid);
	/* ����PWMռ�ձ� */
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, (uint16_t)discharging_cur_pid.ctrOut);

}

