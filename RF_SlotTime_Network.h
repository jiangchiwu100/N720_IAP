
#ifndef __RF_SlotTime_Network_H
#define __RF_SlotTime_Network_H

#ifdef __cplusplus
extern "C" 
{
#endif 
  
#define PublicChannel   458375000
  
  
  typedef union 
  {
    uint8_t Cmd;
    struct          
    {
      uint8_t Reserve:     2; //����
      uint8_t FrameType:   3; //0������֡  1���㲥Уʱ֡  2���㳭֡ 3��BOOT֡ 4���쳣�ϱ�֡
      bool JoinNetwork:    1; //1:������0:δ����
      bool ACK:            1; //1:��ACK	0:����ACK
      bool FrameDirection: 1; //1:����	0:����
    }Str_Frame_Cmd;
  }Union_Frame_Cmd;
  
  
  
  
  typedef enum
  {
    ChannelInit         ,//�ŵ���ʼ�� ѡ��˽���ŵ� �ϵ�ʱ��Ҫ�����ŵ��Ƿ�ռ��
    ListenThePublicChannel    ,//�����ŵ��������ŵ���
    CadBroadcast        ,//������CAD�㲥״̬ 
    CadSlot             ,//ʱ϶��CAD״̬
  }ENUM_RF_Tash_State_Machine;
  
typedef enum//ˮ��״̬���� 
{
  RfSendOK                = 0,//���ſ���      1�����ͳɹ�  0��δ���� 
  RfReceiveOK             = 1,//��������״̬  1�����ճɹ�  0��δ���ܳɹ� 
  CadSlotTask             = 2,//ʱ϶����״̬  1�����ڽ���  0��δ����
  CadBroadcastTask        = 3,//ʱ϶�㲥״̬  1�����ڽ���  0��δ����
  InterceptPublicChannelTask  = 4,//�����㲥״̬  1�����ڽ���  0��δ����
} Enum_Radio_Status_Name;
  
/*******************************************************************************
  Polling���� ����״̬��
******************************************************************************/
  typedef enum 
  {  
    InterceptPollingInit,
    InterceptPolling    ,//����ID״̬
    AnswerPollingDone   ,//ID�������
  }ENUM_Polling_Status;
  
/*******************************************************************************
  ʱ϶�㲥���� ����״̬��
******************************************************************************/
  typedef enum 
  {  
    SlotTimeBroadcastPreamble ,
    SlotTimeBroadcastPreambleDone ,//����ID״̬
    SlotTimeBroadcastBeacon,
    SlotTimeBroadcastBeaconDone,
  }ENUM_SlotTimeBroadcast_Status;
  
extern  TaskHandle_t xHandleRFInterceptPublicChannelTask  ;
extern  TaskHandle_t xHandleRFSlotTimeNetWorkTickTask;
extern  TaskHandle_t xHandleRFSlotTimeBroadcastTask     ;


void RF_Slot_Time_Net_Work_Tick_Task( void *pvParameters );
void Reset_Slot_Time_Net_Work_Tick(void);
void Set_Slot_Time_Net_Work_Tick(uint16_t data);
void Reset_Sx1278(void);
void RF_Intercept_Public_Channel_Task( void *pvParameters );
void RF_Slot_Time_Broadcast_Task( void *pvParameters );

  





void Slot_Time_Tick_Task(void *pvParameters);
  
  
#ifdef __cplusplus
}
#endif
#endif