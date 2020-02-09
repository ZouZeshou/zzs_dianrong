/******************************************************************************/
/** @file bsp_can.c
 *  @version 1.0
 *  @date 2018.12.8
 *  @brief can receive callback and the bsp functions
 *  @author Echo
 */

/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "bsp_can.h"
#include "cap_ctrl.h"

/* Private typedef -----------------------------------------------------------*/
//can接收邮箱的结构体
CAN_TX_TYPEDEF 		can1_tx;
CAN_TX_TYPEDEF 		can2_tx;
CAN_RX_TYPEDEF 		can1_rx;
CAN_RX_TYPEDEF 		can2_rx;
/* Private functions ---------------------------------------------------------*/
ext_power_heat_data_t	Judge_PowerHeatData;
wl2data				 				data2bytes;
wl4data				 				data4bytes;
/**
 * @brief CAN1 Receive completely Callback
 * @param None
 * @return None
 * @attention it must be called if CAN1 need be used
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
	//*Do not printf !!*/
	if(hcan->Instance == CAN1)
	{
		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0,&can1_rx.header, can1_rx.data);
		switch(can1_rx.header.StdId)
		{
			case 0x103:{
				data2bytes.c[0] = can1_rx.data[0];
				data2bytes.c[1] = can1_rx.data[1];
				Judge_PowerHeatData.chassis_volt = data2bytes.ud;
				cap_info.Chassis_Volt = (float)Judge_PowerHeatData.chassis_volt * 0.001f;
				
				data2bytes.c[0] = can1_rx.data[2];
				data2bytes.c[1] = can1_rx.data[3];
				Judge_PowerHeatData.chassis_current = data2bytes.ud;

				data4bytes.c[0] = can1_rx.data[4];
				data4bytes.c[1] = can1_rx.data[5];
				data4bytes.c[2] = can1_rx.data[6];
				data4bytes.c[3] = can1_rx.data[7];
				Judge_PowerHeatData.chassis_power = data4bytes.f;
				cap_info.Chassis_Power = Judge_PowerHeatData.chassis_power ;
				FPS.judge++;
			break;
			}
			case 0x104:{
				data2bytes.c[0] = can1_rx.data[0];
				data2bytes.c[1] = can1_rx.data[1];
				Judge_PowerHeatData.chassis_power_buffer = data2bytes.ud;
				cap_info.ChassisPower_Buff = Judge_PowerHeatData.chassis_power_buffer;
				break;
			}
			case 0x308:{
				data4bytes.c[0] = can1_rx.data[0];
				data4bytes.c[1] = can1_rx.data[1];
				data4bytes.c[2] = can1_rx.data[2];
				data4bytes.c[3] = can1_rx.data[3];
				cap_info.Cap_Volt = data4bytes.f; 
			break;
			}
			case 0x199:{
				cap_info.use_Cap = can1_rx.data[0]; 
			break;
			}
		}
	}
	
}

/**
 * @brief CAN1 send msg functions using the can transmit_IT
 * @param 1. can_send_id
 *				2. data
 * @return None
 * @attention you can choose the suitable one to use according 
 *						to the data type you want to transmit
 */
void CAN1_Send_Current(uint32_t id,int16_t cur1,int16_t cur2, int16_t cur3, int16_t cur4 ){
	can1_tx.header.DLC=0x08;
	can1_tx.header.IDE=CAN_ID_STD;
	can1_tx.header.RTR=CAN_RTR_DATA;
	can1_tx.header.StdId=id;
	
	can1_tx.data[0]=(unsigned char)(cur1>>8);
	can1_tx.data[1]=(unsigned char)(cur1);
	can1_tx.data[2]=(unsigned char)(cur2>>8);
	can1_tx.data[3]=(unsigned char)(cur2);
	can1_tx.data[4]=(unsigned char)(cur3>>8);
	can1_tx.data[5]=(unsigned char)(cur3);
	can1_tx.data[6]=(unsigned char)(cur4>>8);
	can1_tx.data[7]=(unsigned char)(cur4);
	
	HAL_CAN_AddTxMessage(&hcan1,&can1_tx.header,can1_tx.data,(uint32_t *)CAN_TX_MAILBOX0);
}

void CAN1_Send_bytes(uint32_t id,uint8_t data[8]){
	can1_tx.header.DLC=0x08;
	can1_tx.header.IDE=CAN_ID_STD;
	can1_tx.header.RTR=CAN_RTR_DATA;
	can1_tx.header.StdId=id;
	can1_tx.data[0]= data[0];
	can1_tx.data[1]= data[1];
	can1_tx.data[2]= data[2];
	can1_tx.data[3]= data[3];
	can1_tx.data[4]= data[4];
	can1_tx.data[5]= data[5];
	can1_tx.data[6]= data[6];
	can1_tx.data[7]= data[7];
	HAL_CAN_AddTxMessage(&hcan1, &can1_tx.header, can1_tx.data, (uint32_t *)CAN_TX_MAILBOX0);
}


/***********************************************************************/
/***********************************************************************/
/************** The follow functions are the BSP functions *************/
/***********************************************************************/
/***********************************************************************/
/**
 * @brief Enable CAN1
 * @param None
 * @return None
 * @attention it must be called if CAN1 need be used
 */
void MY_CAN1_ENABLE(void)
{
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	CanFilter_Init(&hcan1);
}

