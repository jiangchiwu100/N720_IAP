#ifndef __LoRaProtocol_H
#define __LoRaProtocol_H


#ifdef __cplusplus
extern "C" {
#endif 
  
 /**
  0��Beacon��Ӧ���ڸ�λ�����й㲥��
  1�����ز�ѯ  QueryGateway 
  2����������  AccessGateway  
  3���㲥Уʱ  BroadcastTime
  4��Ping����֡ PingType
  5��ʧ����ѯ  
  6������͸����Ӧ���ڱ������У�
  7������͸����Ӧ�����������У�
  8��Polling�ϱ�����
  **/
  typedef  enum 
  {    
    NetProtocol_Beacon                  = 0,//�������ŵ�beacon 
    NetProtocol_QueryGateway            = 1,//��ѯ����
    NetProtocol_GateWayRegister         = 2,//�������� 
    NetProtocol_QuerySlot               = 3,//ʧ����ѯ
    NetProtocol_BroadcastTime           = 4,//�㲥Уʱ 
    NetProtocol_PingType                = 5,//ping����
    NetProtocol_PingData                = 6,//ping����
    NetProtocol_PollingType             = 7,//polling����
    NetProtocol_PollingData             = 8,//polling���� 
    NetProtocol_Err                     = 0x0f,
  }Enum_CtrlCode_Status;//lora����״̬
 
  //������_����
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
      uint8_t                   Validity        ;//��Ч��
      uint16_t                  SlotTick        ;//ʱ϶�δ�
      uint16_t                  SlotNum         ;//ʱ϶��
      Str_Calendar              Rtc             ;//��ǰʱ�� 
      uint8_t                   SignalStrength  ;
      uint8_t                   DeviceType      ;//�豸����   
      uint8_t                   FrameHead       ;//֡ͷ
      uint8_t                   BufLen          ;//������   
      Union_LoRa_Ctrl           ControlCode     ;//������ 
      uint8_t                   ChannleNum      ;//�ŵ��� 
      uint8_t                   NodeID[4]       ;//�ڵ��ַ
      uint8_t                   Buf[64]         ;//
    }Str_Lora_Protocol_MSG_Q ;                   //polling ������Ϣ����
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
