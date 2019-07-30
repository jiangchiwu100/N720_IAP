#include "includes.h"



Union_LoRaSettings LoRaSettings;





void Lora_Rx_Init(void)                                                         
{
  LoraRx.len   = 0 ;
  LoraRx.rssi  = 0 ;
}

//������Ԫʱ��ms ��ʼ����CAD���ѷ���ǰ���� ��ʱ����Ҫ�õ�
float Sx1278_Calculated_SymbTime(Union_LoRaSettings *setting)
{
  float G_BandWidthKHz = 500.0;//���ؼ���Symbol����ʹ��
  
  switch(setting->Str_LoRaSettings.SignalBw )
  {
  case BW500KHZ:G_BandWidthKHz   = 500.0;break;
  case BW250KHZ:G_BandWidthKHz   = 250.0;break;
  case BW125KHZ:G_BandWidthKHz   = 125.0;break;
  case BW62_50KHZ:G_BandWidthKHz = 62.5;break;
  case BW41_66KHZ:G_BandWidthKHz = 41.66;break;
  case BW31_25KHZ:G_BandWidthKHz = 31.25;break;
  case BW20_83KHZ:G_BandWidthKHz = 20.83;break;
  case BW15_62KHZ:G_BandWidthKHz = 15.62;break;
  case BW10_41KHZ:G_BandWidthKHz = 10.41;break;
  case BW7_81KHZ:G_BandWidthKHz  = 7.81;break;
  }
  return((2<<((setting->Str_LoRaSettings.SpreadingFactor>>4)-1))/G_BandWidthKHz);
}

//������Ԫ���� ����CADǰ���볤�� ����time��λms
uint16_t Sx1278_Calculated_Cad_Preamble(float time)
{
  time = time/LoRaSettings.Str_LoRaSettings.SymbTime;
  return((uint16_t ) time);
}

void Sx1278_Init(void)
{
  Sx1278_GPIO_Init();
  
  LoRaSettings.Str_LoRaSettings.CrcOn = true;                   /* CRC У�� [0: OFF, 1: ON] */ 
  LoRaSettings.Str_LoRaSettings.ErrorCoding = CR_4_8;           /* ErrorCoding [4/5, 4/6, 4/7, 4/8]*/
  LoRaSettings.Str_LoRaSettings.ImplicitHeaderOn = false;       /*ImplicitHeaderOn [0: OFF, 1: ON] ע�⣬��ͷģʽ��Implicit Header��ʱ��������ǰ�涨���շ�˫����PL*/
  LoRaSettings.Str_LoRaSettings.PayloadLength = 128;            /*PayloadLength (used for implicit header mode)*/
  LoRaSettings.Str_LoRaSettings.Power = 10;                     /* ���书��*/
  LoRaSettings.Str_LoRaSettings.RFFrequency = 470000000;        /* �ز�Ƶ�� */
  LoRaSettings.Str_LoRaSettings.RxSingleOn = false;             /*[0: Continuous, 1 Single] */
  LoRaSettings.Str_LoRaSettings.SignalBw = BW125KHZ;            /* ���� */
  LoRaSettings.Str_LoRaSettings.SpreadingFactor = SF11;         /* ��Ƶ����[6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips] */ 
  LoRaSettings.Str_LoRaSettings.SymbTime = Sx1278_Calculated_SymbTime(&LoRaSettings);
  
  Sx1278_Lora_Init(&LoRaSettings);
}
 
void Sx1278_Lora_Init( Union_LoRaSettings *settings )
{
  Sx1278_Hardware_Reset();//Ӳ����λ
  if(Sx1278_Lora_Spi_Test()==true)//Ӳ��SPI ���� 
  {
    Sx1278_Lora_Set_On();          //����1278������loraģʽ     
    Sx1278_LoRa_Set_RFFrequency( settings->Str_LoRaSettings.RFFrequency );
    Sx1278_LoRa_Set_SpreadingFactor(settings->Str_LoRaSettings.SpreadingFactor); // SF6 only operates in implicit header mode.
    Sx1278_LoRa_Set_ErrorCoding(settings->Str_LoRaSettings.ErrorCoding);//Code rate ������
    Sx1278_LoRa_Set_PacketCrcOn(settings->Str_LoRaSettings.CrcOn);
    Sx1278_LoRa_Set_SignalBandwidth(settings->Str_LoRaSettings.SignalBw);
    Sx1278_LoRa_Set_ImplicitHeaderOn(settings->Str_LoRaSettings.ImplicitHeaderOn);
    Sx1278_LoRa_Set_SymbTimeout( 0x3FF );//����ԭ��������д ��һ����
    Sx1278_LoRa_Set_PayloadLength( settings->Str_LoRaSettings.PayloadLength );
    Sx1278_LoRa_Set_RFPower(settings->Str_LoRaSettings.Power);
    Sx1278_LoRa_Set_PaRamp(RFLR_PARAMP_0125_US);
    Sx1278_Lora_Set_RxGain();
    Sx1278_LoRa_Set_LowDatarateOptimize( true );
    Sx1278_LoRa_Set_AgcAutoOn( true );//��ΪAGC�Զ�����
    
    Sx1278_LoRa_Set_StandbyMode();
    
#ifdef PrintfEnable
    printf("********************LORA��ɳ�ʼ��*********************\n\r");
#endif
  }
  else
  {
    #ifdef PrintfEnable
    printf("********************LORA��ʼ������*********************\n\r");
    #endif
  }
}

