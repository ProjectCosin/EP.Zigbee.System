#ifndef TEMPERATRUE_H
#define TEMPERATRUE_H

#include "ioCC2530.h"
#include "OSAL_Nv.h"
#include "OSAL.h"
#include <string.h>
#include "AF.h"
#include "ds18b20.h"
#include "sampleapp.h"


//初始化
void Temprature_initial(void);
//读数据
unsigned int Temprature_read_data(unsigned char *buffer);
//周期发送数据
void SampleApp_SendPeriodicMessage_wendu(void);
#endif