void CanFilter_Init(CAN_HandleTypeDef* hcan)
{
/* *************************************************************
 * 对于BankNumber 这一项设置，作用是将过滤器组分成两个段
 * 第一段为CAN1使用（范围是0——BankNumber - 1），第二段为can2使用
*/
/*	BankNumber(FMR->CAN2SB)   				对于CAN1生效的过滤器段							对于CAN2生效的过滤器段	*/
/**************************************************************************************************/
/*						0																	0																		0~27					*/
/**************************************************************************************************/
/*			num = 1~27												  	0~num-1																num~27				*/

  /* can1 */
	/* filter 0 */
  uint16_t StdId_Chas1 =0x308;//chassis motor lf  dji m3508                 
  uint16_t StdId_Chas2 =0x202;//chassis motor rf  dji m3508  
  uint16_t StdId_Chas3 =0x203;//chassis motor ld  dji m3508  
  uint16_t StdId_Chas4 =0x204;//chassis motor rd  dji m3508 
	/* filter 1*/
  uint16_t StdId_Yaw =0x199;//yaw motor 				 dji GM6020  
  uint16_t StdId_Pitch =0x206;//pitch motor 			 dji GM6623
  uint16_t StdId_ChasImu =0x313;//chassis imu 	
  uint16_t StdId_GameState =0x101; 
	/* filter 2 */
  uint16_t StdId_RobotState =0x102;
  uint16_t StdId_PowerHeat1 =0x103;
  uint16_t StdId_PowerHeat2 =0x104;
  uint16_t StdId_RobotPos1 =0x105;
	/* filter 3 */
  uint16_t StdId_RobotPos2 =0x106;
  uint16_t StdId_BuffMusk =0x107; 
  uint16_t StdId_RobotHurt =0x111;
  uint16_t StdId_Shootdata =0x112;
	
	/* can2 */	
  uint16_t StdId_Trigger =0x207;//trigger motor 		 dji m2006
  uint16_t StdId_GimImu =0x401;//gimbal imu
  uint16_t StdId_FricL =0x201;//frictiont motor  dji 2006
  uint16_t StdId_FricR =0x202;//frictiont motor  dji 2006
	uint16_t StdId_GimImu2 = 0x405;
	CAN_FilterTypeDef canfilter;
	
	canfilter.SlaveStartFilterBank=14;
  //use different filter for can1&can2
  if(hcan == &hcan1)
  {
		
		canfilter.FilterActivation=ENABLE;
		canfilter.FilterBank=0U;
		canfilter.FilterMode=CAN_FILTERMODE_IDLIST;//列表模式
		canfilter.FilterScale=CAN_FILTERSCALE_16BIT;//16位宽
		canfilter.FilterFIFOAssignment=CAN_FILTER_FIFO0;
		canfilter.FilterIdHigh = StdId_Chas1<<5;
		canfilter.FilterIdLow = StdId_Chas2<<5;
		canfilter.FilterMaskIdHigh = StdId_Chas3<<5;
		canfilter.FilterMaskIdLow = StdId_Chas4<<5;
		HAL_CAN_ConfigFilter(hcan,&canfilter);
		
		canfilter.FilterActivation=ENABLE;
		canfilter.FilterBank=1U;
		canfilter.FilterMode=CAN_FILTERMODE_IDLIST;//列表模式
		canfilter.FilterScale=CAN_FILTERSCALE_16BIT;//16位宽
		canfilter.FilterFIFOAssignment=CAN_FILTER_FIFO0;
		canfilter.FilterIdHigh = StdId_Yaw<<5;
		canfilter.FilterIdLow = StdId_Pitch<<5;
		canfilter.FilterMaskIdHigh = StdId_ChasImu<<5;
		canfilter.FilterMaskIdLow = StdId_GameState << 5;
		HAL_CAN_ConfigFilter(hcan,&canfilter);
		
		canfilter.FilterActivation=ENABLE;
		canfilter.FilterBank=2U;
		canfilter.FilterMode=CAN_FILTERMODE_IDLIST;//列表模式
		canfilter.FilterScale=CAN_FILTERSCALE_16BIT;//16位宽
		canfilter.FilterFIFOAssignment=CAN_FILTER_FIFO0;
		canfilter.FilterIdHigh = StdId_RobotState<<5;
		canfilter.FilterIdLow = StdId_PowerHeat1<<5;
		canfilter.FilterMaskIdHigh = StdId_PowerHeat2<<5;
		canfilter.FilterMaskIdLow = StdId_RobotPos1 << 5;
		HAL_CAN_ConfigFilter(hcan,&canfilter);

		canfilter.FilterActivation=ENABLE;
		canfilter.FilterBank=3U;
		canfilter.FilterMode=CAN_FILTERMODE_IDLIST;//列表模式
		canfilter.FilterScale=CAN_FILTERSCALE_16BIT;//16位宽
		canfilter.FilterFIFOAssignment=CAN_FILTER_FIFO0;
		canfilter.FilterIdHigh = StdId_RobotPos2<<5;
		canfilter.FilterIdLow  	= StdId_BuffMusk<<5;
		canfilter.FilterMaskIdHigh = StdId_RobotHurt<<5;
		canfilter.FilterMaskIdLow = StdId_Shootdata << 5;
		HAL_CAN_ConfigFilter(hcan,&canfilter);
	
	
  }
}


