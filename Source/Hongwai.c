#include "Hongwai.h"

extern afAddrType_t Point_To_Point_DstAddr;//点对点通信定义
extern endPointDesc_t SampleApp_epDesc;
extern uint8 SampleApp_TransID;  // This is the unique message ID (counter)


bool Hongwai_state=true;

/*********************************************************************
 * @fn          Switch_Initial
 * @brief       initial IO .
 * @param       null - . 
 * @return      return  null.
 */
void Hongwai_Initial(void)
{
  P1DIR |= 0x7f;         
  //P1_7 = 1; //off
  return;
}
/*********************************************************************
 * @fn          SampleApp_Switch_control
 * @brief       command from hccu to control switch.
 * @param       Data - command. 
 * @return      return null.
 */
void SampleApp_Hongwai_control(byte *Data)
{ 
   if(Data[10] == 'E'){
      if(Data[12]=='1'){
        SampleApp_Hongwai_State(1);
      }else if(Data[12]=='0' ){
        SampleApp_Hongwai_State(0);
      }
    }
}
/*********************************************************************
 * @fn          SampleApp_SendPeriodicMessage_switch
 * @brief       send and control switch state periodly.
 * @param       null. 
 * @return      null.
 */
void SampleApp_SendPeriodicMessage_Hongwai( void )
{
  uint8 *TmpBuf = osal_mem_alloc(24);
  uint8 *IEEEAddr;
  //get MAC address
  IEEEAddr = NLME_GetExtAddr();
  TmpBuf[0] = '/';
  TmpBuf[1] = 'D';
  TmpBuf[2] = '|';
  osal_cpyExtAddr( &TmpBuf[3],IEEEAddr);
  TmpBuf[11] = '|';
  TmpBuf[12] = '1';
  TmpBuf[13] = '|';
  TmpBuf[14] = 'h';//status,{对应属性值}|*
  TmpBuf[15] = 'o';
  TmpBuf[16] = 'n';
  TmpBuf[17] = 'g';
  TmpBuf[18] = 'w';
  TmpBuf[19] = 'a';
  TmpBuf[20] = ',';
  //open or close 
  if(P1_7){
     TmpBuf[21] = '1';
  }else{
     TmpBuf[21] = '0';
  }
  TmpBuf[22] = '|';
  TmpBuf[23] = '*';
  //send
  if ( AF_DataRequest(&Point_To_Point_DstAddr, //协调器地址0x0000
                      &SampleApp_epDesc,
                      SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                      24,                //数据length
                      TmpBuf,              //装载要发送的数据
                      &SampleApp_TransID,
                      AF_DISCV_ROUTE,
                      AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {  }
  osal_mem_free(TmpBuf);
}
