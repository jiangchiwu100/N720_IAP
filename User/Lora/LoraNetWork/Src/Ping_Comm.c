#include "Driverlib.h"



SCH_TCB PingSlotCommTaskTcb;
static uint16_t SlotTimeTick = 0;//时隙滴答


//时隙滴答初始化 
void Ping_Slot_Tick_Init(uint16_t dat)
{
  SlotTimeTick = dat;
}

//初始化时隙编号
void Ping_Slot_Num_Init(uint16_t dat)
{
  LoraNetParams.Str_LoraNetworkParams.SlotNetNum  = dat * 8 ;
  LoraNetParams.Str_LoraNetworkParams.SlotNetNum += 4;//根据时隙号 计算时隙唤醒点
}

//时隙时间触发
bool Ping_Slot_Trigger(void)
{
  if( ++SlotTimeTick >= 5760 ) //累加时隙滴答 360秒 
  {
    SlotTimeTick = 0;
  }
  
  if( LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus ) //如果工作在时隙网络状态
  { 
    if((SlotTimeTick == LoraNetParams.Str_LoraNetworkParams.SlotNetNum) || (SlotTimeTick == 5632 )) //是否到达时隙点
    {
      SCHTaskResume(PingSlotCommTaskTcb);
      W_State(&LoraState,CadTaskIng,true);
      return(true);
    }
    else
    {
      return(false);
    }
  }
  else
  {
    return(false);
  }
}

 
/*
  新达北京数据采集程序框架
*/
void Ping_Slot_Comm_Task(void)// 无线侦听功能
{
  static uint16_t PingTaskDelay = 0;
  
  SCHTaskBegin();
  
  while (1)
  {
    if( LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus )//cad只有时隙状态下可以启动
    {
      PingTaskDelay = Ping_Slot_Comm();
      if( PingTaskDelay == Cad_Suspend )
      {   
        Mbus_Vcc_Ds() ;         // 关闭串口 关闭24V  关闭接收电路
        LM358_Vcc_Ds();
        SCHTaskResume(SleepTaskTcb);
        W_State(&SysState,SleepDisable,false);
        Sx1278_LoRa_Set_SleepMode();                                            //无线进入休眠
        W_State(&LoraState,CadTaskIng,false);
        SCHCurTaskPend();
      }
      else
      {
        SCHCurTaskDly( PingTaskDelay / SCH_HW_TIM_MS );      //2秒钟一次
      }
    }
    else
    {
      SCHCurTaskPend();//任务挂起
    }
  }
  
  SCHTaskEnd();
}
/*
标注：点抄唤醒时，集中器唤醒节点，不需要节点应答。因为只有一次机会，应答与否没有
区别，反而增加节点功耗。
新达北京数据采集程序框架
*/
uint16_t Ping_Slot_Comm(void)
{
  static   ENUM_PingComm_Status PingCommState = Ping_Enable ;
  static   uint8_t CadLoopNum=0;
  static   Union_Lora_Protocol_MSG_Q msg;
  uint16_t  delay=0;
  
  switch(PingCommState)
  {
  case Ping_Enable:     //启动cad 
    { 
      for(uint8_t num=0;num<69;num++)
      {
        msg.dat[num] = 0;
      }
      if( R_State(&LoraState,CadTaskIng) == true )
      {
  #ifdef Debug 
        Test(); //在ping信道继续接收ping数据帧
  #endif
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PingLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]);
        W_State(&LoraState,CadDetected,false) ; //清除标志     
        W_State(&LoraState,CadDone,false) ;     //清除标志          
        Sx1278_LoRa_Set_Cad_Init() ;            //启动CAD
        delay           = 250;    //预留了足够的时间
        PingCommState   = Ping_Deteced;
      }
      else
      {
        delay           = Task_Suspend;         //挂起任务 
        PingCommState   = Ping_Enable;          
      } 
    }
    break;
  case Ping_Deteced:    //检测是否收到前导码
    {
      if(R_State(&LoraState,CadDone) && R_State(&LoraState,CadDetected)) //检测到前导码
      { 
  #ifdef Debug 
        Test(); //在ping信道继续接收ping数据帧
  #endif
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PingLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]); 
        Enable_Lora_Rx();               //如果侦测到前导码 就在CAD信道打开接收
        delay           = 5000;         //开一个接受周期
        PingCommState   = Ping_Rx_Ping; //状态机跳入到接收ping数据帧
      }
      else
      { 
        if(++CadLoopNum >= 3)//CAD增加到3次 提高可靠性
        {
          CadLoopNum = 0;                                     //清除标志  
          delay         = Task_Suspend;                                          //挂起任务 
          PingCommState = Ping_Enable;
        }
        else
        { 
          delay         = 0;    
          W_State(&LoraState,CadTaskIng,false); //挂起任务 
          PingCommState = Ping_Enable;
        }
      }
    }
    break;
  case Ping_Rx_Ping:    //查看是否收到ping数据帧
    {  
      if(R_State(&LoraState,RfRcv_OK) == true) //接收到数据帧
      {     
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg);    //解析lora组网数据帧
        
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true) //数据帧有效
        {                                                               
          if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_PingType )     //数据帧类型为ping点抄唤醒节点
          {                                                             
            if(msg.Str_Lora_Protocol_MSG_Q.FatherID == LoraNetParams.Str_LoraNetworkParams.FatherID ) //父节点发送的beacon
            {
              Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]); 
              Enable_Lora_Rx();                                          
              delay             = 5000;                                    //延时3秒
              PingCommState     = Ping_Rx_Data;                            // 开始接收数据帧
            }
            else                                                                                     //父节点不正确
            {
              delay             = Task_Suspend;                   
              PingCommState     = Ping_Enable; 
            }
          }
          else if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_BroadcastTime)//帧类型为广播下发时隙
          { //每小时一次广播时间
            LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum++; //每次
            delay               = Task_Suspend; //挂起任务 
            PingCommState       = Ping_Enable;
          }
          else if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_BroadcastSleepRtc)//帧类型为广播下发休眠时期
          {
            delay               = Task_Suspend; //挂起任务 
            PingCommState       = Ping_Enable;
          }
          else                                                                            //其他数据帧类型 
          {
            delay               = Task_Suspend;                        
            PingCommState       = Ping_Enable; 
          }
        }
        else                                             //接收数据无效
        {
          delay                 = Task_Suspend;                    
          PingCommState         = Ping_Enable; 
        }
      }
      else                                     //未接收到数据帧
      {
        delay                   = Task_Suspend;                         //挂起任务 
        PingCommState           = Ping_Enable;
      }
    }
    break;
  case Ping_Rx_Data:    
    { 
      if(R_State(&LoraState,RfRcv_OK) == true)  //接收到数据帧
      {     
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg); //解析lora组网数据帧 
        
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true) //数据有效性
        {
          if(msg.Str_Lora_Protocol_MSG_Q.FatherID == LoraNetParams.Str_LoraNetworkParams.FatherID)//是不是对应的父节点
          { 
            Uart_Tx_Init();
            Combine_XDKJ_188(&msg.Str_Lora_Protocol_MSG_Q.Buf[0],msg.Str_Lora_Protocol_MSG_Q.BufLen,&UartTx); //通过645部分 组合新达科技下发数据帧 //填充到串口下发数据缓存里
            Mbus_Vcc_En();                      //打开24V电源
            LM358_Vcc_En();                     //打开接收电源
            if(UartTx.rssi == 0x01)
            {
              delay           = 1500;           //热表延时1.5 s
            }
            else if(UartTx.rssi == 0x02)
            {
              delay           = 8000;           //阀门延时8 s
            }
            PingCommState     = Ping_MBUS_Tx;                         
          }
          else                                                                                  //父节点错误
          {
            delay               = Task_Suspend; //挂起任务 
            PingCommState       = Ping_Enable;
          }
        }
        else                                    //数据无效
        {
          delay                 = Task_Suspend; //挂起任务 
          PingCommState         = Ping_Enable; 
        }
      }
      else                                      //没有接收到数据 超时溢出 
      {
        delay                   = Task_Suspend; //挂起任务 
        PingCommState           = Ping_Enable;
      }
    }
    break; 
  case Ping_MBUS_Tx://完成MBUS预热 启动
    {
      W_State(&SysState,SleepDisable,true);     //
      SCHTaskPend(SleepTaskTcb);                // 停止休眠程序 
      Uart_Rx_Init();
      if(UartTx.rssi == 0x01)
      {
        Sys_Uart_Config();//热表串口初始化
      }
      else if(UartTx.rssi == 0x02)
      {
        Sys_Uart_Valve_Config();//阀门串口初始化
      }
      Uart_Rx_Init();//初始化接收缓存
      Uart_Tx();     //启动发送
      delay = 1000;  //发送+接收 延时
      PingCommState = Ping_MBUS_Rx;
    }
    break;
  case Ping_MBUS_Rx:
    {
      Str_Lora rxtem ;
      
      Mbus_Vcc_Ds();// 关闭串口 关闭24V  关闭接收电路
      LM358_Vcc_Ds();
      SCHTaskResume(SleepTaskTcb);
      W_State(&SysState,SleepDisable,false);
      Lora_Tx_Init();
      Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]);
      Resolve_XDKJ_188(&UartRx.buf[0],UartRx.len,&rxtem);//分析接收数据
      memcpy(&msg.Str_Lora_Protocol_MSG_Q.Buf[0],&rxtem.buf[0],rxtem.len);//解析回传新达科技数据帧 提取有效部分 组织成645 
      msg.Str_Lora_Protocol_MSG_Q.BufLen = rxtem.len;
      Combine_LoRa_Protocol_Frame(&msg);//把645部分填充到lora数据帧中 //发送出去
      Disable_Uart_One();
      Uart_Rx_Init();
      Uart_Tx_Init();
      Lora_Rx_Init();
      Lora_Tx_Init();
      delay                   = 5000; 
      PingCommState           = Ping_Tx_Data; 
    }
    break;
  case Ping_Tx_Data://发送完成之后  要再一次接收 集中器如果没有接收到应答 会进行重试
    {
      if(R_State(&LoraState,RfSend_OK) == true)//正常发送完成
      {
        Enable_Lora_Rx();                       //如果侦测到前导码 就在CAD信道打开接收
        delay                   = 4000;         //再等待一个周期 然后开接收 
        PingCommState           = Ping_Rx_Data; //状态机跳入到接收ping数据帧
      }
      else                                      //发送程序异常
      {
        delay                   = Task_Suspend; //挂起任务 
        PingCommState           = Ping_Enable;
      }
    }
    break;
  default:
    {
      Disable_Uart_One();
      Uart_Rx_Init();
      Uart_Tx_Init();
      Lora_Rx_Init();
      Lora_Tx_Init();
      delay                   = Task_Suspend; //挂起任务 
      PingCommState           = Ping_Enable;
    }
    break;
  }
  return(delay);
}

