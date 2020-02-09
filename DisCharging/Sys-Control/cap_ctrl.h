#ifndef __CAP_CTRL_H_
#define __CAP_CTRL_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "pid.h"
#include "adc.h"
#include "tim.h"

#define  Current_Sensor_Cali  0
#define  Cap_Board_ID   		  6
typedef struct
{
  uint32_t adc1_dma;
  uint32_t adc2_dma;
  uint32_t timer3;
	uint32_t timer2;
	uint32_t timer5;
	uint32_t judge;
}OFFLINE_COUNT_STRUCT;
extern OFFLINE_COUNT_STRUCT FPS;

typedef struct{
	float 		DisChargingCur_Now ;//ÿһ��adc�����õ������ڳ��ĵ���ֵ
	float 		DisChargingCur_Base ;//�������� �ɼ�һǧ�εõ������ƫ��ֵ, �����궨
	float 		DisChargingCur_Sum ;//adc����30�εĺͣ�������pid
	float 		ChassisCur_Now;//ADC�����õ��ĵ��̵ĵ���ֵ
	float 		ChassisCur_Base ;//�������� �ɼ�һǧ�εõ������ƫ��ֵ, �����궨
	float 		ChassisCur_Sum;//ADC�����õ��ĵ��̵����ͣ�������PID
	float 		Cap_Volt;
	float 		Chassis_Volt;
	float 		Chassis_Power;
	float 		DisChargingCur_Target;//��pidʹ�õ�Ŀ�����ֵ
	float 		DisChargingCur_Back;//��pidʹ�õķ��ص���ֵ�� ����DisChargingCur_Sum
	uint16_t  adc1_rx_buff[2];
	uint16_t  adc2_rx_buff[1];
	uint16_t  ChassisPower_Buff;
	uint8_t		use_Cap;
}CHARGING_CTRL_STRUCT;

extern CHARGING_CTRL_STRUCT cap_info;
extern PID_AbsoluteType	discharging_cur_pid;

void CapInfo_Init(void);
void DisChargingControl(CHARGING_CTRL_STRUCT * cap_info);
#endif
