#include "includes.h"



Union_LoRaSettings LoRaSettings;





void Lora_Rx_Init(void)                                                         
{
  LoraRx.len   = 0 ;
  LoraRx.rssi  = 0 ;
}

//计算码元时间ms 初始化、CAD唤醒发送前导码 的时候需要用到
float Sx1278_Calculated_SymbTime(Union_LoRaSettings *setting)
{
  float G_BandWidthKHz = 500.0;//本地计算Symbol周期使用
  
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

//根据码元长度 计算CAD前导码长度 输入time单位ms
uint16_t Sx1278_Calculated_Cad_Preamble(float time)
{
  time = time/LoRaSettings.Str_LoRaSettings.SymbTime;
  return((uint16_t ) time);
}

void Sx1278_Init(void)
{
  Sx1278_GPIO_Init();
  
  LoRaSettings.Str_LoRaSettings.CrcOn = true;                   /* CRC 校验 [0: OFF, 1: ON] */ 
  LoRaSettings.Str_LoRaSettings.ErrorCoding = CR_4_8;           /* ErrorCoding [4/5, 4/6, 4/7, 4/8]*/
  LoRaSettings.Str_LoRaSettings.ImplicitHeaderOn = false;       /*ImplicitHeaderOn [0: OFF, 1: ON] 注意，无头模式（Implicit Header）时，必须提前规定好收发双方的PL*/
  LoRaSettings.Str_LoRaSettings.PayloadLength = 128;            /*PayloadLength (used for implicit header mode)*/
  LoRaSettings.Str_LoRaSettings.Power = 10;                     /* 发射功率*/
  LoRaSettings.Str_LoRaSettings.RFFrequency = 470000000;        /* 载波频率 */
  LoRaSettings.Str_LoRaSettings.RxSingleOn = false;             /*[0: Continuous, 1 Single] */
  LoRaSettings.Str_LoRaSettings.SignalBw = BW125KHZ;            /* 带宽 */
  LoRaSettings.Str_LoRaSettings.SpreadingFactor = SF11;         /* 扩频因子[6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips] */ 
  LoRaSettings.Str_LoRaSettings.SymbTime = Sx1278_Calculated_SymbTime(&LoRaSettings);
  
  Sx1278_Lora_Init(&LoRaSettings);
}
 
void Sx1278_Lora_Init( Union_LoRaSettings *settings )
{
  Sx1278_Hardware_Reset();//硬件复位
  if(Sx1278_Lora_Spi_Test()==true)//硬件SPI 测试 
  {
    Sx1278_Lora_Set_On();          //设置1278工作在lora模式     
    Sx1278_LoRa_Set_RFFrequency( settings->Str_LoRaSettings.RFFrequency );
    Sx1278_LoRa_Set_SpreadingFactor(settings->Str_LoRaSettings.SpreadingFactor); // SF6 only operates in implicit header mode.
    Sx1278_LoRa_Set_ErrorCoding(settings->Str_LoRaSettings.ErrorCoding);//Code rate 编码率
    Sx1278_LoRa_Set_PacketCrcOn(settings->Str_LoRaSettings.CrcOn);
    Sx1278_LoRa_Set_SignalBandwidth(settings->Str_LoRaSettings.SignalBw);
    Sx1278_LoRa_Set_ImplicitHeaderOn(settings->Str_LoRaSettings.ImplicitHeaderOn);
    Sx1278_LoRa_Set_SymbTimeout( 0x3FF );//跟随原厂驱动改写 不一定用
    Sx1278_LoRa_Set_PayloadLength( settings->Str_LoRaSettings.PayloadLength );
    Sx1278_LoRa_Set_RFPower(settings->Str_LoRaSettings.Power);
    Sx1278_LoRa_Set_PaRamp(RFLR_PARAMP_0125_US);
    Sx1278_Lora_Set_RxGain();
    Sx1278_LoRa_Set_LowDatarateOptimize( true );
    Sx1278_LoRa_Set_AgcAutoOn( true );//改为AGC自动增益
    
    Sx1278_LoRa_Set_StandbyMode();
    
#ifdef PrintfEnable
    printf("********************LORA完成初始化*********************\n\r");
#endif
  }
  else
  {
    #ifdef PrintfEnable
    printf("********************LORA初始化故障*********************\n\r");
    #endif
  }
}

//发送数据---通过无线lora方式发送数据
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

    SX1276Write( REG_LR_PAYLOADLENGTH,Len); /* 在implicit模式下( 隐式包头),必须写入 FIFO长度 */
    SX1276Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE)); //开发送完成中断
    SX1276Write( REG_LR_FIFOTXBASEADDR, 0x00 );                   /* 写入发送的首地址             */ 
    SX1276Write( REG_LR_FIFOADDRPTR,0x00);                      //指针指向地址00              
    SX1276WriteFifo( PayloadBuf, Len );                // Write payload buffer to LORA modem                  
    SX1276Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );//中断映射到IO
    LoraTx_Enable();
    SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_TRANSMITTER ); //设置LORA为发送状态 
  }
  
  PrintSlotTime();
}

