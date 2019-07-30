#include "driverlib.h"


// 入网---》节点侦听下行信道  请求的时候 用上行信道
Union_LoraNetworkParams  LoraNetParams;  
uint32_t LoraState = 0;//标记lora收发状态
Str_Lora LoraRx,LoraTx;  

uint32_t Lora_Frequency_PollingLink[Lora_Max_Channels]={0};  //polling信道 
uint32_t Lora_Frequency_CommLink[Lora_Max_Channels]={0};     //通信信道
uint32_t Lora_Frequency_PingLink[Lora_Max_Channels]={0};     //时隙唤醒信道

void Lora_Rx_Init(void)                                                         
{
  LoraRx.len  = 0 ;
}

void Lora_Tx_Init(void)                                                        
{
  LoraTx.len=0;
}

void Enable_Lora_Rx(void)
{
  Sx1278_LoRa_Set_RxMode(LoRaSettings.Str_LoRaSettings.RxSingleOn); //启动接收
  W_State(&LoraState,RfRcv_OK,false); //清除标志
  Lora_Rx_Init();//清空接收缓存区
}

/*******************************************************************************
** Function name:       Lora_Network_Sys_Init
** Descriptions:        无线网络上电初始化 
**                      1：侦听beacon----则立刻启动任务（正常复位）
**                      2：信道遍历------则延迟六小时启动任务（正常复位）
**                      3：其他状态------则延迟3小时启动任务（异常复位）
** input parameters:    void 
** output parameters:   void
** Returned value:      void 
** Created by:          程卫玺
** Created Date:        2018-5-17   
*******************************************************************************/
void Lora_Network_Init(void)
{
  Str_Calendar   rtctem;
  uint8_t RtcNow[2];
  
  R_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );   //从 eeprom 读取 LoraNetwork 参数值
  /********************初始化信道*******************/
   for(uint8_t num=0;num<Lora_Max_Channels;num++)
  {
    Lora_Frequency_PollingLink[num]     = UpLink_Frequency_Start   + num * 600000; //  470300000 80---无线上行信道 起始编号
    Lora_Frequency_CommLink[num]        = Comm_Frequency_Start     + num * 600000; //  470300000 80---无线上行信道 起始编号
    Lora_Frequency_PingLink[num]        = DownLink_Frequency_Start + num * 600000; //  500300000
  }
  /********************初始化网络*******************/
  LoraNetParams.Str_LoraNetworkParams.SOF               = 0x68;
  LoraNetParams.Str_LoraNetworkParams.LoRaSet           = 1;
  LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum  = 6;//每小时减一次  每次校时加一次       
  /********************初始化网络*******************/
  RTC_Get_Calendar(&rtctem);
  
  RtcNow[0]   = rtctem.Month;
  RtcNow[1]   = rtctem.DayOfMonth;
  if((memcmp(&RtcNow[0],&LoraNetParams.Str_LoraNetworkParams.SilenceStartRtc[0],2)>= 0) && (memcmp(&RtcNow[0],&LoraNetParams.Str_LoraNetworkParams.SilenceStopRtc[0],2)<= 0))
  {
    LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SilenceStatus;
  }
  else
  {
    /*********时隙定时器**********/
    Sys_Lptim_Config();           //配置低功耗定时器 中断62.5ms
  }
  
  /********************检查复位情况***********/
  switch(LoraNetParams.Str_LoraNetworkParams.NetworkStatus)
  {
  case LoraNetwork_SilenceStatus://非抄表期启动 
    {
      LoraNetParams.Str_LoraNetworkParams.ChannleNum = 0;
      LoraNetParams.Str_LoraNetworkParams.FatherID   = 0xaa;
      LoraNetParams.Str_LoraNetworkParams.LoRaSet    = 1;
      LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum = 6;
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SilenceStatus;
    }
    break;
  case LoraNetwork_ListenToBeaconStatus://正常复位 
    {
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_ListenToBeaconStatus;
      SCHTaskResume(ListenToBeaconTaskTcb);//调用侦听beacon任务
    }
    break;
  case LoraNetwork_PingSlotCommStatus://意外复位
    {
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SlotTimeQueryStatus;//跳转到失步查询
      SCHTaskResume(PollingCommClassTaskTcb);
    }
    break;
  case LoraNetwork_TraversalChannelStatus://信道遍历轮询，未入网情况
    {
      uint32_t tem;
      
#ifdef Debug                                            
      tem = 1000 / 250;
#else
      tem = rand() % 512 ;//分成512份时间片                                                   
      tem = tem  * 1500 + 21600000;//21600000 = 6小时   3600000 1小时
      tem = tem  / 250;
#endif  
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
      SCHTaskDly(PollingCommClassTaskTcb,tem);
    }
    break;
  default:
    {
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;//跳转到失步查询
      SCHTaskResume(PollingCommClassTaskTcb);
    }
    break;
  }

  W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams));/****保存无线LORA网络数据****/
}

/*******************************************************************************
** Function name:       Lora_Network_Sys_Init
** Descriptions:        无线网络复位之前进行网络维护
**                      1：时隙状态------则跳入--------》侦听beacon
**                      2：侦听beacon----则跳入--------》信道遍历
**                      3：其他状态------则跳入--------》信道遍历
** input parameters:    void 
** output parameters:   void
** Returned value:      void 
** Created by:          程卫玺
** Created Date:        2018-5-17   
*******************************************************************************/
void Lora_Network_Maintenance_Day(void)
{
  LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum = 0;
  if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus)
  {//如果系统当前为时隙状态 则跳入到侦听状态 然后准备复位                               
    LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_ListenToBeaconStatus;
  }
  else
  {//其他都是没有入网的状态，所以复位之后，根据情况进行入网
    LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
    LoraNetParams.Str_LoraNetworkParams.ChannleNum    = 0;
    LoraNetParams.Str_LoraNetworkParams.FatherID      = 0xAA;//父节点地址为广播地址
  }
  W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );/****保存无线LORA网络数据****/
}

void Lora_Network_Maintenance_Minute(void)
{
  static uint8_t HourCycle=0;
  //只有在时隙状态时 才去判断 是否需要进行失步查询 
  //接收到广播时间次数 与RTC的时（16进制进行转换） 进行比较 当RTC hour 大于 次数3 以上就进行失步查询 
  if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus)//是否在时隙状态
  {
    if(++HourCycle >= 60)
    {
      HourCycle = 0;
      LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum--;
      if(LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum == 0)
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SlotTimeQueryStatus;//进行失步查询
        W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );/****保存无线LORA网络数据****/
        uint32_t tem;
#ifdef Debug                                            
       tem = 1000  ;
#else
       tem = rand() % 64 ;//分成512份时间片                                                   
       tem = tem  * 1500;//21600000 = 6小时   3600000 1小时
       tem = tem   ;
#endif  
        SCHTaskDly(PollingCommClassTaskTcb,tem / SCH_SYS_TICKS_MS);//1秒之后调用失步查询程序
      }
    }
  }
}


