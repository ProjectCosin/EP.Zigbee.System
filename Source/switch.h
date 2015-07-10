#ifndef SWITCH_H
#define SWITCH_H

#include "ioCC2530.h"
#include "OSAL_Nv.h"
#include "OSAL.h"
#include <string.h>
#include "AF.h"
#include "ds18b20.h"
#include "sampleapp.h"



//��ʼ��IO
void Switch_Initial(void);
//���ؿ���
void SampleApp_Switch_control5(uint8 *data);

void SampleApp_Switch_State(int  switchstate);
//���ڷ�������
void SampleApp_SendPeriodicMessage_switch( void );
#endif