//侦听无线任务 申请入网
#include "driverlib.h"


SCH_TCB ListenToBeaconTaskTcb;

 


 

/******************************************************************************
为了增加可靠性，改变原有侦听策略，从8秒增加到24秒，相当于连续侦听3次。如果8秒内侦听
到beacon信标帧，则停止侦听。
应当考虑到 万一其他信道所发beacon帧被另一个信道的节点侦听到 会出现意外情况 所以
节点侦听到beacon之后应当判断是否是自己的父节点所发 如果不是的话需要继续侦听
******************************************************************************/
void Listen_To_Beacon_Task(void) 
{ 
  static uint16_t ListenToBeaconTaskDelay = 0;
  
  SCHTaskBegin();
  
  while (1)
  {
    ListenToBeaconTaskDelay = Listen_To_Beacon();
    if( ListenToBeaconTaskDelay == Task_Suspend )//挂起标志
    {
      Sx1278_LoRa_Set_SleepMode();    //无线进入休眠
      SCHCurTaskPend();               //挂起 任务
    }
    else
    {
      SCHCurTaskDly( ListenToBeaconTaskDelay / SCH_HW_TIM_MS );      //2秒钟一次
    }
  }
  
  SCHTaskEnd();
  
}

uint16_t Listen_To_Beacon(void)
{ 
  static ENUM_ListenToBeacon_Status ListenToBeaconTaskState = ListenToBeacon_Enable ;
  static uint8_t LoopNum=0;
  uint16_t delay=0; 
  
  
  switch(ListenToBeaconTaskState)
  {
  case ListenToBeacon_Enable://启动cad 
    { //改频率  调用侦听函数
      Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PingLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]); 
      Enable_Lora_Rx();
      
      delay             = 6000;  //4秒之后进入接收解析，                           
      ListenToBeaconTaskState   = ListenToBeacon_Rx;
    }
    break;
  case ListenToBeacon_Rx://查看是否收到ping数据帧
    { 
      if(LoopNum++ <= 3)
      {
        if(R_State(&LoraState,RfRcv_OK) == true) //接收到数据帧
        { 
          Union_Lora_Protocol_MSG_Q msg;
          
          Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg);//解析lora组网数据帧
          if(msg.Str_Lora_Protocol_MSG_Q.Validity == true)
          {
            if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == 0)//beacon类型
            {
              if(msg.Str_Lora_Protocol_MSG_Q.FatherID == LoraNetParams.Str_LoraNetworkParams.FatherID )//是不是父节点发送的beacon
              {
                LoraNetParams.Str_LoraNetworkParams.NetworkStatus =   LoraNetwork_PingSlotCommStatus ; //程序跳转到时隙状态
                W_Eeprom(LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );//保存当前状态
                LoopNum                 = 0;
                delay                   = Task_Suspend;                              //挂起任务 
                ListenToBeaconTaskState = ListenToBeacon_Enable; 
              }
              else//父节点不正确
              {
                delay                   = 0;                              //挂起任务 
                ListenToBeaconTaskState = ListenToBeacon_Enable; 
              }
            }
            else//帧类型不正确
            {
              delay                     = 0;                              //挂起任务 
              ListenToBeaconTaskState   = ListenToBeacon_Enable; 
            }
          }
          else//接收数据无效
          {
            delay                       = 0;                              //挂起任务 
            ListenToBeaconTaskState     = ListenToBeacon_Enable; 
          }
        }
        else                                     //没有收到数据 超时溢出
        {
          delay                         = 0;                              //挂起任务 
          ListenToBeaconTaskState       = ListenToBeacon_Enable; 
        }
      }
      else  //多次都没有侦听到 就要启动入网流程
      {
        uint32_t tem = 0;
        
        LoopNum = 0;
        
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
        W_Eeprom(LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );//保存当前状态
        
#ifdef Debug                                            
        tem = 1000 / 250;
#else
        tem = rand() % 512 ;//分成512份时间片                                                   
        tem = delay  * 1500 + 21600000;//21600000 = 6小时   3600000 1小时
        tem = tem / 250;
#endif    
        SCHTaskDly(PollingCommClassTaskTcb,tem);
        delay                   = Task_Suspend;                              //挂起任务 
        ListenToBeaconTaskState = ListenToBeacon_Enable;
      }
    }
    break;
  default:
    break;
   }
  return(delay);
}



























