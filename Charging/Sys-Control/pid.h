#ifndef __PID_H__
#define __PID_H__

#include "stdbool.h"
#include "stdint.h"
/********increase mode*******/
typedef  struct
{
	float kp;
	float ki;
	float kd;
	
	float errNow;
	float dCtrOut;
	float ctrOut;
	
	float errold1;
	float errold2;
	
	float MaxOut;

}PID_IncrementType;
/******* absolute mode********/
typedef struct
{
	float kp;
	float ki;
	float kd;	
	float errILim;
	float errNow; 
	float ctrOut;
	float errOld;
	float errP;
	float errI;
	float errD;
	float MaxOut;
	
	float errIPoint;
	float errDPoint;
	
	bool  integral_separate;//whether use the integral separation
	float err_threshold;//the threshold of the integral separation
}PID_AbsoluteType;


void PID_IncrementMode(PID_IncrementType *);        //增量式pid函数
void PID_AbsoluteMode(PID_AbsoluteType *);					//位置式pid函数
void pid_abs_para_init(PID_AbsoluteType *pid, float kp, float ki, float kd, float errILim, float MaxOutCur);
void MutiPID_Adjust(bool dir, PID_AbsoluteType *pid, float err, float small_limit, float big_limit, float p1, float i1, float d1, float p2, float i2, float d2);
static bool BetaGeneration(PID_AbsoluteType *pid);
#endif
