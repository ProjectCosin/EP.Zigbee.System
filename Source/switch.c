#include "switch.h"

extern afAddrType_t Point_To_Point_DstAddr;//点对点通信定义
extern endPointDesc_t SampleApp_epDesc;
extern uint8 SampleApp_TransID;  // This is the unique message ID (counter)


bool switch_state=true;

/*********************************************************************
 * @fn          Switch_Initial
 * @brief       initial IO .
 * @param       null - . 
 * @return      return  null.
 */
void Switch_Initial(void)
{
  P1DIR |= 0xff;         
  P1_7 = 1; //off
  return;
}
/*********************************************************************
 * @fn          SampleApp_Switch_State
 * @brief       control switch.
 * @param       status - 1 on, 0 off. 
 * @return      return null.
 */
void SampleApp_Switch_State(int  switchstate)
{  
  if(switchstate){	 
    P1_7 = 0;
    switch_state = true;
  }else{
    P1_7 = 1;
    switch_state = false;
  }
}
/*********************************************************************
 * @fn          SampleApp_Switch_control
 * @brief       command from hccu to control switch.
 * @param       Data - command. 
 * @return      return null.
 */
void SampleApp_Switch_control5(byte *Data)
{ 
   if(Data[10] == 'E'){
      if(Data[12]=='1'){
        SampleApp_Switch_State(1);
      }else if(Data[12]=='0' ){
        SampleApp_Switch_State(0);
      }
    }
}
/*********************************************************************
 * @fn          SampleApp_SendPeriodicMessage_switch
 * @brief       send and control switch state periodly.
 * @param       null. 
 * @return      null.
 */
void SampleApp_SendPeriodicMessage_switch( void )
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
  TmpBuf[14] = 's';//status,{对应属性值}|*
  TmpBuf[15] = 't';
  TmpBuf[16] = 'a';
  TmpBuf[17] = 't';
  TmpBuf[18] = 'u';
  TmpBuf[19] = 's';
  TmpBuf[20] = ',';
  //open or close 
  if(switch_state){
    TmpBuf[21] = '1';//1,on; 0,off
  }else{
    TmpBuf[21] = '0';//1,on; 0,off
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
