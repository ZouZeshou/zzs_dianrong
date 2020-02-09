#ifndef _BSP_IO_H
#define _BSP_IO_H


#include "stm32f4xx_hal.h"
void turn_on_laser(void);
void turn_off_laser(void);

void MY_PWM_ENABLE(void); 
void MY_VCCOUT_ENABLE(void);

#endif
