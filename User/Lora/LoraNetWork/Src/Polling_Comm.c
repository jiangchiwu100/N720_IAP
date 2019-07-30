//加入网络任务
#include "driverlib.h"

SCH_TCB PollingCommClassTaskTcb;
SCH_TCB PollingCommTaskTcb;
static Str_Polling_MSG_DownLink  MSG_D  ;
static Str_Polling_MSG_UpLink    MSG_U  ;
static Str_Traversal_Channel     Channel;

//初始化polling任务消息
void Polling_MSG_Q_Init(void)
{
  MSG_D.ChannelNum = 0;
  MSG_D.CtrlCode   = NetProtocol_Err;
  MSG_D.datlen     = 0;
  MSG_D.FatherID   = 0;
  
  MSG_U.CtrlCode   = NetProtocol_Err;
  MSG_U.FatherID   = 0;
  MSG_U.PollingFlag= 0;
  MSG_U.Rssi       = 0;
}

//需要polling的任务 都从这里启动 
//例如时隙查询、网关查询、入网申请、主动polling上报数据
//W_State(&LoraState,ReadRssi,true);    //enable rx rssi
void Polling_Comm_Class_Task(void)     //polling类任务
{                       
  static uint8_t ChannelNum = 0;
    
  SCHTaskBegin();
  
  while (1)
  {
    if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus        == LoraNetwork_SlotTimeQueryStatus)    //失步查询类型 已经入网情况下
    {
      Polling_MSG_Q_Init();//清除消息
     /********************下行传递数据*********************/
      MSG_D.ChannelNum  = LoraNetParams.Str_LoraNetworkParams.ChannleNum;
      MSG_D.CtrlCode    = NetProtocol_QuerySlot;//进行失步查询 查询时隙滴答
      MSG_D.FatherID    = LoraNetParams.Str_LoraNetworkParams.FatherID;
     /************************************************/
      SCHTaskResume(PollingCommTaskTcb);
      SCHTaskQpend();           //任务等待消息
      if(MSG_U.PollingFlag == true)//polling任务成功
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus    = LoraNetwork_PingSlotCommStatus;
        LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum = 6;
        LoraNetParams.Str_LoraNetworkParams.FatherID         = MSG_U.FatherID;
      }
      else                                                   //polling任务失败
      { //失步查询没有成功的时候 启动信道遍历任务 
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
      }
      W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );
    }//遍历信道类型
    else if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus   == LoraNetwork_TraversalChannelStatus)//遍历信道
    {
      for(ChannelNum=0;ChannelNum<Lora_Max_Channels;ChannelNum++ )
      {
        Polling_MSG_Q_Init();//清除消息结构体
        /************************************************/
        Channel.Num[ChannelNum] = ChannelNum; 
        Channel.Father[ChannelNum] = 0;
        Channel.Rssi[ChannelNum]= 0;
        /********************下行传递数据*********************/
        MSG_D.ChannelNum  = ChannelNum;
        MSG_D.CtrlCode    = NetProtocol_QueryGateway;//进行网关查询  
        MSG_D.FatherID    = 0xAA;
        /************************************************/
        SCHTaskResume(PollingCommTaskTcb);
        SCHTaskQpend();           //任务等待消息
        /************************************************/
        if(MSG_U.PollingFlag == true)//polling任务成功
        {
          Channel.Father[ChannelNum] = MSG_U.FatherID;
          Channel.Rssi[ChannelNum]   = MSG_U.Rssi;
        }
        else                                                   //polling任务失败
        {
          Channel.Father[ChannelNum] = 0;
          Channel.Rssi[ChannelNum]   = 0;
        }
      }
      Bubble_Sort_Lora(&Channel);//冒泡排序
      ChannelNum = 0;
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus  = LoraNetwork_GateWayRegisterStatus;
    }//入网申请类型
    else if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus   == LoraNetwork_GateWayRegisterStatus) //网关注册
    { 
      for(ChannelNum=0;ChannelNum<Lora_Max_Channels;ChannelNum++ )
      {
        Polling_MSG_Q_Init();//清除消息结构体
        /********************下行传递数据*********************/
        MSG_D.ChannelNum  = Channel.Num[ChannelNum];
        MSG_D.FatherID    = Channel.Father[ChannelNum];
        MSG_D.CtrlCode    = NetProtocol_GateWayRegister;//进行网关查询  
        /************************************************/
        SCHTaskResume(PollingCommTaskTcb);
        SCHTaskQpend();           //任务等待消息
        /************************************************/
        if(MSG_U.PollingFlag == true)//polling任务成功
        {
          LoraNetParams.Str_LoraNetworkParams.ChannleNum        = Channel.Num[ChannelNum];
          LoraNetParams.Str_LoraNetworkParams.FatherID          = MSG_U.FatherID;
          LoraNetParams.Str_LoraNetworkParams.NetworkStatus     = LoraNetwork_PingSlotCommStatus;
          LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum  = 6;
          ChannelNum = 0xaa;//退出for循环 不用0xFF ++ 之后又变为0
        }
      }
      if(ChannelNum < 0xaa)//没有入网 
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus  = LoraNetwork_TraversalChannelStatus;
      }
       W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );
    }//polling主动上报类型 
    else if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus   == LoraNetwork_PollingSeriaNetStatus) //polling透传
    {
      Polling_MSG_Q_Init();//清除消息
     /********************下行传递数据*********************/
      MSG_D.ChannelNum  = LoraNetParams.Str_LoraNetworkParams.ChannleNum;
      MSG_D.FatherID    = LoraNetParams.Str_LoraNetworkParams.FatherID;
      MSG_D.CtrlCode    = NetProtocol_PollingData;//进行失步查询 查询时隙滴答
     /************************************************/
      SCHTaskResume(PollingCommTaskTcb);
      SCHTaskQpend();           //任务等待消息
      if(MSG_U.PollingFlag == true)//polling任务成功
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_PollingSeriaNetStatus;
      }
      else                                                   //polling任务失败
      {//失步查询没有成功的时候 启动信道遍历任务 
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_PollingSeriaNetStatus;
      }
    }//挂起POLLING任务
    else
    {
      SCHCurTaskPend();
    }
  }
  SCHTaskEnd();
}

