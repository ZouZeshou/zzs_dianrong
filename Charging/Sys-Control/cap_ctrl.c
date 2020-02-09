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
//4�� 1250
//3�� 1500
//5�� 1250
#define    CHARGING_POWER_THRESHOLD    2000  //�������Ƶ�77W�� ����30����Ϊ��PIDʱ����30��
#define		 CHARGING_VOLTAGE						 29.5  //��ѹ���ĵ�ѹ
#define    CHARGING_PID_DEBUG					 0

uint32_t JudgePassTime, JudgeFresh;

PID_AbsoluteType					charging_cur_pid;
CHARGING_CTRL_STRUCT 			cap_info;

void CapInfo_Init(void){
	#if Cap_Board_ID == 3
	pid_abs_para_init(&charging_cur_pid, 2.3f, 0.2f, 0.0f, 40000.0f, 8399.0f);
	#elif Cap_Board_ID == 4
	pid_abs_para_init(&charging_cur_pid, 2.3f, 0.2f, 0.0f, 40000.0f, 8399.0f);
	#elif Cap_Board_ID == 5
	pid_abs_para_init(&charging_cur_pid, 2.3f, 0.2f, 0.0f, 40000.0f, 8399.0f);
	#elif Cap_Board_ID == 6
	pid_abs_para_init(&charging_cur_pid, 2.3f, 0.2f, 0.0f, 40000.0f, 8399.0f);
	#endif
//	pid_abs_para_init(&charging_cur_pid, 5.0f, 0.3f, 0.0f, 8000.0f, 8399.0f);
	memset(&cap_info, 0, sizeof(CHARGING_CTRL_STRUCT));
	cap_info.ChargingCur_Base = 2.56;//30 times 2.480f;
	cap_info.ChassisCur_Base = 2.555;//30times 2.435f;
	cap_info.Chassis_Volt = 25.0f;
	cap_info.Cap_Volt = 15.0f;
	cap_info.ChassisPower_Buff = 60.0f;
}

/* ��������PID����
 * �ܹ������Ƴ���30�� ��ȥ�������ڵĵ�ѹ���Ե��������ڿ��ù���
 * �ٳ��Գ��ĵ�ѹ�� ����Ŀ�����ֵ
 * ���ص���ֵͨ��adc�����õ�
 * �������pwm��ռ�ձ�
*/
void ChargingControl(CHARGING_CTRL_STRUCT * cap_info)
{
	#if CHARGING_PID_DEBUG
		charging_cur_pid.kp = p;
		charging_cur_pid.ki = i;
		charging_cur_pid.kd = d;
	#endif
	/* ����Ŀ������� */
	cap_info->ChargingCur_Target = 
		(CHARGING_POWER_THRESHOLD - cap_info->Chassis_Volt * cap_info->ChassisCur_Sum) /\
		cap_info->Chassis_Volt;
	/* ����pid��� */
	charging_cur_pid.errNow = cap_info->ChargingCur_Target - cap_info->ChargingCur_Sum;
	/* ����pid��� */
	PID_AbsoluteMode(&charging_cur_pid);
	/* ����PWMռ�ձ� */
	if(cap_info->Cap_Volt < 15)
		charging_cur_pid.ctrOut *= 0.6;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,(uint16_t)charging_cur_pid.ctrOut);

}

