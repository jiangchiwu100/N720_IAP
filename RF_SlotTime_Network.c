
#include "includes.h"

static uint32_t SlotTimeNetworkTick=0;
static uint8_t  RfChannelNum = 16    ;
static Str_Com Rf_Rx,Rf_Tx           ;
static uint32_t RadioStatus = 0      ;//无线射频任务状态变量
const  uint32_t Rfchannel[64]={
  458375000,458750000,459125000,459500000,459875000,460250000,460625000,461000000,
  461375000,461750000,462125000,462500000,462875000,463250000,463625000,464000000,
  464375000,464750000,465125000,465500000,465875000,466250000,466625000,467000000, 
  467375000,467750000,468125000,468500000,468875000,469250000,469625000,470000000, 
  470375000,470750000,471125000,471500000,471875000,472250000,472625000,473000000,
  473375000,473750000,474125000,474500000,474875000,475250000,475625000,476000000, 
  478000000,478375000,478750000,479125000,479500000,479875000,480250000,480625000, 
  481000000,481375000,481750000,482125000,482500000,482875000,483250000,483625000, 
};


TaskHandle_t xHandleRFSlotTimeNetWorkTickTask    = NULL;//时隙tick 任务
TaskHandle_t xHandleRFInterceptPublicChannelTask = NULL;//广播侦听
TaskHandle_t xHandleRFSlotTimeBroadcastTask      = NULL;//时隙广播
/*******************************************************************************
** Function name:           Rf_Rx_Buf_Init
** Descriptions:            RF接收缓存初始化
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2015-5-25     
*******************************************************************************/
void Rf_Rx_Buf_Init(void)
{
  Rf_Rx.len=0;
}

/*******************************************************************************
** Function name:           Rf_Tx_Buf_Init
** Descriptions:            RF发射缓存初始化
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2015-5-25     
*******************************************************************************/
void Rf_Tx_Buf_Init(void)
{
  Rf_Tx.len=0;
}

/*******************************************************************************
** Function name:           Rf_Rx_Task
** Descriptions:            接收Rf数据  在rf中断程序中调用
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2015-5-25     
*******************************************************************************/
void Rf_Rx_Buff(uint8_t *buf,uint8_t len,uint16_t rssi)
{
  
  if(len == 0)
    return;
  
  for(Rf_Rx.len=0;Rf_Rx.len<len;Rf_Rx.len++)
  {
    Rf_Rx.buf[Rf_Rx.len] = buf[Rf_Rx.len];
  }
  Rf_Rx.Rssi = rssi;
}

//累加时隙网络时标
void RF_Slot_Time_Net_Work_Tick_Task( void *pvParameters )
{
  
  static TickType_t RfNetWorkTick;
  
  /* 获取当前的系统时间 */
  RfNetWorkTick = xTaskGetTickCount();
  
  while(1)
  {
    if( SlotTimeNetworkTick++ >= 0xffff )
    {
      SlotTimeNetworkTick = 0 ;
    }
    vTaskDelayUntil( &RfNetWorkTick , 125 );//125ms  
  }
}


//清除时隙网络时标
void Reset_Slot_Time_Net_Work_Tick(void)
{
  SlotTimeNetworkTick = 0 ;
}


//设置时隙网络时标
void Set_Slot_Time_Net_Work_Tick(uint16_t data)
{
  SlotTimeNetworkTick = data ; 
}



void Reset_Sx1278(void)
{  
  SX1276Reset();
  if(SpiTest())
  {
    LoRa_Init_TypeDef lorainit = 
    {
      458375000,        //中心频率
      BW125KHZ,         //带宽
      SF11,             //扩频因子
      CR_4_5,           //CRC
      19,                //输出功率
      true,             //CRC开启
      true,             //ExplicitHeaderOn [true:隐式报头模式 ]
    };  
    SX1278_Lora_Init(&lorainit);
    Lora_RxMode();      /* 改为侦听模式 */
  }
  
}

//公共网络侦听入网以及异常处理任务
//上电后运行 当发现有抄读表任务时就跳入到CAD任务中  
//分配时隙空间  计算公式 = （2400 + x * 40）从2400时隙开始  每个表分配40个

