/**************************************************************************************************
  Filename:       SampleApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $
  Description:    Sample Application (no Profile).

  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends it's messages either as broadcast or
  broadcast filtered group messages.  The other (more normal)
  message addressing is unicast.  Most of the other sample
  applications are written to support the unicast message model.

  Key control:
    SW1:  Sends a flash command to all devices in Group 1.
    SW2:  Adds/Removes (toggles) this device in and out
          of Group 1.  This will enable and disable the
          reception of the flash command.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL_Nv.h"
#include "hal_flash.h"
#include <string.h>
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"


#include "temperatrue.h"
#include "switch.h"
#include "light.h"
#include "Hongwai.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define OSAL_NV_PAGES_USED      HAL_NV_PAGE_CNT
#define OSAL_NV_PAGE_BEG        HAL_NV_PAGE_BEG
#define OSAL_NV_PAGE_END       (OSAL_NV_PAGE_BEG + OSAL_NV_PAGES_USED - 1)
/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};



/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   
devStates_t SampleApp_NwkState;
afAddrType_t Sample_Coordinator_DstAddr;//点对点通信定义
endPointDesc_t SampleApp_epDesc;
uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t Point_To_Point_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;
aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;
uint16 DstnwkAddr;

bool delete_ep = false;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void myApp_StartReporting( void );
void delay(int ms);
void SampleApp_BroadcastIPandMAC( void );//广播ip和mac事件
void SampleApp_Delete_EP( void );//退网操作
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void SampleApp_Init( uint8 task_id )
{
  
  //红外检测
  Hongwai_Initial();
  
  
  //Switch_Initial();//初始化P1_7引
  //SampleApp_Switch_State(0);//初始为关闭状态

  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
 /***********串口初始化************/
  MT_UartInit();//初始化
  MT_UartRegisterTaskID(task_id);//登记任务号
  
 #if defined ( BUILD_ALL_DEVICES )
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  Point_To_Point_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  Point_To_Point_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  Point_To_Point_DstAddr.addr.shortAddr = 0xFFFF;

  //  点对点通讯定义
  Sample_Coordinator_DstAddr.addrMode = (afAddrMode_t)Addr16Bit; //点播
  Sample_Coordinator_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  Sample_Coordinator_DstAddr.addr.shortAddr = 0x0000;//发给协调器  

  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
}

/*********************************************************************
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{  
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter
  
  if ( events & SYS_EVENT_MSG ){
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt ) {
      switch ( MSGpkt->hdr.event ) {
        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:  
          SampleApp_MessageMSGCB( MSGpkt );  
          break;
        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE: 
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ((SampleApp_NwkState == DEV_ROUTER) || (SampleApp_NwkState == DEV_END_DEVICE)){
            //第一次入网，发送自身的IP和MAC地址
            SampleApp_BroadcastIPandMAC();                  
            
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          break;
        default:
          break;          
      }
      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT ){

    
    if(delete_ep){
      //send "/D|mac|DEL,0|*"
      SampleApp_Delete_EP();
      delete_ep = false;
    } else{
      SampleApp_SendPeriodicMessage_light_intensity();
      //SampleApp_SendPeriodicMessage_wendu();//发送数据函数
      //SampleApp_SendPeriodicMessage_switch();//switch
      //SampleApp_SendPeriodicMessage_Hongwai();//红外检测
      
    }
    // Setup to send message again in normal period (+ a little jitter)   
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,(SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );
    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  } 
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn          SampleApp_MessageMSGCB
 * @brief       logical operation when recieve data from coordinator.
 * @param       pkt - msg packet. 
 * @return      null.
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint8 tempp[17];
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_COM_CLUSTERID:
      for(int i=0;i<pkt->cmd.DataLength;i++){        
          tempp[i] = pkt->cmd.Data[i];        
      }
      //if delete network info "/C|929E|DEL,1|*"
      if(tempp[8]=='D' ||tempp[9]=='E'||tempp[10]=='L'||tempp[11]==','||tempp[12]=='1' ){
        delete_ep = true;
      }
        
     //下行控制
     //SampleApp_Switch_control(pkt->cmd.Data);

      if(tempp[15]=='1'){
        SampleApp_Switch_State(1);
      }else if(tempp[15]=='0' ){
        SampleApp_Switch_State(0);
      }
     
     break;
    
    default :
     break;
    
  }
}

/*********************************************************************
 * @fn          SampleApp_BroadcastIPandMAC
 * @brief       Broadcast property, IP and MAC address to coordinator .
 * @param       null. 
 * @return      null.
 */
