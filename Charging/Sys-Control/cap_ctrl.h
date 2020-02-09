#ifndef __CAP_CTRL_H_
#define __CAP_CTRL_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "pid.h"
#include "adc.h"
#include "tim.h"

#define  Current_Sensor_Cali  0
#define  Cap_Board_ID					6
typedef struct
{
  uint32_t adc1_dma;
  uint32_t adc2_dma;
  uint32_t timer3;
	uint32_t timer2;
	uint32_t timer8;
	uint32_t judge;
}OFFLINE_COUNT_STRUCT;
extern OFFLINE_COUNT_STRUCT FPS;

typedef struct{
	float ChargingCur_Now ;//ÿһ��adc�����õ������ڳ��ĵ���ֵ
	float ChargingCur_Base ;//�������� �ɼ�һǧ�εõ������ƫ��ֵ, �����궨
	float ChargingCur_Sum ;//adc����30�εĺͣ�������pid
	float ChassisCur_Now;//ADC�����õ��ĵ��̵ĵ���ֵ
	float ChassisCur_Base ;//�������� �ɼ�һǧ�εõ������ƫ��ֵ, �����궨
	float ChassisCur_Sum;//ADC�����õ��ĵ��̵����ͣ�������PID
	float Cap_Volt;
	float Chassis_Volt;
	float Chassis_Power;
	uint16_t ChassisPower_Buff;
	float ChargingCur_Target;//��pidʹ�õ�Ŀ�����ֵ
	float ChargingCur_Back;//��pidʹ�õķ��ص���ֵ�� ����ChargingCur_Sum
	uint16_t adc1_rx_buff[2];
	uint16_t adc2_rx_buff[1];
	
	uint8_t	can_use_Cap;
	uint32_t	judge_fresh_time;
}CHARGING_CTRL_STRUCT;

extern CHARGING_CTRL_STRUCT cap_info;
extern PID_AbsoluteType	charging_cur_pid;
extern uint32_t JudgePassTime, JudgeFresh;

void CapInfo_Init(void);
void ChargingControl(CHARGING_CTRL_STRUCT * cap_info);
#endif
