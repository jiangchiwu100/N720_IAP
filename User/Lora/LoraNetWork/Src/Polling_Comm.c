//������������
#include "driverlib.h"

SCH_TCB PollingCommClassTaskTcb;
SCH_TCB PollingCommTaskTcb;
static Str_Polling_MSG_DownLink  MSG_D  ;
static Str_Polling_MSG_UpLink    MSG_U  ;
static Str_Traversal_Channel     Channel;

//��ʼ��polling������Ϣ
void Polling_MSG_Q_Init(void)
{
  MSG_D.ChannelNum = 0;
  MSG_D.CtrlCode   = NetProtocol_Err;
  MSG_D.datlen     = 0;
  MSG_D.FatherID   = 0;
  
  MSG_U.CtrlCode   = NetProtocol_Err;
  MSG_U.FatherID   = 0;
  MSG_U.PollingFlag= 0;
  MSG_U.Rssi       = 0;
}

//��Ҫpolling������ ������������ 
//����ʱ϶��ѯ�����ز�ѯ���������롢����polling�ϱ�����
//W_State(&LoraState,ReadRssi,true);    //enable rx rssi
void Polling_Comm_Class_Task(void)     //polling������
{                       
  static uint8_t ChannelNum = 0;
    
  SCHTaskBegin();
  
  while (1)
  {
    if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus        == LoraNetwork_SlotTimeQueryStatus)    //ʧ����ѯ���� �Ѿ����������
    {
      Polling_MSG_Q_Init();//�����Ϣ
     /********************���д�������*********************/
      MSG_D.ChannelNum  = LoraNetParams.Str_LoraNetworkParams.ChannleNum;
      MSG_D.CtrlCode    = NetProtocol_QuerySlot;//����ʧ����ѯ ��ѯʱ϶�δ�
      MSG_D.FatherID    = LoraNetParams.Str_LoraNetworkParams.FatherID;
     /************************************************/
      SCHTaskResume(PollingCommTaskTcb);
      SCHTaskQpend();           //����ȴ���Ϣ
      if(MSG_U.PollingFlag == true)//polling����ɹ�
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus    = LoraNetwork_PingSlotCommStatus;
        LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum = 6;
        LoraNetParams.Str_LoraNetworkParams.FatherID         = MSG_U.FatherID;
      }
      else                                                   //polling����ʧ��
      { //ʧ����ѯû�гɹ���ʱ�� �����ŵ��������� 
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_TraversalChannelStatus;
      }
      W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );
    }//�����ŵ�����
    else if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus   == LoraNetwork_TraversalChannelStatus)//�����ŵ�
    {
      for(ChannelNum=0;ChannelNum<Lora_Max_Channels;ChannelNum++ )
      {
        Polling_MSG_Q_Init();//�����Ϣ�ṹ��
        /************************************************/
        Channel.Num[ChannelNum] = ChannelNum; 
        Channel.Father[ChannelNum] = 0;
        Channel.Rssi[ChannelNum]= 0;
        /********************���д�������*********************/
        MSG_D.ChannelNum  = ChannelNum;
        MSG_D.CtrlCode    = NetProtocol_QueryGateway;//�������ز�ѯ  
        MSG_D.FatherID    = 0xAA;
        /************************************************/
        SCHTaskResume(PollingCommTaskTcb);
        SCHTaskQpend();           //����ȴ���Ϣ
        /************************************************/
        if(MSG_U.PollingFlag == true)//polling����ɹ�
        {
          Channel.Father[ChannelNum] = MSG_U.FatherID;
          Channel.Rssi[ChannelNum]   = MSG_U.Rssi;
        }
        else                                                   //polling����ʧ��
        {
          Channel.Father[ChannelNum] = 0;
          Channel.Rssi[ChannelNum]   = 0;
        }
      }
      Bubble_Sort_Lora(&Channel);//ð������
      ChannelNum = 0;
      LoraNetParams.Str_LoraNetworkParams.NetworkStatus  = LoraNetwork_GateWayRegisterStatus;
    }//������������
    else if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus   == LoraNetwork_GateWayRegisterStatus) //����ע��
    { 
      for(ChannelNum=0;ChannelNum<Lora_Max_Channels;ChannelNum++ )
      {
        Polling_MSG_Q_Init();//�����Ϣ�ṹ��
        /********************���д�������*********************/
        MSG_D.ChannelNum  = Channel.Num[ChannelNum];
        MSG_D.FatherID    = Channel.Father[ChannelNum];
        MSG_D.CtrlCode    = NetProtocol_GateWayRegister;//�������ز�ѯ  
        /************************************************/
        SCHTaskResume(PollingCommTaskTcb);
        SCHTaskQpend();           //����ȴ���Ϣ
        /************************************************/
        if(MSG_U.PollingFlag == true)//polling����ɹ�
        {
          LoraNetParams.Str_LoraNetworkParams.ChannleNum        = Channel.Num[ChannelNum];
          LoraNetParams.Str_LoraNetworkParams.FatherID          = MSG_U.FatherID;
          LoraNetParams.Str_LoraNetworkParams.NetworkStatus     = LoraNetwork_PingSlotCommStatus;
          LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum  = 6;
          ChannelNum = 0xaa;//�˳�forѭ�� ����0xFF ++ ֮���ֱ�Ϊ0
        }
      }
      if(ChannelNum < 0xaa)//û������ 
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus  = LoraNetwork_TraversalChannelStatus;
      }
       W_Eeprom( LoraNetParamsAddr,&LoraNetParams.dat[0] ,sizeof(LoraNetParams) );
    }//polling�����ϱ����� 
    else if(LoraNetParams.Str_LoraNetworkParams.NetworkStatus   == LoraNetwork_PollingSeriaNetStatus) //polling͸��
    {
      Polling_MSG_Q_Init();//�����Ϣ
     /********************���д�������*********************/
      MSG_D.ChannelNum  = LoraNetParams.Str_LoraNetworkParams.ChannleNum;
      MSG_D.FatherID    = LoraNetParams.Str_LoraNetworkParams.FatherID;
      MSG_D.CtrlCode    = NetProtocol_PollingData;//����ʧ����ѯ ��ѯʱ϶�δ�
     /************************************************/
      SCHTaskResume(PollingCommTaskTcb);
      SCHTaskQpend();           //����ȴ���Ϣ
      if(MSG_U.PollingFlag == true)//polling����ɹ�
      {
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_PollingSeriaNetStatus;
      }
      else                                                   //polling����ʧ��
      {//ʧ����ѯû�гɹ���ʱ�� �����ŵ��������� 
        LoraNetParams.Str_LoraNetworkParams.NetworkStatus = LoraNetwork_PollingSeriaNetStatus;
      }
    }//����POLLING����
    else
    {
      SCHCurTaskPend();
    }
  }
  SCHTaskEnd();
}

