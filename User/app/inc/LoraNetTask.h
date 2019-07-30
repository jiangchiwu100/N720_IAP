
#ifndef __LoraNetTask_H
#define __LoraNetTask_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
  
 /*******************************************************************************
  CADʱ϶���� �´ﱱ��ú�ĵ緧���ȱ������ ����״̬��
******************************************************************************/
  typedef enum 
  {  
    Enum_Ping_Enable  ,//ȡ�������ж�Ӧʱ϶ �ж��ǹ㲥���ǵ㳭 ȡ�ö�Ӧ�ı��  ��֯��������֡ 
    Enum_Ping_Tx_PingType ,//�ȴ���������ź��� ��ʱ5000  Ȼ���������뵽��һ��״̬
    Enum_Ping_Rx_PingType ,//��comm�ŵ��򿪽���  Ȼ�����뵽��һ��״̬
    Enum_Ping_Tx_Data ,//�жϽ��ճ�ʱ  �ж��Ƿ�Ϊpingtype����֡ �ж��Ƿ���һ�� ������һ��״̬ 
    Enum_Ping_Tx_Data_End,
    Enum_Ping_Rx_Data ,//����
  }ENUM_Ping_StateMachine;
 
  
  
  typedef enum 
  {  
    Enum_Polling_Rx_PollingType ,//����polling���� 
    Enum_Polling_Tx_PollingType ,//���յ�polling���� ����֡ ��ת��ͨ���ŵ� Ӧ��pollingtype����֡
    Enum_Polling_Rx_PollingData ,//���յ�polling���� ����֡ 
    Enum_Polling_Tx_PollingData ,
    
  }ENUM_Polling_StateMachine;
 

  
 
  //lora�������   �ŵ���� ������ ���ڵ�ID PANID

  
void   Lora_Network_Init(void);
void   Lora_SlotTime_Network(void);

extern OS_STK AppTaskLoraPingStk[];  
extern OS_STK AppTaskLoraPollingStk[]; 
extern OS_STK AppTaskLoraBeaconStk[];
extern Str_Lora LoraRx;


uint16_t Get_Slot_Tick(void);
void Set_Slot_Tick(uint16_t dat);
void AppTaskLoraBeacon(void *p_arg);
uint16_t LoraBeaconTask(void);

void AppTaskLoraPing(void *p_arg);
uint16_t LoraPingTask(void);
  
void AppTaskLoraPolling(void *p_arg);
uint16_t LoraPollingTask(void);

void PrintSlotTime(void);

#ifdef __cplusplus
}
#endif
#endif
