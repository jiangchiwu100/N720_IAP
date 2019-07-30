//������������ ��������
#include "driverlib.h"


SCH_TCB ListenToBeaconTaskTcb;

 


 

/******************************************************************************
Ϊ�����ӿɿ��ԣ��ı�ԭ���������ԣ���8�����ӵ�24�룬�൱����������3�Ρ����8��������
��beacon�ű�֡����ֹͣ������
Ӧ�����ǵ� ��һ�����ŵ�����beacon֡����һ���ŵ��Ľڵ������� ������������ ����
�ڵ�������beacon֮��Ӧ���ж��Ƿ����Լ��ĸ��ڵ����� ������ǵĻ���Ҫ��������
******************************************************************************/
void Listen_To_Beacon_Task(void) 
{ 
  static uint16_t ListenToBeaconTaskDelay = 0;
  
  SCHTaskBegin();
  
  while (1)
  {
    ListenToBeaconTaskDelay = Listen_To_Beacon();
    if( ListenToBeaconTaskDelay == Task_Suspend )//�����־
    {
      Sx1278_LoRa_Set_SleepMode();    //���߽�������
      SCHCurTaskPend();               //���� ����
    }
    else
    {
      SCHCurTaskDly( ListenToBeaconTaskDelay / SCH_HW_TIM_MS );      //2����һ��
    }
  }
  
  SCHTaskEnd();
  
}

uint16_t Listen_To_Beacon(void)
{ 
  static ENUM_ListenToBeacon_Status ListenToBeaconTaskState = ListenToBeacon_Enable ;
  static uint8_t LoopNum=0;
  uint16_t delay=0; 
  
  
  switch(ListenToBeaconTaskState)
  {
  case ListenToBeacon_Enable://����cad 
    { //��Ƶ��  ������������
      Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PingLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]); 
      Enable_Lora_Rx();
      
      delay             = 6000;  //4��֮�������ս�����                           
      ListenToBeaconTaskState   = ListenToBeacon_Rx;
    }
    break;
  case ListenToBeacon_Rx://�鿴�Ƿ��յ�ping����֡
    { 
      if(LoopNum++ <= 3)
      {
        if(R_State(&LoraState,RfRcv_OK) == true) //���յ�����֡
        { 
          Union_Lora_Protocol_MSG_Q msg;
          
          Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg);//����lora��������֡
          if(msg.Str_Lora_Protocol_MSG_Q.Validity == true)
          {
            if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == 0)//beacon����
            {
              if(msg.Str_Lora_Protocol_MSG_Q.FatherID == LoraNetParams.Str_LoraNetworkParams.FatherID )//�ǲ��Ǹ��ڵ㷢�͵�beacon
              {
                LoraNetParams.Str_LoraNetworkParams.NetworkStatus =   LoraNetwork_PingSlotCommStatus ; //������ת��ʱ϶״̬
                W_Eeprom(LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );//���浱ǰ״̬
                LoopNum                 = 0;
                delay                   = Task_Suspend;                              //�������� 
                ListenToBeaconTaskState = ListenToBeacon_Enable; 
              }
              else//���ڵ㲻��ȷ
              {
                delay                   = 0;                              //�������� 
                ListenToBeaconTaskState = ListenToBeacon_Enable; 
              }
            }
            else//֡���Ͳ���ȷ
            {
              delay                     = 0;                              //�������� 
              ListenToBeaconTaskState   = ListenToBeacon_Enable; 
            }
          }
          else//����������Ч
          {
            delay                       = 0;                              //�������� 
            ListenToBeaconTaskState     = ListenToBeacon_Enable; 
          }
        }
        else                                     //û���յ����� ��ʱ���
        {
          delay                         = 0;                              //�������� 
          ListenToBeaconTaskState       = ListenToBeacon_Enable; 
        }
      }
      else  //��ζ�û�������� ��Ҫ������������
      {
        uint32_t tem = 0;
        
        LoopNum = 0;
        
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
        W_Eeprom(LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );//���浱ǰ״̬
        
#ifdef Debug                                            
        tem = 1000 / 250;
#else
        tem = rand() % 512 ;//�ֳ�512��ʱ��Ƭ                                                   
        tem = delay  * 1500 + 21600000;//21600000 = 6Сʱ   3600000 1Сʱ
        tem = tem / 250;
#endif    
        SCHTaskDly(PollingCommClassTaskTcb,tem);
        delay                   = Task_Suspend;                              //�������� 
        ListenToBeaconTaskState = ListenToBeacon_Enable;
      }
    }
    break;
  default:
    break;
   }
  return(delay);
}



