void SampleApp_BroadcastIPandMAC( void )
{  
  uint8  Buf[22];
  uint8 *TmpBuf = Buf+1;
  uint16 nwkAddr;
  uint8 *IEEEAddr;
  /* 入网成功后，保存本地识别数据 */
  int value = 1;
  int value1 = 0;
  uint16 TEST_NV=0x0222;
  osal_nv_item_init(TEST_NV,1,NULL);//NULL表示初始化的时候，item数据部分为空
  osal_nv_write(TEST_NV,0,1,&value);
  /********************************/
  osal_nv_read(TEST_NV,0,1,&value1);
 
  //获取段地址和长地址
  nwkAddr = NLME_GetShortAddr();
  IEEEAddr = NLME_GetExtAddr();
  TmpBuf[0] = LO_UINT16( nwkAddr );
  TmpBuf[1] = HI_UINT16( nwkAddr );
  osal_cpyExtAddr( &TmpBuf[2],IEEEAddr);//status,
  TmpBuf[10] = 'L';
  TmpBuf[11] = ',';
  //发送
  if ( AF_DataRequest( &Sample_Coordinator_DstAddr,
                      &SampleApp_epDesc,
                      Device_annce,
                      12,
                      TmpBuf,
                      &SampleApp_TransID,
                      AF_DISCV_ROUTE,
                      AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {}
} 
/*********************************************************************
 * @fn          SampleApp_Delete_EP
 * @brief       Broadcast IP and MAC address to coordinator .
 * @param       null. 
 * @return      null.
 */
void SampleApp_Delete_EP( void )
{  
  uint8  Buf[19];
  uint8 *TmpBuf = Buf+1;
  uint8 *IEEEAddr;
  /* 退网操作 */
  
  /*============*/
 
  IEEEAddr = NLME_GetExtAddr();
  TmpBuf[0] = '/';
  TmpBuf[1] = 'D';
  TmpBuf[2] = '|';
  osal_cpyExtAddr( &TmpBuf[3],IEEEAddr);
  TmpBuf[11] = '|';
  TmpBuf[12] = 'D';
  TmpBuf[13] = 'E';
  TmpBuf[14] = 'L';
  TmpBuf[15] = ',';
  TmpBuf[16] = '0';
  TmpBuf[17] = '|';
  TmpBuf[18] = '*';
  //发送
  if ( AF_DataRequest( &Sample_Coordinator_DstAddr,
                      &SampleApp_epDesc,
                      SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                      19,
                      TmpBuf,
                      &SampleApp_TransID,
                      AF_DISCV_ROUTE,
                      AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {}
  
  delay(1000);
  
  //delete flash
   uint8 pg;
   for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
   {
     HalFlashErase(pg);
   }
   
   //initial all settings.
   ZDORemoveDevice(0);
   
    /*退网成功后，保存本地识别数据 */
   int value = 0;
   uint16 TEST_NV=0x0222;
   osal_nv_item_init(TEST_NV,1,NULL);//NULL表示初始化的时候，item数据部分为空
   osal_nv_write(TEST_NV,0,1,&value);
   
   
}

void delay( int ms ){ 
  while( ms-- ){
    for(int i=100; i!=0; --i ); 
  }
}
