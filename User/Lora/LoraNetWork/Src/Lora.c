#include "driverlib.h"


// ����---���ڵ����������ŵ�  �����ʱ�� �������ŵ�
Union_LoraNetworkParams  LoraNetParams;  
uint32_t LoraState = 0;//���lora�շ�״̬
Str_Lora LoraRx,LoraTx;  

uint32_t Lora_Frequency_PollingLink[Lora_Max_Channels]={0};  //polling�ŵ� 
uint32_t Lora_Frequency_CommLink[Lora_Max_Channels]={0};     //ͨ���ŵ�
uint32_t Lora_Frequency_PingLink[Lora_Max_Channels]={0};     //ʱ϶�����ŵ�

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
  Sx1278_LoRa_Set_RxMode(LoRaSettings.Str_LoRaSettings.RxSingleOn); //��������
  W_State(&LoraState,RfRcv_OK,false); //�����־
  Lora_Rx_Init();//��ս��ջ�����
}

/*******************************************************************************
** Function name:       Lora_Network_Sys_Init
** Descriptions:        ���������ϵ��ʼ�� 
**                      1������beacon----��������������������λ��
**                      2���ŵ�����------���ӳ���Сʱ��������������λ��
**                      3������״̬------���ӳ�3Сʱ���������쳣��λ��
** input parameters:    void 
** output parameters:   void
** Returned value:      void 
** Created by:          ������
** Created Date:        2018-5-17   
*******************************************************************************/
void Lora_Network_Init(void)
{
  Str_Calendar   rtctem;
  uint8_t RtcNow[2];
  
  R_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );   //�� eeprom ��ȡ LoraNetwork ����ֵ
  /********************��ʼ���ŵ�*******************/
   for(uint8_t num=0;num<Lora_Max_Channels;num++)
  {
    Lora_Frequency_PollingLink[num]     = UpLink_Frequency_Start   + num * 600000; //  470300000 80---���������ŵ� ��ʼ���
    Lora_Frequency_CommLink[num]        = Comm_Frequency_Start     + num * 600000; //  470300000 80---���������ŵ� ��ʼ���
    Lora_Frequency_PingLink[num]        = DownLink_Frequency_Start + num * 600000; //  500300000
  }
  /********************��ʼ������*******************/
  LoraNetParams.Str_LoraNetworkParams.SOF               = 0x68;
  LoraNetParams.Str_LoraNetworkParams.LoRaSet           = 1;
  LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum  = 6;//ÿСʱ��һ��  ÿ��Уʱ��һ��       
  /********************��ʼ������*******************/
  RTC_Get_Calendar(&rtctem);
  
  RtcNow[0]   = rtctem.Month;
  RtcNow[1]   = rtctem.DayOfMonth;
  if((memcmp(&RtcNow[0],&LoraNetParams.Str_LoraNetworkParams.SilenceStartRtc[0],2)>= 0) && (memcmp(&RtcNow[0],&LoraNetParams.Str_LoraNetworkParams.SilenceStopRtc[0],2)<= 0))
  {
    LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SilenceStatus;
  }
  else
  {
    /*********ʱ϶��ʱ��**********/
    Sys_Lptim_Config();           //���õ͹��Ķ�ʱ�� �ж�62.5ms
  }
  
  /********************��鸴λ���***********/
  switch(LoraNetParams.Str_LoraNetworkParams.NetworkStatus)
  {
  case LoraNetwork_SilenceStatus://�ǳ��������� 
    {
      LoraNetParams.Str_LoraNetworkParams.ChannleNum = 0;
      LoraNetParams.Str_LoraNetworkParams.FatherID   = 0xaa;
      LoraNetParams.Str_LoraNetworkParams.LoRaSet    = 1;
      LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum = 6;
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SilenceStatus;
    }
    break;
  case LoraNetwork_ListenToBeaconStatus://������λ 
    {
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_ListenToBeaconStatus;
      SCHTaskResume(ListenToBeaconTaskTcb);//��������beacon����
    }
    break;
  case LoraNetwork_PingSlotCommStatus://���⸴λ
    {
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SlotTimeQueryStatus;//��ת��ʧ����ѯ
      SCHTaskResume(PollingCommClassTaskTcb);
    }
    break;
  case LoraNetwork_TraversalChannelStatus://�ŵ�������ѯ��δ�������
    {
      uint32_t tem;
      
#ifdef Debug                                            
      tem = 1000 / 250;
#else
      tem = rand() % 512 ;//�ֳ�512��ʱ��Ƭ                                                   
      tem = tem  * 1500 + 21600000;//21600000 = 6Сʱ   3600000 1Сʱ
      tem = tem  / 250;
#endif  
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
      SCHTaskDly(PollingCommClassTaskTcb,tem);
    }
    break;
  default:
    {
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;//��ת��ʧ����ѯ
      SCHTaskResume(PollingCommClassTaskTcb);
    }
    break;
  }

  W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams));/****��������LORA��������****/
}

/*******************************************************************************
** Function name:       Lora_Network_Sys_Init
** Descriptions:        �������縴λ֮ǰ��������ά��
**                      1��ʱ϶״̬------������--------������beacon
**                      2������beacon----������--------���ŵ�����
**                      3������״̬------������--------���ŵ�����
** input parameters:    void 
** output parameters:   void
** Returned value:      void 
** Created by:          ������
** Created Date:        2018-5-17   
*******************************************************************************/
void Lora_Network_Maintenance_Day(void)
{
  LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum = 0;
  if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus)
  {//���ϵͳ��ǰΪʱ϶״̬ �����뵽����״̬ Ȼ��׼����λ                               
    LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_ListenToBeaconStatus;
  }
  else
  {//��������û��������״̬�����Ը�λ֮�󣬸��������������
    LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
    LoraNetParams.Str_LoraNetworkParams.ChannleNum    = 0;
    LoraNetParams.Str_LoraNetworkParams.FatherID      = 0xAA;//���ڵ��ַΪ�㲥��ַ
  }
  W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );/****��������LORA��������****/
}

void Lora_Network_Maintenance_Minute(void)
{
  static uint8_t HourCycle=0;
  //ֻ����ʱ϶״̬ʱ ��ȥ�ж� �Ƿ���Ҫ����ʧ����ѯ 
  //���յ��㲥ʱ����� ��RTC��ʱ��16���ƽ���ת���� ���бȽ� ��RTC hour ���� ����3 ���Ͼͽ���ʧ����ѯ 
  if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus)//�Ƿ���ʱ϶״̬
  {
    if(++HourCycle >= 60)
    {
      HourCycle = 0;
      LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum--;
      if(LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum == 0)
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_SlotTimeQueryStatus;//����ʧ����ѯ
        W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );/****��������LORA��������****/
        uint32_t tem;
#ifdef Debug                                            
       tem = 1000  ;
#else
       tem = rand() % 64 ;//�ֳ�512��ʱ��Ƭ                                                   
       tem = tem  * 1500;//21600000 = 6Сʱ   3600000 1Сʱ
       tem = tem   ;
#endif  
        SCHTaskDly(PollingCommClassTaskTcb,tem / SCH_SYS_TICKS_MS);//1��֮�����ʧ����ѯ����
      }
    }
  }
}


