/******************************************************************************/
/** @file pid.c
 *  @version 1.0
 *  @date 2018.12.8
 *
 *  @brief the declerations of abs/inc pid functions 
 *
 *  @author 
 *
 */
 /*****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "pid.h"
#include "string.h"
#include "math.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
/*********************增量式PID控制***********************/
void PID_IncrementMode(PID_IncrementType *pid)
{
float dErrP,dErrI,dErrD;
	
	/*if(pid->kp<0) pid->kp=-pid->kp;
	if(pid->ki<0) pid->ki=-pid->ki;
	if(pid->kd<0) pid->kd=pid->kd;*/
	
	dErrP = pid->errNow-pid->errold1;
	dErrI = pid->errNow;
	dErrD = pid->errNow - 2*pid->errold1 + pid->errold2;
	
	pid->errold2 = pid->errold1;
	pid->errold1 = pid->errNow;
	
	pid->dCtrOut = pid->kp * dErrP + pid->ki * dErrI + pid->kd*dErrD;
	
//	printf("errNow=%f\r\n ctrOut=%f\r\n errold1=%f\r\n errold2=%f\r\n dCtrOut=%f\r\n",pid->errNow,pid->ctrOut,pid->errold1,pid->errold2,pid->dCtrOut);
	
	if(pid->kp==0&&pid->ki==0&&pid->kd==0)
		pid->ctrOut=0;
	else
		pid->ctrOut += pid->dCtrOut;
	if(pid->MaxOut != 0){
		if(pid->ctrOut > pid->MaxOut) pid->ctrOut = pid->MaxOut;
		if(pid->ctrOut < - pid->MaxOut) pid->ctrOut = -pid->MaxOut;
	}
}
/********************绝对式PID控制**************************/
void PID_AbsoluteMode(PID_AbsoluteType *pid) //
{
	pid->errP=pid->errNow;
	
	/* whether use the integral separation */
	if(pid->integral_separate != 0){

		if(pid->errIPoint == 0){
			pid->errI += pid->errNow;
		}else{
			if(fabs(pid->errNow) < fabs(pid->errIPoint)){
				pid->errI += (pid->errIPoint - fabs(pid->errNow))/pid->errIPoint*pid->errNow;
			}
		}
		
		bool beta = BetaGeneration(pid);

		if(pid->errILim != 0)	//抗积分饱和
		{
			if(pid->errI >  pid->errILim)      pid->errI =  pid->errILim;
			else if(pid->errI < -pid->errILim) pid->errI = -pid->errILim;
		}

		pid->errD = pid->errNow-pid->errOld;
		if(fabs(pid->errNow) < fabs(pid->errDPoint)){
			pid->errD *= 0.1f;
		}
		
		if(beta != 0)
			pid->ctrOut = pid->kp * pid->errP + pid->ki * pid->errI + pid->kd * pid->errD;
		else
			pid->ctrOut = pid->kp * pid->errP + pid->kd * pid->errD;
	}
	else{
		
		if(pid->errIPoint == 0){
			pid->errI += pid->errNow;
		}else{
			if(fabs(pid->errNow) < fabs(pid->errIPoint)){
				pid->errI += (pid->errIPoint - fabs(pid->errNow))/pid->errIPoint*pid->errNow;
			}
		}

		if(pid->errILim != 0)	//抗积分饱和
		{
			if(pid->errI >  pid->errILim)      pid->errI =  pid->errILim;
			else if(pid->errI < -pid->errILim) pid->errI = -pid->errILim;
		}

		pid->errD = pid->errNow-pid->errOld;
		if(fabs(pid->errNow) < fabs(pid->errDPoint)){
			pid->errD *= 0.1f;
		}
		pid->errOld =pid->errNow;
		pid->ctrOut = pid->kp*pid->errP + pid->ki * pid->errI + pid->kd*pid->errD;
	}
	
	/* limit the max output */
	if(pid->MaxOut != 0){
		if(pid->ctrOut >  pid->MaxOut) pid->ctrOut =  pid->MaxOut;
		if(pid->ctrOut < -pid->MaxOut) pid->ctrOut = -pid->MaxOut;
	}
}

/**
 * @brief   init the parameters of the absolutely mode pid
 * @param 	PID_AbsoluteType *pid, float kp, float ki, float kd, float errILim, float MaxOutCur		
 * @return None
 * @attention None
 */
void pid_abs_para_init(PID_AbsoluteType *pid, float kp, float ki, float kd, float errILim, float MaxOutCur)
{
	memset(pid,0,sizeof(PID_AbsoluteType));
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->errILim = errILim;
	pid->MaxOut = MaxOutCur;
}

/**
 * @brief   update the parameters of the absolutely mode pid
 * @param 	PID_AbsoluteType *pid, float kp, float ki, float kd	
 * @return None
 * @attention None
 */
void MutiPID_Adjust(bool dir, PID_AbsoluteType *pid, float err, float small_limit, float big_limit, float p1, float i1, float d1, float p2, float i2, float d2)
{
	if(dir != 0)
	{
		if(err < small_limit)
		{
			pid->kp = p1;
			pid->ki = i1;
			pid->kd = d1;
			dir = 0;
		}
	}
	else
	{
		if(err > big_limit)
		{
			pid->kp = p2;
			pid->ki = i2;
			pid->kd = d2;
			dir = 1;
		}
	}
}

/* use this function to generate the Intergral separation threshold value 
 */
static bool BetaGeneration(PID_AbsoluteType *pid)
{
	uint16_t beta=0;
	if(fabs(pid->errNow) <= pid->err_threshold) beta=1;
	else beta = 0;
	return beta;
}