void Sx1278_LoRa_Get_Rx_Data( uint8_t *pbuf, uint8_t *size )
{
  uint8_t RegFifoRxCurrentAddr = 0;
  
  /************* 读取数据 ************/
  /* REG_LR_FIFORXCURRENTADDR 读取最后一包接收数据首指针*/
  SX1276Read( REG_LR_FIFORXCURRENTADDR, &RegFifoRxCurrentAddr );  
  if( LoRaSettings.Str_LoRaSettings.ImplicitHeaderOn == true ) 
  {
    __no_operation();
  } 
  else 
  {
    SX1276Read( REG_LR_NBRXBYTES, size );	 	/* 读取包长度 */
    SX1276Write( REG_LR_FIFOADDRPTR, RegFifoRxCurrentAddr );  /* 写入FIFO访问地址 */
    SX1276ReadFifo( pbuf, *size);
  }
 
}

void Sx1278_LoRa_Interrupt_Process_DIO0(void)
{
  uint8_t RegIrqFlags = 0;
 
  RegIrqFlags = Sx1278_LoRa_Read_InterruptFlag();
  if( ( RegIrqFlags & RFLR_IRQFLAGS_RXDONE ) == RFLR_IRQFLAGS_RXDONE )          //接收完
  {
    Sx1278_LoRa_Reset_InterruptFlag( RFLR_IRQFLAGS_RXDONE );// Clear Irq
    if( ( RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) != RFLR_IRQFLAGS_PAYLOADCRCERROR ) /* 没有CRC错误 */ 
    {
      Sx1278_LoRa_Get_Rx_Data(&LoraRx.buf[0],&LoraRx.len);
      LoraRx.rssi = Sx1278_Lora_Get_Packet_RSSI(Sx1278_Lora_Get_SNR());

      
#ifdef  PrintfEnable
      printf("**接收数据**");
  
  for(uint8_t num=0;num<LoraRx.len;num++)
  {
    printf ("0x%x,",LoraRx.buf[num]);
  }
printf("\n\r");
#endif
      OSSemPost(SempLoraRx);  
    }
  }
  else if(( RegIrqFlags & RFLR_IRQFLAGS_TXDONE ) == RFLR_IRQFLAGS_TXDONE) //发送完毕的时候
  {
    Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_TXDONE); // Clear Irq
    /**************写标志位***************/
     OSSemPost(SempLoraTx); 
     PrintSlotTime();
  }
  else
  { 
    Sx1278_LoRa_Reset_InterruptFlag( RFLR_IRQFLAGS_TXDONE );// Clear Irq//其他情况
    Sx1278_LoRa_Reset_InterruptFlag( RFLR_IRQFLAGS_RXDONE );// Clear Irq
  }
  
  
  Sx1278_LoRa_Set_StandbyMode();//发送或者接受完成之后进入standby模式
  
}



  
  
