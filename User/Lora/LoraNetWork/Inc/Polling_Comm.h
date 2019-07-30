#ifndef __Polling_Comm_H
#define __Polling_Comm_H

#ifdef __cplusplus
extern "C" {
#endif 

  //Polling 任务 状态机
  typedef enum 
  {  
    PollingState_TX_Polling     ,//发送polling数据帧
    PollingState_TX_Polling_End ,//发送完polling数据帧
    PollingState_RX_Polling_ACK     ,//接收polling应答数据帧
    PollingState_TX_Data,
    PollingState_TX_Data_End,
    PollingState_RX_Data_ACK,
  }ENUM_Polling_Status;
  
  typedef struct
  { 
    Enum_CtrlCode_Status      CtrlCode        ;//组网帧控制码
    uint8_t                   ChannelNum      ;//信道编号      
    uint8_t                   FatherID        ;//父节点ID 
    uint8_t                   datlen          ;//数据帧长度
    uint8_t                   dat[32]         ;//用于传递数据帧
  }Str_Polling_MSG_DownLink                   ;//polling 下行任务消息队列

  typedef struct
  { 
    uint8_t                   PollingFlag     ;//polling执行情况 是否成功
    Enum_CtrlCode_Status      CtrlCode        ;//组网帧控制码
    uint8_t                   FatherID        ;//父节点ID 
    uint8_t                   Rssi            ;//数据帧信号强度
  }Str_Polling_MSG_UpLink                     ;//polling 上行任务消息队列

  typedef struct 
  {
    uint8_t             Num[Lora_Max_Channels];//
    uint8_t            Rssi[Lora_Max_Channels];
    uint8_t          Father[Lora_Max_Channels]; 
  }Str_Traversal_Channel                      ;//信道遍历结构体
  
  
  
  
  
  extern SCH_TCB PollingCommClassTaskTcb;
  extern SCH_TCB PollingCommTaskTcb;
  
  void Polling_MSG_Q_Init(void);
  void Polling_Comm_Class_Task(void) ;
  void Polling_Comm_Task(void) ;
  uint16_t Polling_Comm(void);
  void Bubble_Sort_Lora(Str_Traversal_Channel *point)  ;
  
  
#ifdef __cplusplus
}
#endif
#endif