//��������---ͨ������lora��ʽ��������
void Sx1278_Lora_Tx_Data(uint8_t *PayloadBuf, uint8_t Len,uint16_t PreambleNum)
{
  uint8_t RegIrqFlagsMask = 0;

//  Encrypt_Data(&*PayloadBuf,Len,LoraNetParams.Str_LoraNetworkParams.LoraKey);
  if (0 == PayloadBuf || 0 == Len) 
  {
    return ;
  }	
  else
  {
    RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                      RFLR_IRQFLAGS_RXDONE |
                      RFLR_IRQFLAGS_PAYLOADCRCERROR |
                      RFLR_IRQFLAGS_VALIDHEADER |
                      //RFLR_IRQFLAGS_TXDONE |
                      RFLR_IRQFLAGS_CADDONE |
                      RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                      RFLR_IRQFLAGS_CADDETECTED;
    SX1276Write( REG_LR_IRQFLAGSMASK, RegIrqFlagsMask );
    SX1276Write( REG_LR_HOPPERIOD , 0 );
    
    Sx1278_LoRa_Set_StandbyMode();
    Sx1278_LoRa_Set_PreambleLength(PreambleNum);

    SX1276Write( REG_LR_PAYLOADLENGTH,Len); /* ��implicitģʽ��( ��ʽ��ͷ),����д�� FIFO���� */
    SX1276Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE)); //����������ж�
    SX1276Write( REG_LR_FIFOTXBASEADDR, 0x00 );                   /* д�뷢�͵��׵�ַ             */ 
    SX1276Write( REG_LR_FIFOADDRPTR,0x00);                      //ָ��ָ���ַ00              
    SX1276WriteFifo( PayloadBuf, Len );                // Write payload buffer to LORA modem                  
    SX1276Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );//�ж�ӳ�䵽IO
    LoraTx_Enable();
    SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_TRANSMITTER ); //����LORAΪ����״̬ 
  }
  
  PrintSlotTime();
}

void Sx1278_LoRa_Get_Rx_Data( uint8_t *pbuf, uint8_t *size )
{
  uint8_t RegFifoRxCurrentAddr = 0;
  
  /************* ��ȡ���� ************/
  /* REG_LR_FIFORXCURRENTADDR ��ȡ���һ������������ָ��*/
  SX1276Read( REG_LR_FIFORXCURRENTADDR, &RegFifoRxCurrentAddr );  
  if( LoRaSettings.Str_LoRaSettings.ImplicitHeaderOn == true ) 
  {
    __no_operation();
  } 
  else 
  {
    SX1276Read( REG_LR_NBRXBYTES, size );	 	/* ��ȡ������ */
    SX1276Write( REG_LR_FIFOADDRPTR, RegFifoRxCurrentAddr );  /* д��FIFO���ʵ�ַ */
    SX1276ReadFifo( pbuf, *size);
  }
 
}

void Sx1278_LoRa_Interrupt_Process_DIO0(void)
{
  uint8_t RegIrqFlags = 0;
 
  RegIrqFlags = Sx1278_LoRa_Read_InterruptFlag();
  if( ( RegIrqFlags & RFLR_IRQFLAGS_RXDONE ) == RFLR_IRQFLAGS_RXDONE )          //������
  {
    Sx1278_LoRa_Reset_InterruptFlag( RFLR_IRQFLAGS_RXDONE );// Clear Irq
    if( ( RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) != RFLR_IRQFLAGS_PAYLOADCRCERROR ) /* û��CRC���� */ 
    {
      Sx1278_LoRa_Get_Rx_Data(&LoraRx.buf[0],&LoraRx.len);
      LoraRx.rssi = Sx1278_Lora_Get_Packet_RSSI(Sx1278_Lora_Get_SNR());

      
#ifdef  PrintfEnable
      printf("**��������**");
  
  for(uint8_t num=0;num<LoraRx.len;num++)
  {
    printf ("0x%x,",LoraRx.buf[num]);
  }
printf("\n\r");
#endif
      OSSemPost(SempLoraRx);  
    }
  }
  else if(( RegIrqFlags & RFLR_IRQFLAGS_TXDONE ) == RFLR_IRQFLAGS_TXDONE) //������ϵ�ʱ��
  {
    Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_TXDONE); // Clear Irq
    /**************д��־λ***************/
     OSSemPost(SempLoraTx); 
     PrintSlotTime();
  }
  else
  { 
    Sx1278_LoRa_Reset_InterruptFlag( RFLR_IRQFLAGS_TXDONE );// Clear Irq//�������
    Sx1278_LoRa_Reset_InterruptFlag( RFLR_IRQFLAGS_RXDONE );// Clear Irq
  }
  
  
  Sx1278_LoRa_Set_StandbyMode();//���ͻ��߽������֮�����standbyģʽ
  
}



  
  
