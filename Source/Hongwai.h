#ifndef HONGWAI_H
#define HONGWAI_H

#include "ioCC2530.h"
#include "OSAL_Nv.h"
#include "OSAL.h"
#include <string.h>
#include "AF.h"
#include "ds18b20.h"
#include "sampleapp.h"



//初始化IO
void Hongwai_Initial(void);
//开关控制
void SampleApp_Hongwai_control(uint8 *data);

void SampleApp_Hongwai_State(int  state);
//周期发送数据
void SampleApp_SendPeriodicMessage_Hongwai( void );
#endif