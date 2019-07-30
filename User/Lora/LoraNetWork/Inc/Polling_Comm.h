#ifndef __Polling_Comm_H
#define __Polling_Comm_H

#ifdef __cplusplus
extern "C" {
#endif 

  //Polling ���� ״̬��
  typedef enum 
  {  
    PollingState_TX_Polling     ,//����polling����֡
    PollingState_TX_Polling_End ,//������polling����֡
    PollingState_RX_Polling_ACK     ,//����pollingӦ������֡
    PollingState_TX_Data,
    PollingState_TX_Data_End,
    PollingState_RX_Data_ACK,
  }ENUM_Polling_Status;
  
  typedef struct
  { 
    Enum_CtrlCode_Status      CtrlCode        ;//����֡������
    uint8_t                   ChannelNum      ;//�ŵ����      
    uint8_t                   FatherID        ;//���ڵ�ID 
    uint8_t                   datlen          ;//����֡����
    uint8_t                   dat[32]         ;//���ڴ�������֡
  }Str_Polling_MSG_DownLink                   ;//polling ����������Ϣ����

  typedef struct
  { 
    uint8_t                   PollingFlag     ;//pollingִ����� �Ƿ�ɹ�
    Enum_CtrlCode_Status      CtrlCode        ;//����֡������
    uint8_t                   FatherID        ;//���ڵ�ID 
    uint8_t                   Rssi            ;//����֡�ź�ǿ��
  }Str_Polling_MSG_UpLink                     ;//polling ����������Ϣ����

  typedef struct 
  {
    uint8_t             Num[Lora_Max_Channels];//
    uint8_t            Rssi[Lora_Max_Channels];
    uint8_t          Father[Lora_Max_Channels]; 
  }Str_Traversal_Channel                      ;//�ŵ������ṹ��
  
  
  
  
  
  extern SCH_TCB PollingCommClassTaskTcb;
  extern SCH_TCB PollingCommTaskTcb;
  
  void Polling_MSG_Q_Init(void);
  void Polling_Comm_Class_Task(void) ;
  void Polling_Comm_Task(void) ;
  uint16_t Polling_Comm(void);
  void Bubble_Sort_Lora(Str_Traversal_Channel *point)  ;
  
  
#ifdef __cplusplus
}
#endif
#endif