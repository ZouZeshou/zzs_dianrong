#ifndef __BSP_CAN_H_
#define __BSP_CAN_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"

//0x0202 ʵʱ������������
typedef struct {
	uint16_t chassis_volt; /* ���������ѹ mV */
	uint16_t chassis_current; /* ����������� mA*/
	float chassis_power; /* ����������� W */
	uint16_t chassis_power_buffer; /* ���̹��ʻ��� J */
	uint16_t shooter_heat0; /* 17mmǹ������ */
	uint16_t shooter_heat1; /* 42mmǹ������ */
} ext_power_heat_data_t;

typedef struct {
	uint8_t armor_id : 4; /* װ���˺�ʱΪװ��ID ����Ϊ0 */
	uint8_t hurt_type : 4; /* 0 װ���˺� 
												  * 1 ģ�����߿�Ѫ 
												  * 2 ǹ�ڳ�������Ѫ 
												  * 3 ���̳����ʿ�Ѫ */
} ext_robot_hurt_t;

typedef union{
	uint8_t c[2];
	int16_t d;
	uint16_t ud;
}wl2data;

typedef union{
	uint8_t c[4];
	float f;
	uint32_t d;
}wl4data;

typedef struct{
	CAN_TxHeaderTypeDef 		header;
	uint8_t   							data[8];
}CAN_TX_TYPEDEF;

typedef struct{
	CAN_RxHeaderTypeDef 		header;
	uint8_t   							data[8];
}CAN_RX_TYPEDEF;
	
typedef enum{
	type_yaw  = 0,
	type_pitch = 1,
	type_trig = 2
}MOTORTYPE_ENUM;

typedef struct{
	union{
	uint8_t 								bit[2];
	int16_t 								data;
	}pit_spd;
	union{
	uint8_t 								bit[2];
	int16_t 								data;
	}yaw_spd;
	union{
	uint8_t									bit[4];
	float										data;
	}yaw_ang;
	union{
	uint8_t 								bit[2];
	int16_t 								data;
	}offset_spd;
}ImuData_Struct;

extern ImuData_Struct imu_can_gim,imu_can_chas;
extern CAN_TX_TYPEDEF 		can1_tx, can2_tx;
extern CAN_RX_TYPEDEF 		can1_rx, can2_rx;
extern ext_power_heat_data_t	Judge_PowerHeatData;
extern ext_robot_hurt_t			Judge_RobotHurtData;
void	MY_CAN1_ENABLE(void);
void	MY_CAN2_ENABLE(void);
void  CanFilter_Init(CAN_HandleTypeDef* hcan);
void  CAN1_Send_Current(uint32_t id,int16_t cur1,int16_t cur2, int16_t cur3, int16_t cur4 );
void  CAN2_Send_Current(uint32_t id,int16_t cur1,int16_t cur2, int16_t cur3, int16_t cur4 );
void  CAN1_Send_bytes(uint32_t id,uint8_t data[8]);
#endif
