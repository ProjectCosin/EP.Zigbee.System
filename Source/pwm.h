#ifndef PWM_H
#define PWM_H

#include "ioCC2530.h"
#include "OSAL_Nv.h"
#include "OSAL.h"
#include <string.h>
#include "AF.h"
#include "ds18b20.h"
#include "sampleapp.h"

/* 初始化定时器3  */
void Init_Timer3(void);
//初始化IO
void PWM_Initial(void);
//开关控制
void SampleApp_PWM_control(uint8 *data);
void SampleApp_PWM_State(int  switchstate);
//周期发送数据
void SampleApp_SendPeriodicMessage_PWM( void );
#endif