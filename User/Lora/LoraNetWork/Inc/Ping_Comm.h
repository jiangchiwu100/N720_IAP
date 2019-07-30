
#ifndef __Ping_Comm_H
#define __Ping_Comm_H

#ifdef __cplusplus
extern "C" 
{
#endif 
  
  
 
#define Cad_Suspend 0xffff
 
  
/*******************************************************************************
  CAD时隙任务 基本程序框架 程序状态机
******************************************************************************/
//  typedef enum 
//  {  
//    Ping_Enable  ,
//    Ping_Deteced ,
//    Ping_Rx_Ping ,
//    Ping_Rx_Data ,
//    Ping_Tx_Data ,
//  }ENUM_PingComm_Status;
  
/*******************************************************************************
  CAD时隙任务 新达北京煤改电阀门热表程序框架 程序状态机
******************************************************************************/
  typedef enum 
  {  
    Ping_Enable  ,
    Ping_Deteced ,
    Ping_Rx_Ping ,
    Ping_Rx_Data ,
    Ping_MBUS_Tx ,
    Ping_MBUS_Rx ,
    Ping_Tx_Data ,
  }ENUM_PingComm_Status;
  
extern SCH_TCB PingSlotCommTaskTcb;

void Ping_Slot_Tick_Init(uint16_t dat);
void Ping_Slot_Num_Init(uint16_t dat);
bool Ping_Slot_Trigger(void);
void Ping_Slot_Comm_Task(void);// 无线侦听功能
uint16_t Ping_Slot_Comm(void);
  
  
 
  
#ifdef __cplusplus
}
#endif
#endif