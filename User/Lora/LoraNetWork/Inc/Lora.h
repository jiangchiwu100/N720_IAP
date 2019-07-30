
#ifndef __Lora_H
#define __Lora_H

#ifdef __cplusplus
extern "C" 
{
#endif 


#define Task_Suspend 0xffff
#define Lora_Max_Channels               2//��Ӧ���������ŵ� �������ŵ�Ƶ��
#define UpLink_Frequency_Start          470300000//10����472.1
#define Comm_Frequency_Start            470500000//10����472.1
#define DownLink_Frequency_Start        470700000//10����474.1
  
  
  #pragma pack(1)

  typedef struct 
  {
    uint8_t Num[Lora_Max_Channels];
    uint8_t Rssi[Lora_Max_Channels];
    uint8_t Father[Lora_Max_Channels]; 
  }Str_Lora_Channel;
  

  //lora���� ״̬ һ��4��״̬ 
  typedef  enum 
  {    
    LoraNetwork_SilenceStatus          = 0,//���羲Ĭ״̬ 
    LoraNetwork_ListenToBeaconStatus    = 1,//�������ŵ�beacon 
    LoraNetwork_TraversalChannelStatus  = 2,//������ѯ�����ŵ��ź�ǿ��
    LoraNetwork_GateWayRegisterStatus   = 3,//����ע�� ��������
    LoraNetwork_PingSlotCommStatus      = 4,//LORAʱ϶����״̬ 
    LoraNetwork_SlotTimeQueryStatus     = 5,//ʱ϶ʱ���ѯ  ʧ����ѯ״̬
    LoraNetwork_PollingSeriaNetStatus   = 6,//Polling͸������״̬
  }Enum_NetworkStatus;//lora����״̬


  //lora�������   �ŵ���� ������ ���ڵ�ID PANID
  typedef union
  {
    uint8_t dat[12]              ;
    struct
    { 
      uint8_t                   LoRaSet         ;//lora������Ϣ �������ò���
      Enum_NetworkStatus        NetworkStatus   ;//����״̬ 
      uint8_t                   ChannleNum      ;//�����ŵ���� 
      uint8_t                   FatherID        ;//���ڵ�ID  
      uint8_t                   BroadcastTimeNum;//�㲥Уʱ
      uint8_t                   SOF             ;//����֡��ʼ��
      uint16_t                  SlotNetNum      ;//ʱ϶������
      uint8_t                   SilenceStartRtc[2];//������������
      uint8_t                   SilenceStopRtc[2] ;//���߽�������
    }Str_LoraNetworkParams                      ;//lora�������
  }Union_LoraNetworkParams                      ;
  
  
  typedef struct 
  {
    uint8_t len;
    uint8_t rssi;
    uint8_t buf[64];
  }Str_Lora;
  
  typedef enum 
  {    
    RfSend_OK                   = 0,//�������
    RfRcv_OK                    = 1,//�������
    CadDetected                 = 2,//CAD��⵽CadDetected
    CadDone                     = 3,//CAD���
    ReadRssi                    = 4,//
    CadTaskIng                  = 5,// 
  }Enum_Rf_Status;

  typedef enum 
  {    
    FrameAck                    = 0,//
    FrameFollowUp               = 1,//
    FrameGateWayRegister        = 2,// ��������ɹ�
    FrameTiming                 = 3,//�㲥Уʱ
    FrameGateWayQuery           = 4,//���ز�ѯ֡ 
    FrameBeacon                 = 5,//beacon 
    FramePing                   = 6,//Ping����֡
    FrameSlotTimeQuery          = 7,//�����ɹ� 
    GateWayRegisterFrameDisable = 8,// 
    FrameDirect645              = 9,// ֱ��645��ȡ
    FrameInDirect645            = 11,// ���645��ȡ
    FramePolling                = 12,// ���645��ȡ
  }Enum_Protocol_Status;
  
extern Union_LoraNetworkParams  LoraNetParams;
extern uint32_t LoraState         ;
extern uint32_t ProtocolState  ;
extern Str_Lora LoraRx,LoraTx;
extern uint32_t Lora_Frequency_PollingLink[Lora_Max_Channels];  //polling�ŵ� 
extern uint32_t Lora_Frequency_CommLink[Lora_Max_Channels];    //ͨ���ŵ�
extern uint32_t Lora_Frequency_PingLink[Lora_Max_Channels];     //ʱ϶�����ŵ�
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