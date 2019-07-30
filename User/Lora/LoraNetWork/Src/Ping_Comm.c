#include "Driverlib.h"



SCH_TCB PingSlotCommTaskTcb;
static uint16_t SlotTimeTick = 0;//ʱ϶�δ�


//ʱ϶�δ��ʼ�� 
void Ping_Slot_Tick_Init(uint16_t dat)
{
  SlotTimeTick = dat;
}

//��ʼ��ʱ϶���
void Ping_Slot_Num_Init(uint16_t dat)
{
  LoraNetParams.Str_LoraNetworkParams.SlotNetNum  = dat * 8 ;
  LoraNetParams.Str_LoraNetworkParams.SlotNetNum += 4;//����ʱ϶�� ����ʱ϶���ѵ�
}

//ʱ϶ʱ�䴥��
bool Ping_Slot_Trigger(void)
{
  if( ++SlotTimeTick >= 5760 ) //�ۼ�ʱ϶�δ� 360�� 
  {
    SlotTimeTick = 0;
  }
  
  if( LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus ) //���������ʱ϶����״̬
  { 
    if((SlotTimeTick == LoraNetParams.Str_LoraNetworkParams.SlotNetNum) || (SlotTimeTick == 5632 )) //�Ƿ񵽴�ʱ϶��
    {
      SCHTaskResume(PingSlotCommTaskTcb);
      W_State(&LoraState,CadTaskIng,true);
      return(true);
    }
    else
    {
      return(false);
    }
  }
  else
  {
    return(false);
  }
}

 
/*
  �´ﱱ�����ݲɼ�������
*/
void Ping_Slot_Comm_Task(void)// ������������
{
  static uint16_t PingTaskDelay = 0;
  
  SCHTaskBegin();
  
  while (1)
  {
    if( LoraNetParams.Str_LoraNetworkParams.NetworkStatus == LoraNetwork_PingSlotCommStatus )//cadֻ��ʱ϶״̬�¿�������
    {
      PingTaskDelay = Ping_Slot_Comm();
      if( PingTaskDelay == Cad_Suspend )
      {   
        Mbus_Vcc_Ds() ;         // �رմ��� �ر�24V  �رս��յ�·
        LM358_Vcc_Ds();
        SCHTaskResume(SleepTaskTcb);
        W_State(&SysState,SleepDisable,false);
        Sx1278_LoRa_Set_SleepMode();                                            //���߽�������
        W_State(&LoraState,CadTaskIng,false);
        SCHCurTaskPend();
      }
      else
      {
        SCHCurTaskDly( PingTaskDelay / SCH_HW_TIM_MS );      //2����һ��
      }
    }
    else
    {
      SCHCurTaskPend();//�������
    }
  }
  
  SCHTaskEnd();
}
/*
��ע���㳭����ʱ�����������ѽڵ㣬����Ҫ�ڵ�Ӧ����Ϊֻ��һ�λ��ᣬӦ�����û��
���𣬷������ӽڵ㹦�ġ�
�´ﱱ�����ݲɼ�������
*/
uint16_t Ping_Slot_Comm(void)
{
  static   ENUM_PingComm_Status PingCommState = Ping_Enable ;
  static   uint8_t CadLoopNum=0;
  static   Union_Lora_Protocol_MSG_Q msg;
  uint16_t  delay=0;
  
  switch(PingCommState)
  {
  case Ping_Enable:     //����cad 
    { 
      for(uint8_t num=0;num<69;num++)
      {
        msg.dat[num] = 0;
      }
      if( R_State(&LoraState,CadTaskIng) == true )
      {
  #ifdef Debug 
        Test(); //��ping�ŵ���������ping����֡
  #endif
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PingLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]);
        W_State(&LoraState,CadDetected,false) ; //�����־     
        W_State(&LoraState,CadDone,false) ;     //�����־          
        Sx1278_LoRa_Set_Cad_Init() ;            //����CAD
        delay           = 250;    //Ԥ�����㹻��ʱ��
        PingCommState   = Ping_Deteced;
      }
      else
      {
        delay           = Task_Suspend;         //�������� 
        PingCommState   = Ping_Enable;          
      } 
    }
    break;
  case Ping_Deteced:    //����Ƿ��յ�ǰ����
    {
      if(R_State(&LoraState,CadDone) && R_State(&LoraState,CadDetected)) //��⵽ǰ����
      { 
  #ifdef Debug 
        Test(); //��ping�ŵ���������ping����֡
  #endif
        Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_PingLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]); 
        Enable_Lora_Rx();               //�����⵽ǰ���� ����CAD�ŵ��򿪽���
        delay           = 5000;         //��һ����������
        PingCommState   = Ping_Rx_Ping; //״̬�����뵽����ping����֡
      }
      else
      { 
        if(++CadLoopNum >= 3)//CAD���ӵ�3�� ��߿ɿ���
        {
          CadLoopNum = 0;                                     //�����־  
          delay         = Task_Suspend;                                          //�������� 
          PingCommState = Ping_Enable;
        }
        else
        { 
          delay         = 0;    
          W_State(&LoraState,CadTaskIng,false); //�������� 
          PingCommState = Ping_Enable;
        }
      }
    }
    break;
  case Ping_Rx_Ping:    //�鿴�Ƿ��յ�ping����֡
    {  
      if(R_State(&LoraState,RfRcv_OK) == true) //���յ�����֡
      {     
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg);    //����lora��������֡
        
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true) //����֡��Ч
        {                                                               
          if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_PingType )     //����֡����Ϊping�㳭���ѽڵ�
          {                                                             
            if(msg.Str_Lora_Protocol_MSG_Q.FatherID == LoraNetParams.Str_LoraNetworkParams.FatherID ) //���ڵ㷢�͵�beacon
            {
              Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]); 
              Enable_Lora_Rx();                                          
              delay             = 5000;                                    //��ʱ3��
              PingCommState     = Ping_Rx_Data;                            // ��ʼ��������֡
            }
            else                                                                                     //���ڵ㲻��ȷ
            {
              delay             = Task_Suspend;                   
              PingCommState     = Ping_Enable; 
            }
          }
          else if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_BroadcastTime)//֡����Ϊ�㲥�·�ʱ϶
          { //ÿСʱһ�ι㲥ʱ��
            LoraNetParams.Str_LoraNetworkParams.BroadcastTimeNum++; //ÿ��
            delay               = Task_Suspend; //�������� 
            PingCommState       = Ping_Enable;
          }
          else if(msg.Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode == NetProtocol_BroadcastSleepRtc)//֡����Ϊ�㲥�·�����ʱ��
          {
            delay               = Task_Suspend; //�������� 
            PingCommState       = Ping_Enable;
          }
          else                                                                            //��������֡���� 
          {
            delay               = Task_Suspend;                        
            PingCommState       = Ping_Enable; 
          }
        }
        else                                             //����������Ч
        {
          delay                 = Task_Suspend;                    
          PingCommState         = Ping_Enable; 
        }
      }
      else                                     //δ���յ�����֡
      {
        delay                   = Task_Suspend;                         //�������� 
        PingCommState           = Ping_Enable;
      }
    }
    break;
  case Ping_Rx_Data:    
    { 
      if(R_State(&LoraState,RfRcv_OK) == true)  //���յ�����֡
      {     
        Resolve_LoRa_Protocol_Frame(&LoraRx.buf[0],LoraRx.len,&msg); //����lora��������֡ 
        
        if(msg.Str_Lora_Protocol_MSG_Q.Validity == true) //������Ч��
        {
          if(msg.Str_Lora_Protocol_MSG_Q.FatherID == LoraNetParams.Str_LoraNetworkParams.FatherID)//�ǲ��Ƕ�Ӧ�ĸ��ڵ�
          { 
            Uart_Tx_Init();
            Combine_XDKJ_188(&msg.Str_Lora_Protocol_MSG_Q.Buf[0],msg.Str_Lora_Protocol_MSG_Q.BufLen,&UartTx); //ͨ��645���� ����´�Ƽ��·�����֡ //��䵽�����·����ݻ�����
            Mbus_Vcc_En();                      //��24V��Դ
            LM358_Vcc_En();                     //�򿪽��յ�Դ
            if(UartTx.rssi == 0x01)
            {
              delay           = 1500;           //�ȱ���ʱ1.5 s
            }
            else if(UartTx.rssi == 0x02)
            {
              delay           = 8000;           //������ʱ8 s
            }
            PingCommState     = Ping_MBUS_Tx;                         
          }
          else                                                                                  //���ڵ����
          {
            delay               = Task_Suspend; //�������� 
            PingCommState       = Ping_Enable;
          }
        }
        else                                    //������Ч
        {
          delay                 = Task_Suspend; //�������� 
          PingCommState         = Ping_Enable; 
        }
      }
      else                                      //û�н��յ����� ��ʱ��� 
      {
        delay                   = Task_Suspend; //�������� 
        PingCommState           = Ping_Enable;
      }
    }
    break; 
  case Ping_MBUS_Tx://���MBUSԤ�� ����
    {
      W_State(&SysState,SleepDisable,true);     //
      SCHTaskPend(SleepTaskTcb);                // ֹͣ���߳��� 
      Uart_Rx_Init();
      if(UartTx.rssi == 0x01)
      {
        Sys_Uart_Config();//�ȱ��ڳ�ʼ��
      }
      else if(UartTx.rssi == 0x02)
      {
        Sys_Uart_Valve_Config();//���Ŵ��ڳ�ʼ��
      }
      Uart_Rx_Init();//��ʼ�����ջ���
      Uart_Tx();     //��������
      delay = 1000;  //����+���� ��ʱ
      PingCommState = Ping_MBUS_Rx;
    }
    break;
  case Ping_MBUS_Rx:
    {
      Str_Lora rxtem ;
      
      Mbus_Vcc_Ds();// �رմ��� �ر�24V  �رս��յ�·
      LM358_Vcc_Ds();
      SCHTaskResume(SleepTaskTcb);
      W_State(&SysState,SleepDisable,false);
      Lora_Tx_Init();
      Sx1278_LoRa_Set_RFFrequency(Lora_Frequency_CommLink[LoraNetParams.Str_LoraNetworkParams.ChannleNum]);
      Resolve_XDKJ_188(&UartRx.buf[0],UartRx.len,&rxtem);//������������
      memcpy(&msg.Str_Lora_Protocol_MSG_Q.Buf[0],&rxtem.buf[0],rxtem.len);//�����ش��´�Ƽ�����֡ ��ȡ��Ч���� ��֯��645 
      msg.Str_Lora_Protocol_MSG_Q.BufLen = rxtem.len;
      Combine_LoRa_Protocol_Frame(&msg);//��645������䵽lora����֡�� //���ͳ�ȥ
      Disable_Uart_One();
      Uart_Rx_Init();
      Uart_Tx_Init();
      Lora_Rx_Init();
      Lora_Tx_Init();
      delay                   = 5000; 
      PingCommState           = Ping_Tx_Data; 
    }
    break;
  case Ping_Tx_Data://�������֮��  Ҫ��һ�ν��� ���������û�н��յ�Ӧ�� ���������
    {
      if(R_State(&LoraState,RfSend_OK) == true)//�����������
      {
        Enable_Lora_Rx();                       //�����⵽ǰ���� ����CAD�ŵ��򿪽���
        delay                   = 4000;         //�ٵȴ�һ������ Ȼ�󿪽��� 
        PingCommState           = Ping_Rx_Data; //״̬�����뵽����ping����֡
      }
      else                                      //���ͳ����쳣
      {
        delay                   = Task_Suspend; //�������� 
        PingCommState           = Ping_Enable;
      }
    }
    break;
  default:
    {
      Disable_Uart_One();
      Uart_Rx_Init();
      Uart_Tx_Init();
      Lora_Rx_Init();
      Lora_Tx_Init();
      delay                   = Task_Suspend; //�������� 
      PingCommState           = Ping_Enable;
    }
    break;
  }
  return(delay);
}

