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
	float 		DisChargingCur_Now ;//每一次adc采样得到的正在充电的电流值
	float 		DisChargingCur_Base ;//电流采样 采集一千次得到的零点偏置值, 用来标定
	float 		DisChargingCur_Sum ;//adc采样30次的和，用来做pid
	float 		ChassisCur_Now;//ADC采样得到的底盘的电流值
	float 		ChassisCur_Base ;//电流采样 采集一千次得到的零点偏置值, 用来标定
	float 		ChassisCur_Sum;//ADC采样得到的底盘电流和，用来做PID
	float 		Cap_Volt;
	float 		Chassis_Volt;
	float 		Chassis_Power;
	float 		DisChargingCur_Target;//做pid使用的目标电流值
	float 		DisChargingCur_Back;//做pid使用的返回电流值， 等于DisChargingCur_Sum
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
