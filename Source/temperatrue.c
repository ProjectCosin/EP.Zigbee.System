#include "temperatrue.h"

extern afAddrType_t Point_To_Point_DstAddr;//点对点通信定义
extern endPointDesc_t SampleApp_epDesc;
extern uint8 SampleApp_TransID;  // This is the unique message ID (counter)


unsigned char DS18B20_1[8]={40,252,5,130,4,0,0,131};
unsigned int  M_wendu;
/*********************************************************************
 * @fn          Temprature_initial
 * @brief       initial temperature .
 * @param       buffer - . 
 * @return      return temprature data.
 */
void Temprature_initial()
{

  return;
}
/*********************************************************************
 * @fn          Temprature_read_data
 * @brief       read temperature .
 * @param       buffer - . 
 * @return      return temprature data.
 */
unsigned int Temprature_read_data(unsigned char *buffer)
{
  unsigned char temh,teml; 
  unsigned int wendu;
  init_1820();  //复位18b20  
  write_1820(0xcc); 
  write_1820(0x44);     //启动  
  Delay_nus(50000);
  init_1820();  
  write_1820(0xcc); 
  write_1820(0xbe); 
  teml=read_1820();  //读数据  
  temh=read_1820();
  wendu=temh*256+teml;
  return(wendu);   //返回温度数据
}

/*********************************************************************
 * @fn          SampleApp_SendPeriodicMessage_wendu
 * @brief       send Temperature sensor state periodly.
 * @param       null. 
 * @return      null.
 */
void SampleApp_SendPeriodicMessage_wendu(void)
{
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  uint8 *TmpBuf = osal_mem_alloc(22);
  uint8 *IEEEAddr;
  
  IEEEAddr = NLME_GetExtAddr();
  TmpBuf[0] = '/';
  TmpBuf[1] = 'D';
  TmpBuf[2] = '|';
  osal_cpyExtAddr( &TmpBuf[3],IEEEAddr);
  TmpBuf[11] = '|';
  TmpBuf[12] = '1';
  TmpBuf[13] = '|';
  TmpBuf[14] = 'T';
  TmpBuf[15] = ',';
  M_wendu= Temprature_read_data(DS18B20_1);   //采样温				  
  TmpBuf[16] = asc_16[M_wendu/100%10];
  TmpBuf[17] = asc_16[M_wendu/10%10];
  TmpBuf[18] = asc_16[M_wendu/1%10];
  TmpBuf[19] = '|';
  TmpBuf[20] = '*';
  if ( AF_DataRequest(&Point_To_Point_DstAddr, //协调器地址0x0000
                      &SampleApp_epDesc,
                      SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                      21,                //数据length
                      TmpBuf,              //装载要发送的数据
                      &SampleApp_TransID,
                      AF_DISCV_ROUTE,
                      AF_DEFAULT_RADIUS) == afStatus_SUCCESS )
  {  }
  
   osal_mem_free(TmpBuf);
}

