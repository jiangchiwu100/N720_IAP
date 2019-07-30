#ifndef __LoRaProtocol_H
#define __LoRaProtocol_H


#ifdef __cplusplus
extern "C" {
#endif 
  
 /**
  0：Beacon（应用于复位后下行广播）
  1：网关查询  QueryGateway 
  2：入网请求  AccessGateway  
  3：广播校时  BroadcastTime
  4：Ping数据帧 PingType
  5：失步查询  
  6：数据透传（应用于被动下行）
  7：数据透传（应用于主动上行）
  8：Polling上报启动
  **/
  typedef  enum 
  {    
    NetProtocol_Beacon                  = 0,//侦听父信道beacon 
    NetProtocol_QueryGateway            = 1,//查询网关
    NetProtocol_GateWayRegister         = 2,//入网请求 
    NetProtocol_QuerySlot               = 3,//失步查询
    NetProtocol_BroadcastTime           = 4,//广播校时 
    NetProtocol_PingType                = 5,//ping类型
    NetProtocol_PingData                = 6,//ping数据
    NetProtocol_PollingType             = 7,//polling类型
    NetProtocol_PollingData             = 8,//polling数据 
    NetProtocol_Err                     = 0x0f,
  }Enum_CtrlCode_Status;//lora网络状态
 
  //控制域_上行
  typedef union 
  {
    uint8_t Byte;
    struct          
    {
      Enum_CtrlCode_Status FunctionCode: 4;    
      bool reserve:  1;
      bool follow_up: 1;
      bool ack: 1;
      bool direction: 1;
    }Str_LoRa_Ctrl;
  }Union_LoRa_Ctrl;

  typedef union
  {
    uint8_t dat[128] ;
    struct
    { 
      uint8_t                   Validity        ;//有效性
      uint16_t                  SlotTick        ;//时隙滴答
      uint16_t                  SlotNum         ;//时隙号
      Str_Calendar              Rtc             ;//当前时间 
      uint8_t                   SignalStrength  ;
      uint8_t                   DeviceType      ;//设备类型   
      uint8_t                   FrameHead       ;//帧头
      uint8_t                   BufLen          ;//长度域   
      Union_LoRa_Ctrl           ControlCode     ;//控制码 
      uint8_t                   ChannleNum      ;//信道号 
      uint8_t                   NodeID[4]       ;//节点地址
      uint8_t                   Buf[64]         ;//
    }Str_Lora_Protocol_MSG_Q ;                   //polling 任务消息队列
  }Union_Lora_Protocol_MSG_Q ;
  
  typedef struct 
  {
    uint8_t len;
    uint8_t PayLoadLen;
    uint8_t buf[128];
  }Str_Protocol;
  
void Resolve_LoRa_Protocol_Frame(uint8_t *spoint,uint8_t len,Union_Lora_Protocol_MSG_Q *msg);

void Combine_LoRa_Protocol_Frame(Str_Lora *tx ,Union_Lora_Protocol_MSG_Q *msg);
  
  
  

  #ifdef __cplusplus
}
#endif
#endif
