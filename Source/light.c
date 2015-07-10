#include "light.h"

extern afAddrType_t Point_To_Point_DstAddr;//点对点通信定义
extern endPointDesc_t SampleApp_epDesc;
extern uint8 SampleApp_TransID;  // This is the unique message ID (counter)


/*********************************************************************
 * @fn          SampleApp_SendPeriodicMessage_light_intensity
 * @brief       send light intensity sensor state periodly.
 * @param       null. 
 * @return      null.
 */
void SampleApp_SendPeriodicMessage_light_intensity( void )
{
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  uint16 advalue;
  uint8 *TmpBuf = osal_mem_alloc(32);
  uint8 *IEEEAddr;
  
  //初始化io
  P0DIR &= 0xfe;
  HalAdcInit();
  HalAdcSetReference(HAL_ADC_REF_125V);
  advalue = HalAdcRead(HAL_ADC_CHN_AIN0, HAL_ADC_RESOLUTION_14);
  //num_voltage = (float)(advalue)*1.15/8096.0;  //定参考电压为1.15V。14位精确度  
    
  IEEEAddr = NLME_GetExtAddr();
  TmpBuf[0] = '/';
  TmpBuf[1] = 'D';
  TmpBuf[2] = '|';
  osal_cpyExtAddr( &TmpBuf[3],IEEEAddr);
  TmpBuf[11] = '|';
  TmpBuf[12] = '1';//temperature
  TmpBuf[13] = '|';
  TmpBuf[14] = 'L';
  TmpBuf[15] = ',';
  TmpBuf[16] = asc_16[advalue/1000%10];
  TmpBuf[17] = asc_16[advalue/100%10];
  TmpBuf[18] = asc_16[advalue/10%10];
  TmpBuf[19] = asc_16[advalue/1%10];
  TmpBuf[20] = '|';
  TmpBuf[21] = '*';

  
  if ( AF_DataRequest(&Point_To_Point_DstAddr, //协调器地址0x0000
                      &SampleApp_epDesc,
                      SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                      22,                //数据length
                      TmpBuf,              //装载要发送的数据
                      &SampleApp_TransID,
                      AF_DISCV_ROUTE,
                      AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {  }
   osal_mem_free(TmpBuf);
}
