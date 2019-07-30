
#include "includes.h"

static uint32_t SlotTimeNetworkTick=0;
static uint8_t  RfChannelNum = 16    ;
static Str_Com Rf_Rx,Rf_Tx           ;
static uint32_t RadioStatus = 0      ;//������Ƶ����״̬����
const  uint32_t Rfchannel[64]={
  458375000,458750000,459125000,459500000,459875000,460250000,460625000,461000000,
  461375000,461750000,462125000,462500000,462875000,463250000,463625000,464000000,
  464375000,464750000,465125000,465500000,465875000,466250000,466625000,467000000, 
  467375000,467750000,468125000,468500000,468875000,469250000,469625000,470000000, 
  470375000,470750000,471125000,471500000,471875000,472250000,472625000,473000000,
  473375000,473750000,474125000,474500000,474875000,475250000,475625000,476000000, 
  478000000,478375000,478750000,479125000,479500000,479875000,480250000,480625000, 
  481000000,481375000,481750000,482125000,482500000,482875000,483250000,483625000, 
};


TaskHandle_t xHandleRFSlotTimeNetWorkTickTask    = NULL;//ʱ϶tick ����
TaskHandle_t xHandleRFInterceptPublicChannelTask = NULL;//�㲥����
TaskHandle_t xHandleRFSlotTimeBroadcastTask      = NULL;//ʱ϶�㲥
/*******************************************************************************
** Function name:           Rf_Rx_Buf_Init
** Descriptions:            RF���ջ����ʼ��
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2015-5-25     
*******************************************************************************/
void Rf_Rx_Buf_Init(void)
{
  Rf_Rx.len=0;
}

/*******************************************************************************
** Function name:           Rf_Tx_Buf_Init
** Descriptions:            RF���仺���ʼ��
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2015-5-25     
*******************************************************************************/
void Rf_Tx_Buf_Init(void)
{
  Rf_Tx.len=0;
}

/*******************************************************************************
** Function name:           Rf_Rx_Task
** Descriptions:            ����Rf����  ��rf�жϳ����е���
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2015-5-25     
*******************************************************************************/
void Rf_Rx_Buff(uint8_t *buf,uint8_t len,uint16_t rssi)
{
  
  if(len == 0)
    return;
  
  for(Rf_Rx.len=0;Rf_Rx.len<len;Rf_Rx.len++)
  {
    Rf_Rx.buf[Rf_Rx.len] = buf[Rf_Rx.len];
  }
  Rf_Rx.Rssi = rssi;
}

//�ۼ�ʱ϶����ʱ��
void RF_Slot_Time_Net_Work_Tick_Task( void *pvParameters )
{
  
  static TickType_t RfNetWorkTick;
  
  /* ��ȡ��ǰ��ϵͳʱ�� */
  RfNetWorkTick = xTaskGetTickCount();
  
  while(1)
  {
    if( SlotTimeNetworkTick++ >= 0xffff )
    {
      SlotTimeNetworkTick = 0 ;
    }
    vTaskDelayUntil( &RfNetWorkTick , 125 );//125ms  
  }
}


//���ʱ϶����ʱ��
void Reset_Slot_Time_Net_Work_Tick(void)
{
  SlotTimeNetworkTick = 0 ;
}


//����ʱ϶����ʱ��
void Set_Slot_Time_Net_Work_Tick(uint16_t data)
{
  SlotTimeNetworkTick = data ; 
}



void Reset_Sx1278(void)
{  
  SX1276Reset();
  if(SpiTest())
  {
    LoRa_Init_TypeDef lorainit = 
    {
      458375000,        //����Ƶ��
      BW125KHZ,         //����
      SF11,             //��Ƶ����
      CR_4_5,           //CRC
      19,                //�������
      true,             //CRC����
      true,             //ExplicitHeaderOn [true:��ʽ��ͷģʽ ]
    };  
    SX1278_Lora_Init(&lorainit);
    Lora_RxMode();      /* ��Ϊ����ģʽ */
  }
  
}