void Polling_Comm_Task(void) //polling 任务
{ 
  static uint16_t LoraPollingTaskDelay = 0;
  
  SCHTaskBegin();
  
  while (1)
  {
    LoraPollingTaskDelay =  Polling_Comm();
    if( LoraPollingTaskDelay == Task_Suspend )//挂起标志
    {
      Sx1278_LoRa_Set_SleepMode();    //无线进入休眠
      SCHCurTaskPend();               //挂起 任务
    }
    else
    {
      SCHCurTaskDly( LoraPollingTaskDelay / SCH_HW_TIM_MS );      //2秒钟一次
    }
  }
  
  SCHTaskEnd();
}

uint16_t Polling_Comm(void)
{ 
  static ENUM_Polling_Status PollingState = PollingState_TX_Polling ;
  static uint8_t PollingNum = 0;
  uint16_t delay=0;
  Union_Lora_Protocol_MSG_Q msg;
  uint8_t u8RetStatus = 0; 
  
  switch(PollingState)
  {
  case PollingState_TX_Polling:         //发送polling
    { 
      if(PollingNum++ >= 3)//尝试3次之后还没有发现应答 返回
      {
        PollingNum = 0;
        SCHTaskGetQFree(PollingCommClassTaskTcb, u8RetStatus);//读取队列状态
        if (u8RetStatus == SCH_Q_FREE)      //检查U发送任务队列是否可用
        {      
          MSG_U.PollingFlag = false;//polling任务失败
          SCHTaskQpost(PollingCommClassTaskTcb,&MSG_U ,sizeof(MSG_U));
        }
        delay             = Task_Suspend;                                                 
        PollingState      = PollingState_TX_Polling;
      }
      else
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PollingLink[MSG_D.ChannelNum]); //信道
        msg.Str_Lora_Protocol_MSG_Q.FatherID = MSG_D.FatherID ;//父节点
        msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode = NetProtocol_PollingType;//polling类型数据帧
        Combine_LoRa_Protocol_Frame(&msg);//组合数据帧 并发送出去
        delay             = 5000;                                                 
        PollingState      = PollingState_TX_Polling_End;
      }
    }
    break;
  case PollingState_TX_Polling_End:     //发送polling完成   打开接收 等待应答
    { 
      if(R_State(&LoraState,RfSend_OK) == true)//发送完成
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[MSG_D.ChannelNum]);// 在通信信道接收
        Enable_Lora_Rx();//接收集中器应答数据帧
        delay           = 5000;    
        PollingState    = PollingState_RX_Polling_ACK;
      }
      else                                     //未发送成功 
      {
        delay = rand() % 16 ;                                                   //随机等待时间
        delay = delay * 1500 + 1500;
        PollingState      = PollingState_TX_Polling;
        Sx1278_LoRa_Set_SleepMode();
      }
    }
    break;
  case PollingState_RX_Polling_ACK:     //接收polling应答
    { 
      if(R_State(&LoraState,RfRcv_OK) == true)//接收完成
      {
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg); //解析lora组网数据帧 
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true)//数据帧有效
        {
          if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_PollingType)//polling主动上报类型
          {
            PollingNum          = 0;
            delay               = 0;    
            PollingState        = PollingState_TX_Data;
          }
          else                                                                       //类型错误   
          {
#ifdef Debug
            delay = 3000;
            PollingState      = PollingState_TX_Polling;
#else
            delay = rand() % 16 ; 
            delay = delay * 1500 + 1500;
            PollingState      = PollingState_TX_Polling;
            Sx1278_LoRa_Set_SleepMode();
#endif
          }
        }
        else                                           // 数据帧无效 
        {
#ifdef Debug
            delay = 3000;
            PollingState      = PollingState_TX_Polling;
#else
            delay = rand() % 16 ; 
            delay = delay * 1500 + 1500;
            PollingState      = PollingState_TX_Polling;
            Sx1278_LoRa_Set_SleepMode();
#endif
        }
      }
      else                                     //没有接收到
      {
#ifdef Debug
            delay = 3000;
            PollingState      = PollingState_TX_Polling;
#else
            delay = rand() % 16 ; 
            delay = delay * 1500 + 1500;
            PollingState      = PollingState_TX_Polling;
            Sx1278_LoRa_Set_SleepMode();
#endif
      }
    }
    break;
  case PollingState_TX_Data:            //启动主动上报数据
    {
      if(PollingNum++ >= 3)//尝试3次之后还没有发现应答 返回
      {
        PollingNum = 0;
        SCHTaskGetQFree(PollingCommClassTaskTcb, u8RetStatus);//读取队列状态
        if (u8RetStatus == SCH_Q_FREE)      //检查UART_TX发送任务队列是否可用
        {      
          MSG_U.PollingFlag = false;//polling任务失败
          SCHTaskQpost(PollingCommClassTaskTcb,&MSG_U ,sizeof(MSG_U));
        }
        delay             = Task_Suspend;                                                 
        PollingState      = PollingState_TX_Polling;
      }
      else
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[MSG_D.ChannelNum]); 
        msg.Str_Lora_Protocol_MSG_Q.FatherID = MSG_D.FatherID;
        msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode = MSG_D.CtrlCode;//polling类型数据帧
        Combine_LoRa_Protocol_Frame(&msg);
        delay             = 5000;                                                 
        PollingState      = PollingState_TX_Data_End;
      }
    }
    break;
  case PollingState_TX_Data_End:        //主动上报数据发送完成 打开接收 准备收应答
    {
      if(R_State(&LoraState,RfSend_OK) == true)//发送完成
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[MSG_D.ChannelNum]);//改公共信道
        Enable_Lora_Rx();//接收集中器应答数据帧
        W_State(&LoraState,ReadRssi,true);//读取信号强度
        delay           = 5000;    
        PollingState    = PollingState_RX_Data_ACK;
      }
      else
      {                                                   //随机等待时间
        delay = 0;
        PollingState    = PollingState_TX_Data;
      }
    }
    break;
  case PollingState_RX_Data_ACK:       //接收主动上报应答
    {
      if(R_State(&LoraState,RfRcv_OK) == true)//接收到
      {
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg); //解析lora组网数据帧    
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true)
        {
          //判断回应数据帧是不是和下发数据帧 类型对应的上
          if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == MSG_D.CtrlCode)
          {
            PollingNum = 0;
            SCHTaskGetQFree(PollingCommClassTaskTcb, u8RetStatus);//读取队列状态
            if (u8RetStatus == SCH_Q_FREE)      //检查UART_TX发送任务队列是否可用
            {
              MSG_U.PollingFlag = true;//polling任务成功
              MSG_U.CtrlCode    = msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode;
              MSG_U.FatherID    = msg.Str_Lora_Protocol_MSG_Q.FatherID;
              MSG_U.Rssi        = LoraRx.rssi;
              SCHTaskQpost(PollingCommClassTaskTcb,&MSG_U ,sizeof(MSG_U));
            }
            delay             = Task_Suspend;                                                 
            PollingState      = PollingState_TX_Polling;
          }
          else
          { 
            delay = 0;
            PollingState    = PollingState_TX_Data;
          }
        }
        else//接收的数据帧无效
        {
          delay = 0;
          PollingState    = PollingState_TX_Data;
        }
      }
      else//没有收到应答
      {
        delay = 0;
        PollingState    = PollingState_TX_Data;
      }
    }
    break;
  default:
    {
      delay             = Task_Suspend;                                                 
      PollingState      = PollingState_TX_Polling;
    }
    break;
   }
  return(delay);
}

//冒泡排序 lora各个信道信号强度 Bubble_Sort_Lora(&lora);
void Bubble_Sort_Lora(Str_Traversal_Channel *point)  
{  
  for (uint8_t i=0;i<Lora_Max_Channels-1;i++)  
  {
    for (uint8_t j=0;j<Lora_Max_Channels-i-1;j++)  
    {  
      if (point->Rssi[j]<point->Rssi[j+1]) // 这里是从大到小排序，如果是从小到大排序，只需将“<”换成“>”  
      {  
        uint8_t tem,num,id; 
    
        tem = point->Rssi[j]; 
        num = point->Num[j];
        id  = point->Father[j];
      
        point->Rssi[j]  = point->Rssi[j+1]; 
        point->Num[j]   = point->Num[j+1];
        point->Father[j]= point->Father[j+1];
        
        point->Rssi[j+1]= tem;  
        point->Num[j+1] = num;
        point->Father[j+1] = id;
        
      }  
    }  
  }
} 






















