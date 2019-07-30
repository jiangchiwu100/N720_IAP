
#ifndef __LoraNetTask_H
#define __LoraNetTask_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
  
 /*******************************************************************************
  CAD时隙任务 新达北京煤改电阀门热表程序框架 程序状态机
******************************************************************************/
  typedef enum 
  {  
    Enum_Ping_Enable  ,//取出队列中对应时隙 判断是广播还是点抄 取得对应的表号  组织唤醒数据帧 
    Enum_Ping_Tx_PingType ,//等待发送完成信号量 超时5000  然后马上跳入到下一个状态
    Enum_Ping_Rx_PingType ,//在comm信道打开接收  然后跳入到下一个状态
    Enum_Ping_Tx_Data ,//判断接收超时  判断是否为pingtype数据帧 判断是否标号一致 跳入下一个状态 
    Enum_Ping_Tx_Data_End,
    Enum_Ping_Rx_Data ,//根据
  }ENUM_Ping_StateMachine;
 
  
  
  typedef enum 
  {  
    Enum_Polling_Rx_PollingType ,//开启polling接收 
    Enum_Polling_Tx_PollingType ,//接收到polling类型 数据帧 跳转到通信信道 应答pollingtype数据帧
    Enum_Polling_Rx_PollingData ,//接收到polling类型 数据帧 
    Enum_Polling_Tx_PollingData ,
    
  }ENUM_Polling_StateMachine;
 

  
 
  //lora网络参数   信道编号 网络编号 父节点ID PANID

  
void   Lora_Network_Init(void);
void   Lora_SlotTime_Network(void);

extern OS_STK AppTaskLoraPingStk[];  
extern OS_STK AppTaskLoraPollingStk[]; 
extern OS_STK AppTaskLoraBeaconStk[];
extern Str_Lora LoraRx;


uint16_t Get_Slot_Tick(void);
void Set_Slot_Tick(uint16_t dat);
void AppTaskLoraBeacon(void *p_arg);
uint16_t LoraBeaconTask(void);

void AppTaskLoraPing(void *p_arg);
uint16_t LoraPingTask(void);
  
void AppTaskLoraPolling(void *p_arg);
uint16_t LoraPollingTask(void);

void PrintSlotTime(void);

#ifdef __cplusplus
}
#endif
#endif