//�����������������Լ��쳣��������
//�ϵ������ �������г���������ʱ�����뵽CAD������  
//����ʱ϶�ռ�  ���㹫ʽ = ��2400 + x * 40����2400ʱ϶��ʼ  ÿ�������40��

void  RF_Intercept_Public_Channel_Task( void *pvParameters )
{
  static uint16_t InterceptTick = 100;
  static uint8_t  PollingStatus = InterceptPollingInit;
  
  while(1)
  {
    
    if((!R_Status(&RadioStatus,CadSlotTask)) && (!R_Status(&RadioStatus,CadBroadcastTask)))
    {  //ʱ϶�㲥��ʱ϶����û���ڽ���  �����������ŵ�
      if( Get_Meter_Lib_DataItem_Num() > 0 )//������г�������
      { 
        W_Status(&RadioStatus,InterceptPublicChannelTask,false);
        PollingStatus = InterceptPollingInit;//��λ��������״̬��
        InterceptTick = 1000;            //������������Ƶ�� ��CPU������������
        //����ʱ϶�������� 
      }
      else                                 //û�г�����������������ŵ�����
      {              
        switch(PollingStatus)//�������� ״̬������
        {
        case InterceptPollingInit:
          {
            Reset_Sx1278();
            InterceptTick = 30; /* ������ʱʱ�� */
            PollingStatus = InterceptPolling;
          }
          break;
        case InterceptPolling://�����ڵ��ϱ�
          {
            if( R_Status(&RadioStatus,RfReceiveOK) )
            {
              if(Rf_Rx.buf[0] == 0x9a)     //֡ͷ�Ƿ���ȷ
              {
                if(Rf_Rx.buf[Rf_Rx.buf[1]+3] == 0x16) //֡β�Ƿ���ȷ
                {
                  if(Rf_Rx.buf[Rf_Rx.buf[1]+2] == Count_CheckSum(&Rf_Rx.buf[2],(Rf_Rx.len-4))) 
                  {
                    B32_B08 nodenum ;
                    Union_Frame_Cmd Rf;
                    
                    Rf.Cmd = Rf_Rx.buf[2] ; 
                    switch(Rf.Str_Frame_Cmd.FrameType)
                    {
                    case 0://����֡
                      {
                        nodenum.B32 = Get_Meter_Lib_NodeNum(&Rf_Rx.buf[3]);
                        if(nodenum.B32 < MeterLibraryMaxNum)//�ж��Ƿ��ڱ����
                        {//�ڱ���� ����
                          nodenum.B32  = (2400 + nodenum.B32  * 40);//����ʱ϶ʱ��
                          Rf_Tx.buf[Rf_Tx.len++] = 0x9a;//��֯�ش�����֡ 
                          Rf_Tx.buf[Rf_Tx.len++] = 0x0a;//����10
                          
                          Rf.Str_Frame_Cmd.Reserve = 0;
                          Rf.Str_Frame_Cmd.FrameType = 0;
                          Rf.Str_Frame_Cmd.JoinNetwork = 1;
                          Rf.Str_Frame_Cmd.ACK  =0;
                          Rf.Str_Frame_Cmd.FrameDirection = 0;
                          
                          Rf_Tx.buf[Rf_Tx.len++] = Rf.Cmd;
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[3];
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[4]; 
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[5];  
                          Rf_Tx.buf[Rf_Tx.len++] = Rf_Rx.buf[6];
                          Rf_Tx.buf[Rf_Tx.len++] = RfChannelNum;
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[0];
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[1];
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[2];
                          Rf_Tx.buf[Rf_Tx.len++] = nodenum.B8[3];
                          Rf_Tx.buf[Rf_Tx.len++] = Count_CheckSum(&Rf_Tx.buf[2],Rf_Tx.buf[1]);//У���
                          Rf_Tx.buf[Rf_Tx.len++] = 0x16;//��β
                          Lora_Send_Data(&Rf_Tx.buf[0] , Rf_Tx.len , 0x08);
                          W_Status(&RadioStatus,RfSendOK,false);
                          PollingStatus = AnswerPollingDone;//ת��״̬
                          InterceptTick = 1000;//������ʱ  �ȴ�������� 
                        }
                      }
                      break;
                    case 4://�쳣�ϱ�֡
                      {
                        
                      }
                      break;
                    default://������������֡������
                      break;
                    }
                  }
                }
              }
            }
            Rf_Rx_Buf_Init();
          }
          break;
        case AnswerPollingDone://��Ӧ��� ��Ϊ���ռ���
          {
            if(R_Status(&RadioStatus,RfSendOK))//���ͳɹ�
            {
              W_Status(&RadioStatus,RfSendOK,false);   //�����־λ
              W_Status(&RadioStatus,RfReceiveOK,false);//�����־λ
              PollingStatus = InterceptPolling ;       //1ms����ת������
              InterceptTick = 1;                       //1ms����ת������ 
            }
            else //���Ͳ��ɹ� Ӳ������
            {
              Reset_Sx1278();//���¸�λ����ģ��
              W_Status(&RadioStatus,RfSendOK,false);   //�����־λ
              W_Status(&RadioStatus,RfReceiveOK,false);//�����־λ
              PollingStatus = InterceptPolling ;       //1ms����ת������
              InterceptTick = 1;                       //1ms����ת������      
            }
            Rf_Tx_Buf_Init();
          }
          break;
        default:
          break;
        }
      }
    }
    else//ʱ϶�㲥����ʱ϶�������ڽ���  ��ʱ������������
    {
      W_Status(&RadioStatus,InterceptPublicChannelTask,false);
      PollingStatus = InterceptPolling;//��λ��������״̬��
      InterceptTick = 1000;            //������������Ƶ�� ��CPU������������
    }
    
    vTaskDelay(InterceptTick);    
  }
}