void  RF_Intercept_Public_Channel_Task( void *pvParameters )
{
  static uint16_t InterceptTick = 100;
  static uint8_t  PollingStatus = InterceptPollingInit;
  
  while(1)
  {
    
    if((!R_Status(&RadioStatus,CadSlotTask)) && (!R_Status(&RadioStatus,CadBroadcastTask)))
    {  //时隙广播和时隙抄表都没有在进行  则侦听公共信道
      if( Get_Meter_Lib_DataItem_Num() > 0 )//表库中有抄表任务
      { 
        W_Status(&RadioStatus,InterceptPublicChannelTask,false);
        PollingStatus = InterceptPollingInit;//复位侦听任务状态机
        InterceptTick = 1000;            //降低侦听周期频率 把CPU交给其他任务
        //启动时隙抄表任务 
      }
      else                                 //没有抄表任务，则进行侦听信道任务
      {              
        switch(PollingStatus)//侦听任务 状态机程序
        {
        case InterceptPollingInit:
          {
            Reset_Sx1278();
            InterceptTick = 30; /* 更改延时时间 */
            PollingStatus = InterceptPolling;
          }
          break;
        case InterceptPolling://侦听节点上报
          {
            if( R_Status(&RadioStatus,RfReceiveOK) )
            {
              if(Rf_Rx.buf[0] == 0x9a)     //帧头是否正确
              {
                if(Rf_Rx.buf[Rf_Rx.buf[1]+3] == 0x16) //帧尾是否正确
                {
                  if(Rf_Rx.buf[Rf_Rx.buf[1]+2] == Count_CheckSum(&Rf_Rx.buf[2],(Rf_Rx.len-4))) 
                  {
                    B32_B08 nodenum ;
                    Union_Frame_Cmd Rf;
                    
                    Rf.Cmd = Rf_Rx.buf[2] ; 
                    switch(Rf.Str_Frame_Cmd.FrameType)
                    {
                    case 0://入网帧
                      {
                        nodenum.B32 = Get_Meter_Lib_NodeNum(&Rf_Rx.buf[3]);
                        if(nodenum.B32 < MeterLibraryMaxNum)//判断是否在表库内
                        {//在表库内 根据
                          nodenum.B32  = (2400 + nodenum.B32  * 40);//计算时隙时间
                          Rf_Tx.buf[Rf_Tx.len++] = 0x9a;//组织回传数据帧 
                          Rf_Tx.buf[Rf_Tx.len++] = 0x0a;//长度10
                          
                          Rf.Str_Frame_Cmd.Reserve = 0;
                          Rf.Str_Frame_Cmd.FrameType = 0;
                          Rf.Str_Frame_Cmd.JoinNetwork = 1;
                          Rf.Str_Frame_Cmd.ACK  =0;
                          Rf.Str_Frame_Cmd.FrameDirection = 0;
                          
                          Rf_Tx.buf[Rf_Tx.len++] = Rf.Cmd;
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[3];
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[4]; 
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[5];  
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[6];
                          Rf_Tx.buf[Rf_Tx.len++] = RfChannelNum;
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[0];
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[1];
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[2];
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[3];
                          Rf_Tx.buf[Rf_Tx.len++] = Count_CheckSum(&Rf_Tx.buf[2],Rf_Tx.buf[1]);//校验和
                          Rf_Tx.buf[Rf_Tx.len++] = 0x16;//结尾
                          Lora_Send_Data(&Rf_Tx.buf[0] , Rf_Tx.len , 0x08);
                          W_Status(&RadioStatus,RfSendOK,false);
                          PollingStatus = AnswerPollingDone;//转换状态
                          InterceptTick = 1000;//发射延时  等待发送完成 
                        }
                      }
                      break;
                    case 4://异常上报帧
                      {
                        
                      }
                      break;
                    default://其他类型数据帧不处理
                      break;
                    }
                  }
                }
              }
            }
            Rf_Rx_Buf_Init();
          }
          break;
        case AnswerPollingDone://回应完成 改为接收继续
          {
            if(R_Status(&RadioStatus,RfSendOK))//发送成功
            {
              W_Status(&RadioStatus,RfSendOK,false);   //清除标志位
              W_Status(&RadioStatus,RfReceiveOK,false);//清除标志位
              PollingStatus = InterceptPolling ;       //1ms后跳转到侦听
              InterceptTick = 1;                       //1ms后跳转到侦听 
            }
            else //发送不成功 硬件故障
            {
              Reset_Sx1278();//重新复位无线模块
              W_Status(&RadioStatus,RfSendOK,false);   //清除标志位
              W_Status(&RadioStatus,RfReceiveOK,false);//清除标志位
              PollingStatus = InterceptPolling ;       //1ms后跳转到侦听
              InterceptTick = 1;                       //1ms后跳转到侦听      
            }
            Rf_Tx_Buf_Init();
          }
          break;
        default:
          break;
        }
      }
    }
    else//时隙广播或者时隙抄表正在进行  暂时不做侦听任务
    {
      W_Status(&RadioStatus,InterceptPublicChannelTask,false);
      PollingStatus = InterceptPolling;//复位侦听任务状态机
      InterceptTick = 1000;            //降低侦听周期频率 把CPU交给其他任务
    }
    
    vTaskDelay(InterceptTick);    
  }
}


