#ifndef PWM_H
#define PWM_H

#include "ioCC2530.h"
#include "OSAL_Nv.h"
#include "OSAL.h"
#include <string.h>
#include "AF.h"
#include "ds18b20.h"
#include "sampleapp.h"

/* ��ʼ����ʱ��3  */
void Init_Timer3(void);
//��ʼ��IO
void PWM_Initial(void);
//���ؿ���
void SampleApp_PWM_control(uint8 *data);
void SampleApp_PWM_State(int  switchstate);
//���ڷ�������
void SampleApp_SendPeriodicMessage_PWM( void );
#endif