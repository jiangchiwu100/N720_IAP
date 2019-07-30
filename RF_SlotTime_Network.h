
#ifndef __RF_SlotTime_Network_H
#define __RF_SlotTime_Network_H

#ifdef __cplusplus
extern "C" 
{
#endif 
  
#define PublicChannel   458375000
  
  
  typedef union 
  {
    uint8_t Cmd;
    struct          
    {
      uint8_t Reserve:     2; //备用
      uint8_t FrameType:   3; //0：入网帧  1：广播校时帧  2：点抄帧 3：BOOT帧 4：异常上报帧
      bool JoinNetwork:    1; //1:已入网0:未入网
      bool ACK:            1; //1:需ACK	0:不需ACK
      bool FrameDirection: 1; //1:上行	0:下行
    }Str_Frame_Cmd;
  }Union_Frame_Cmd;
  
  
  
  
  typedef enum
  {
    ChannelInit         ,//信道初始化 选择私有信道 上电时需要测试信道是否占用
    ListenThePublicChannel    ,//侦听信道（公共信道）
    CadBroadcast        ,//周期性CAD广播状态 
    CadSlot             ,//时隙内CAD状态
  }ENUM_RF_Tash_State_Machine;
  
typedef enum//水表状态变量 
{
  RfSendOK                = 0,//阀门开关      1：发送成功  0：未发送 
  RfReceiveOK             = 1,//阀门锁定状态  1：接收成功  0：未接受成功 
  CadSlotTask             = 2,//时隙抄表状态  1：正在进行  0：未进行
  CadBroadcastTask        = 3,//时隙广播状态  1：正在进行  0：未进行
  InterceptPublicChannelTask  = 4,//侦听广播状态  1：正在进行  0：未进行
} Enum_Radio_Status_Name;
  
/*******************************************************************************
  Polling任务 程序状态机
******************************************************************************/
  typedef enum 
  {  
    InterceptPollingInit,
    InterceptPolling    ,//发送ID状态
    AnswerPollingDone   ,//ID发送完成
  }ENUM_Polling_Status;
  
/*******************************************************************************
  时隙广播任务 程序状态机
******************************************************************************/
  typedef enum 
  {  
    SlotTimeBroadcastPreamble ,
    SlotTimeBroadcastPreambleDone ,//发送ID状态
    SlotTimeBroadcastBeacon,
    SlotTimeBroadcastBeaconDone,
  }ENUM_SlotTimeBroadcast_Status;
  
extern  TaskHandle_t xHandleRFInterceptPublicChannelTask  ;
extern  TaskHandle_t xHandleRFSlotTimeNetWorkTickTask;
extern  TaskHandle_t xHandleRFSlotTimeBroadcastTask     ;


void RF_Slot_Time_Net_Work_Tick_Task( void *pvParameters );
void Reset_Slot_Time_Net_Work_Tick(void);
void Set_Slot_Time_Net_Work_Tick(uint16_t data);
void Reset_Sx1278(void);
void RF_Intercept_Public_Channel_Task( void *pvParameters );
void RF_Slot_Time_Broadcast_Task( void *pvParameters );

  





void Slot_Time_Tick_Task(void *pvParameters);
  
  
#ifdef __cplusplus
}
#endif
#endif