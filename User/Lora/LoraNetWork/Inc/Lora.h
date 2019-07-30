
#ifndef __Lora_H
#define __Lora_H

#ifdef __cplusplus
extern "C" 
{
#endif 


#define Task_Suspend 0xffff
#define Lora_Max_Channels               2//对应下面上行信道 和下行信道频率
#define UpLink_Frequency_Start          470300000//10个到472.1
#define Comm_Frequency_Start            470500000//10个到472.1
#define DownLink_Frequency_Start        470700000//10个到474.1
  
  
  #pragma pack(1)

  typedef struct 
  {
    uint8_t Num[Lora_Max_Channels];
    uint8_t Rssi[Lora_Max_Channels];
    uint8_t Father[Lora_Max_Channels]; 
  }Str_Lora_Channel;
  

  //lora网络 状态 一共4总状态 
  typedef  enum 
  {    
    LoraNetwork_SilenceStatus          = 0,//网络静默状态 
    LoraNetwork_ListenToBeaconStatus    = 1,//侦听父信道beacon 
    LoraNetwork_TraversalChannelStatus  = 2,//遍历查询各个信道信号强度
    LoraNetwork_GateWayRegisterStatus   = 3,//网关注册 加入网络
    LoraNetwork_PingSlotCommStatus      = 4,//LORA时隙网络状态 
    LoraNetwork_SlotTimeQueryStatus     = 5,//时隙时间查询  失步查询状态
    LoraNetwork_PollingSeriaNetStatus   = 6,//Polling透传任务状态
  }Enum_NetworkStatus;//lora网络状态


  //lora网络参数   信道编号 网络编号 父节点ID PANID
  typedef union
  {
    uint8_t dat[12]              ;
    struct
    { 
      uint8_t                   LoRaSet         ;//lora配置信息 无线配置参数
      Enum_NetworkStatus        NetworkStatus   ;//网络状态 
      uint8_t                   ChannleNum      ;//入网信道编号 
      uint8_t                   FatherID        ;//父节点ID  
      uint8_t                   BroadcastTimeNum;//广播校时
      uint8_t                   SOF             ;//数据帧起始符
      uint16_t                  SlotNetNum      ;//时隙网络编号
      uint8_t                   SilenceStartRtc[2];//休眠启动日期
      uint8_t                   SilenceStopRtc[2] ;//休眠结束日期
    }Str_LoraNetworkParams                      ;//lora网络参数
  }Union_LoraNetworkParams                      ;
  
  
  typedef struct 
  {
    uint8_t len;
    uint8_t rssi;
    uint8_t buf[64];
  }Str_Lora;
  
  typedef enum 
  {    
    RfSend_OK                   = 0,//发送完成
    RfRcv_OK                    = 1,//接收完成
    CadDetected                 = 2,//CAD检测到CadDetected
    CadDone                     = 3,//CAD完成
    ReadRssi                    = 4,//
    CadTaskIng                  = 5,// 
  }Enum_Rf_Status;

  typedef enum 
  {    
    FrameAck                    = 0,//
    FrameFollowUp               = 1,//
    FrameGateWayRegister        = 2,// 入网请求成功
    FrameTiming                 = 3,//广播校时
    FrameGateWayQuery           = 4,//网关查询帧 
    FrameBeacon                 = 5,//beacon 
    FramePing                   = 6,//Ping数据帧
    FrameSlotTimeQuery          = 7,//侦听成功 
    GateWayRegisterFrameDisable = 8,// 
    FrameDirect645              = 9,// 直接645读取
    FrameInDirect645            = 11,// 间接645读取
    FramePolling                = 12,// 间接645读取
  }Enum_Protocol_Status;
  
extern Union_LoraNetworkParams  LoraNetParams;
extern uint32_t LoraState         ;
extern uint32_t ProtocolState  ;
extern Str_Lora LoraRx,LoraTx;
extern uint32_t Lora_Frequency_PollingLink[Lora_Max_Channels];  //polling信道 
extern uint32_t Lora_Frequency_CommLink[Lora_Max_Channels];    //通信信道
extern uint32_t Lora_Frequency_PingLink[Lora_Max_Channels];     //时隙唤醒信道
extern uint32_t Lora_Keys;
 
void Lora_Rx_Init(void);
void Lora_Tx_Init(void);
void Enable_Lora_Rx(void);
 
void Lora_Network_Maintenance_Minute(void);
void Lora_Network_Maintenance_Day(void);
void Lora_Network_Init(void);

#ifdef __cplusplus
}
#endif
#endif