//时隙广播任务  一来保持和节点的心跳 二来占用信道  
void RF_Slot_Time_Broadcast_Task( void *pvParameters )
{
  static uint8_t    SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
  static uint8_t    TxBeaconLoop=0;
  static TickType_t RfBroadcastTimeTick = 1000 * 60 *15;
  /* 获取当前的系统时间 */

  while(1)
  {
    if( !R_Status(&RadioStatus,CadSlotTask) )//如果是时隙任务没有进行则15分钟进行时隙广播
    {
      if( R_Status(&RadioStatus,InterceptPublicChannelTask) )//侦听任务 在进行
      {
        W_Status(&RadioStatus,InterceptPublicChannelTask,false);//停止侦听任务 
      }
      W_Status(&RadioStatus,CadBroadcastTask,true);
      
      switch(SlotTimeBroadcastStatus)//时隙广播状态
      {
      case SlotTimeBroadcastPreamble:
        {
          uint8_t wakeupbuf[]={0xaa,0xaa,0xaa};
          float G_TsXms;
          uint16_t Preamble_Length=0;
          
          SX1276Reset();
          LoRa_Init_TypeDef lorainit = 
          {
            458375000,        //中心频率
            BW125KHZ,         //带宽
            SF11,             //扩频因子
            CR_4_5,           //CRC
            19,                //输出功率
            true,             //CRC开启
            true,             //ExplicitHeaderOn [true:隐式报头模式 ]
          };  
          G_TsXms = SX1278_Lora_Init(&lorainit);
          Preamble_Length = (uint16_t)(60100/G_TsXms+0.5);//唤醒长度5010ms
          Lora_Send_Data(wakeupbuf , 3 , Preamble_Length);
          W_Status(&RadioStatus,RfSendOK,false);
          SlotTimeBroadcastStatus = SlotTimeBroadcastPreambleDone;
          
          RfBroadcastTimeTick = 61000  ; //   61秒
        }
        break;
      case SlotTimeBroadcastPreambleDone:
        {
          if( R_Status(&RadioStatus,RfSendOK))
          {
             W_Status(&RadioStatus,RfSendOK,false);
             SlotTimeBroadcastStatus = SlotTimeBroadcastPreambleDone;
             TxBeaconLoop = 0;//准备开始发广播心跳
             RfBroadcastTimeTick = 1 ; //1分钟+500ms
          }
          else
          {
            TxBeaconLoop = 0;//准备开始发广播心跳     //停止任务  
            SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
            RfBroadcastTimeTick = 1000 * 60 * 15 ; //15分钟
          }
        }
        break;
      case SlotTimeBroadcastBeacon://发送Beacon
        {
          if( TxBeaconLoop++ > 180 )//连续发送180次 
          {
            TxBeaconLoop = 0;
            SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
            RfBroadcastTimeTick = 1000 * 60 * 15 ; //15分钟
          }
          else
          {
            uint8_t buf[5]; //组织时隙Beacon数据帧  发送出去
            Union_Frame_Cmd Rf;
            
            buf[0] = 0x9A;
            buf[1] = 0x05;
            Rf.Str_Frame_Cmd.ACK = false;           //不需要回答
            Rf.Str_Frame_Cmd.FrameDirection = false;//下行
            Rf.Str_Frame_Cmd.FrameType    =  1;//广播校时帧
            Rf.Str_Frame_Cmd.JoinNetwork  =  1;//已经加入网络
            Rf.Str_Frame_Cmd.Reserve    = 0;
            buf[2] = Rf.Cmd;
            buf[3] = 0x00;
            buf[4] = 0x16;
            Lora_Send_Data(buf,0x05,0x08);
            SlotTimeBroadcastStatus = SlotTimeBroadcastBeaconDone;
            W_Status(&RadioStatus,RfSendOK,false);
            RfBroadcastTimeTick = 600 ; //15分钟
          }
        }
        break;
      case SlotTimeBroadcastBeaconDone://发送完成  
        {
          if( R_Status(&RadioStatus,RfSendOK))
          {
            W_Status(&RadioStatus,RfSendOK,false);
            RfBroadcastTimeTick = 1;
            SlotTimeBroadcastStatus = SlotTimeBroadcastBeacon;
          }
          else
          {
            TxBeaconLoop = 0;
            SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
            RfBroadcastTimeTick = 1000 * 60 * 15 ; //15分钟
          }
        }
        break;
      default:
        break;
      }
      
      W_Status(&RadioStatus,CadBroadcastTask,true);//开始时隙广播心跳任务 
    } 
    else                                    //如果是时隙任务在进行中则退出 
    {
      TxBeaconLoop = 0;
      SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
      RfBroadcastTimeTick = 1000 * 60 * 15 ; //15分钟
    }
    vTaskDelay(RfBroadcastTimeTick);
  }
}



//RF_DIO_0
void EXTI2_IRQHandler(void)
{
  uint8_t state;
  
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line2);
    Nop();
    state=Lora_Interrupt_Flag();
    if( state & RFLR_IRQFLAGS_RXDONE )
    {//接收完成中断 
      uint8_t len=0,buf[128]={0},rssi=0;
      len = Lora_Get_Data(buf);
      rssi = Lora_Packet_Rssi();
      Rf_Rx_Buff(buf,len,rssi);
      W_Status(&RadioStatus,RfReceiveOK,true);
    }
    else if( state & RFLR_IRQFLAGS_TXDONE )
    {//发送完成中断
      W_Status(&RadioStatus,RfSendOK,true);
    }
    
    Lora_RxMode();
    
  }
  
}

//RF_DIO_1
void EXTI3_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line3);/* Clear the  EXTI line 0 pending bit */
    
    Nop();
  }
}

//RF_DIO_3
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line0);/* Clear the  EXTI line 0 pending bit */
    
    Nop();
  }
}