void Polling_Comm_Task(void) //polling ����
{ 
  static uint16_t LoraPollingTaskDelay = 0;
  
  SCHTaskBegin();
  
  while (1)
  {
    LoraPollingTaskDelay =  Polling_Comm();
    if( LoraPollingTaskDelay == Task_Suspend )//�����־
    {
      Sx1278_LoRa_Set_SleepMode();    //���߽�������
      SCHCurTaskPend();               //���� ����
    }
    else
    {
      SCHCurTaskDly( LoraPollingTaskDelay / SCH_HW_TIM_MS );      //2����һ��
    }
  }
  
  SCHTaskEnd();
}

uint16_t Polling_Comm(void)
{ 
  static ENUM_Polling_Status PollingState = PollingState_TX_Polling ;
  static uint8_t PollingNum = 0;
  uint16_t delay=0;
  Union_Lora_Protocol_MSG_Q msg;
  uint8_t u8RetStatus = 0; 
  
  switch(PollingState)
  {
  case PollingState_TX_Polling:         //����polling
    { 
      if(PollingNum++ >= 3)//����3��֮��û�з���Ӧ�� ����
      {
        PollingNum = 0;
        SCHTaskGetQFree(PollingCommClassTaskTcb, u8RetStatus);//��ȡ����״̬
        if (u8RetStatus == SCH_Q_FREE)      //���U������������Ƿ����
        {      
          MSG_U.PollingFlag = false;//polling����ʧ��
          SCHTaskQpost(PollingCommClassTaskTcb,&MSG_U ,sizeof(MSG_U));
        }
        delay             = Task_Suspend;                                                 
        PollingState      = PollingState_TX_Polling;
      }
      else
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PollingLink[MSG_D.ChannelNum]); //�ŵ�
        msg.Str_Lora_Protocol_MSG_Q.FatherID = MSG_D.FatherID ;//���ڵ�
        msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode = NetProtocol_PollingType;//polling��������֡
        Combine_LoRa_Protocol_Frame(&msg);//�������֡ �����ͳ�ȥ
        delay             = 5000;                                                 
        PollingState      = PollingState_TX_Polling_End;
      }
    }
    break;
  case PollingState_TX_Polling_End:     //����polling���   �򿪽��� �ȴ�Ӧ��
    { 
      if(R_State(&LoraState,RfSend_OK) == true)//�������
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[MSG_D.ChannelNum]);// ��ͨ���ŵ�����
        Enable_Lora_Rx();//���ռ�����Ӧ������֡
        delay           = 5000;    
        PollingState    = PollingState_RX_Polling_ACK;
      }
      else                                     //δ���ͳɹ� 
      {
        delay = rand() % 16 ;                                                   //����ȴ�ʱ��
        delay = delay * 1500 + 1500;
        PollingState      = PollingState_TX_Polling;
        Sx1278_LoRa_Set_SleepMode();
      }
    }
    break;
  case PollingState_RX_Polling_ACK:     //����pollingӦ��
    { 
      if(R_State(&LoraState,RfRcv_OK) == true)//�������
      {
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg); //����lora��������֡ 
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true)//����֡��Ч
        {
          if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_PollingType)//polling�����ϱ�����
          {
            PollingNum          = 0;
            delay               = 0;    
            PollingState        = PollingState_TX_Data;
          }
          else                                                                       //���ʹ���   
          {
#ifdef Debug
            delay = 3000;
            PollingState      = PollingState_TX_Polling;
#else
            delay = rand() % 16 ; 
            delay = delay * 1500 + 1500;
            PollingState      = PollingState_TX_Polling;
            Sx1278_LoRa_Set_SleepMode();
#endif
          }
        }
        else                                           // ����֡��Ч 
        {
#ifdef Debug
            delay = 3000;
            PollingState      = PollingState_TX_Polling;
#else
            delay = rand() % 16 ; 
            delay = delay * 1500 + 1500;
            PollingState      = PollingState_TX_Polling;
            Sx1278_LoRa_Set_SleepMode();
#endif
        }
      }
      else                                     //û�н��յ�
      {
#ifdef Debug
            delay = 3000;
            PollingState      = PollingState_TX_Polling;
#else
            delay = rand() % 16 ; 
            delay = delay * 1500 + 1500;
            PollingState      = PollingState_TX_Polling;
            Sx1278_LoRa_Set_SleepMode();
#endif
      }
    }
    break;
  case PollingState_TX_Data:            //���������ϱ�����
    {
      if(PollingNum++ >= 3)//����3��֮��û�з���Ӧ�� ����
      {
        PollingNum = 0;
        SCHTaskGetQFree(PollingCommClassTaskTcb, u8RetStatus);//��ȡ����״̬
        if (u8RetStatus == SCH_Q_FREE)      //���UART_TX������������Ƿ����
        {      
          MSG_U.PollingFlag = false;//polling����ʧ��
          SCHTaskQpost(PollingCommClassTaskTcb,&MSG_U ,sizeof(MSG_U));
        }
        delay             = Task_Suspend;                                                 
        PollingState      = PollingState_TX_Polling;
      }
      else
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[MSG_D.ChannelNum]); 
        msg.Str_Lora_Protocol_MSG_Q.FatherID = MSG_D.FatherID;
        msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode = MSG_D.CtrlCode;//polling��������֡
        Combine_LoRa_Protocol_Frame(&msg);
        delay             = 5000;                                                 
        PollingState      = PollingState_TX_Data_End;
      }
    }
    break;
  case PollingState_TX_Data_End:        //�����ϱ����ݷ������ �򿪽��� ׼����Ӧ��
    {
      if(R_State(&LoraState,RfSend_OK) == true)//�������
      {
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[MSG_D.ChannelNum]);//�Ĺ����ŵ�
        Enable_Lora_Rx();//���ռ�����Ӧ������֡
        W_State(&LoraState,ReadRssi,true);//��ȡ�ź�ǿ��
        delay           = 5000;    
        PollingState    = PollingState_RX_Data_ACK;
      }
      else
      {                                                   //����ȴ�ʱ��
        delay = 0;
        PollingState    = PollingState_TX_Data;
      }
    }
    break;
  case PollingState_RX_Data_ACK:       //���������ϱ�Ӧ��
    {
      if(R_State(&LoraState,RfRcv_OK) == true)//���յ�
      {
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg); //����lora��������֡    
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true)
        {
          //�жϻ�Ӧ����֡�ǲ��Ǻ��·�����֡ ���Ͷ�Ӧ����
          if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == MSG_D.CtrlCode)
          {
            PollingNum = 0;
            SCHTaskGetQFree(PollingCommClassTaskTcb, u8RetStatus);//��ȡ����״̬
            if (u8RetStatus == SCH_Q_FREE)      //���UART_TX������������Ƿ����
            {
              MSG_U.PollingFlag = true;//polling����ɹ�
              MSG_U.CtrlCode    = msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode;
              MSG_U.FatherID    = msg.Str_Lora_Protocol_MSG_Q.FatherID;
              MSG_U.Rssi        = LoraRx.rssi;
              SCHTaskQpost(PollingCommClassTaskTcb,&MSG_U ,sizeof(MSG_U));
            }
            delay             = Task_Suspend;                                                 
            PollingState      = PollingState_TX_Polling;
          }
          else
          { 
            delay = 0;
            PollingState    = PollingState_TX_Data;
          }
        }
        else//���յ�����֡��Ч
        {
          delay = 0;
          PollingState    = PollingState_TX_Data;
        }
      }
      else//û���յ�Ӧ��
      {
        delay = 0;
        PollingState    = PollingState_TX_Data;
      }
    }
    break;
  default:
    {
      delay             = Task_Suspend;                                                 
      PollingState      = PollingState_TX_Polling;
    }
    break;
   }
  return(delay);
}

//ð������ lora�����ŵ��ź�ǿ�� Bubble_Sort_Lora(&lora);
void Bubble_Sort_Lora(Str_Traversal_Channel *point)  
{  
  for (uint8_t i=0;i<Lora_Max_Channels-1;i++)  
  {
    for (uint8_t j=0;j<Lora_Max_Channels-i-1;j++)  
    {  
      if (point->Rssi[j]<point->Rssi[j+1]) // �����ǴӴ�С��������Ǵ�С��������ֻ�轫��<�����ɡ�>��  
      {  
        uint8_t tem,num,id; 
    
        tem = point->Rssi[j]; 
        num = point->Num[j];
        id  = point->Father[j];
      
        point->Rssi[j]  = point->Rssi[j+1]; 
        point->Num[j]   = point->Num[j+1];
        point->Father[j]= point->Father[j+1];
        
        point->Rssi[j+1]= tem;  
        point->Num[j+1] = num;
        point->Father[j+1] = id;
        
      }  
    }  
  }
} 






