//ʱ϶�㲥����  һ�����ֺͽڵ������ ����ռ���ŵ�  
void RF_Slot_Time_Broadcast_Task( void *pvParameters )
{
  static uint8_t    SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
  static uint8_t    TxBeaconLoop=0;
  static TickType_t RfBroadcastTimeTick = 1000 * 60 *15;
  /* ��ȡ��ǰ��ϵͳʱ�� */

  while(1)
  {
    if( !R_Status(&RadioStatus,CadSlotTask) )//�����ʱ϶����û�н�����15���ӽ���ʱ϶�㲥
    {
      if( R_Status(&RadioStatus,InterceptPublicChannelTask) )//�������� �ڽ���
      {
        W_Status(&RadioStatus,InterceptPublicChannelTask,false);//ֹͣ�������� 
      }
      W_Status(&RadioStatus,CadBroadcastTask,true);
      
      switch(SlotTimeBroadcastStatus)//ʱ϶�㲥״̬
      {
      case SlotTimeBroadcastPreamble:
        {
          uint8_t wakeupbuf[]={0xaa,0xaa,0xaa};
          float G_TsXms;
          uint16_t Preamble_Length=0;
          
          SX1276Reset();
          LoRa_Init_TypeDef lorainit = 
          {
            458375000,        //����Ƶ��
            BW125KHZ,         //����
            SF11,             //��Ƶ����
            CR_4_5,           //CRC
            19,                //�������
            true,             //CRC����
            true,             //ExplicitHeaderOn [true:��ʽ��ͷģʽ ]
          };  
          G_TsXms = SX1278_Lora_Init(&lorainit);
          Preamble_Length = (uint16_t)(60100/G_TsXms+0.5);//���ѳ���5010ms
          Lora_Send_Data(wakeupbuf , 3 , Preamble_Length);
          W_Status(&RadioStatus,RfSendOK,false);
          SlotTimeBroadcastStatus = SlotTimeBroadcastPreambleDone;
          
          RfBroadcastTimeTick = 61000  ; //   61��
        }
        break;
      case SlotTimeBroadcastPreambleDone:
        {
          if( R_Status(&RadioStatus,RfSendOK))
          {
             W_Status(&RadioStatus,RfSendOK,false);
             SlotTimeBroadcastStatus = SlotTimeBroadcastPreambleDone;
             TxBeaconLoop = 0;//׼����ʼ���㲥����
             RfBroadcastTimeTick = 1 ; //1����+500ms
          }
          else
          {
            TxBeaconLoop = 0;//׼����ʼ���㲥����     //ֹͣ����  
            SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
            RfBroadcastTimeTick = 1000 * 60 * 15 ; //15����
          }
        }
        break;
      case SlotTimeBroadcastBeacon://����Beacon
        {
          if( TxBeaconLoop++ > 180 )//��������180�� 
          {
            TxBeaconLoop = 0;
            SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
            RfBroadcastTimeTick = 1000 * 60 * 15 ; //15����
          }
          else
          {
            uint8_t buf[5]; //��֯ʱ϶Beacon����֡  ���ͳ�ȥ
            Union_Frame_Cmd Rf;
            
            buf[0] = 0x9A;
            buf[1] = 0x05;
            Rf.Str_Frame_Cmd.ACK = false;           //����Ҫ�ش�
            Rf.Str_Frame_Cmd.FrameDirection = false;//����
            Rf.Str_Frame_Cmd.FrameType    =  1;//�㲥Уʱ֡
            Rf.Str_Frame_Cmd.JoinNetwork  =  1;//�Ѿ���������
            Rf.Str_Frame_Cmd.Reserve    = 0;
            buf[2] = Rf.Cmd;
            buf[3] = 0x00;
            buf[4] = 0x16;
            Lora_Send_Data(buf,0x05,0x08);
            SlotTimeBroadcastStatus = SlotTimeBroadcastBeaconDone;
            W_Status(&RadioStatus,RfSendOK,false);
            RfBroadcastTimeTick = 600 ; //15����
          }
        }
        break;
      case SlotTimeBroadcastBeaconDone://�������  
        {
          if( R_Status(&RadioStatus,RfSendOK))
          {
            W_Status(&RadioStatus,RfSendOK,false);
            RfBroadcastTimeTick = 1;
            SlotTimeBroadcastStatus = SlotTimeBroadcastBeacon;
          }
          else
          {
            TxBeaconLoop = 0;
            SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
            RfBroadcastTimeTick = 1000 * 60 * 15 ; //15����
          }
        }
        break;
      default:
        break;
      }
      
      W_Status(&RadioStatus,CadBroadcastTask,true);//��ʼʱ϶�㲥�������� 
    } 
    else                                    //�����ʱ϶�����ڽ��������˳� 
    {
      TxBeaconLoop = 0;
      SlotTimeBroadcastStatus = SlotTimeBroadcastPreamble;
      RfBroadcastTimeTick = 1000 * 60 * 15 ; //15����
    }
    vTaskDelay(RfBroadcastTimeTick);
  }
}



//RF_DIO_0
void EXTI2_IRQHandler(void)
{
  uint8_t state;
  
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line2);
    Nop();
    state=Lora_Interrupt_Flag();
    if( state & RFLR_IRQFLAGS_RXDONE )
    {//��������ж� 
      uint8_t len=0,buf[128]={0},rssi=0;
      len = Lora_Get_Data(buf);
      rssi = Lora_Packet_Rssi();
      Rf_Rx_Buff(buf,len,rssi);
      W_Status(&RadioStatus,RfReceiveOK,true);
    }
    else if( state & RFLR_IRQFLAGS_TXDONE )
    {//��������ж�
      W_Status(&RadioStatus,RfSendOK,true);
    }
    
    Lora_RxMode();
    
  }
  
}

//RF_DIO_1
void EXTI3_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line3);/* Clear the  EXTI line 0 pending bit */
    
    Nop();
  }
}

//RF_DIO_3
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line0);/* Clear the  EXTI line 0 pending bit */
    
    Nop();
  }
}



