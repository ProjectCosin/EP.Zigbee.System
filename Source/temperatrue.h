#ifndef TEMPERATRUE_H
#define TEMPERATRUE_H

#include "ioCC2530.h"
#include "OSAL_Nv.h"
#include "OSAL.h"
#include <string.h>
#include "AF.h"
#include "ds18b20.h"
#include "sampleapp.h"


//��ʼ��
void Temprature_initial(void);
//������
unsigned int Temprature_read_data(unsigned char *buffer);
//���ڷ�������
void SampleApp_SendPeriodicMessage_wendu(void);
#endif