/*! 
 * \file       sx1278-LoRaMisc.c
 * \brief      SX1278 RF chip high level functions driver
 *
 * \remark     Optional support functions.
 *             These functions are defined only to easy the change of the
 *             parameters.
 *             For a final firmware the radio parameters will be known so
 *             there is no need to support all possible parameters.
 *             Removing these functions will greatly reduce the final firmware
 *             size.
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */

#include "includes.h"



/*!
 * Constant values need to compute the RSSI value
 */
#define RSSI_OFFSET_LF                              -155.0
#define RSSI_OFFSET_HF                              -150.0
#define NOISE_ABSOLUTE_ZERO                         -174.0
#define NOISE_FIGURE_LF                                4.0
#define NOISE_FIGURE_HF                                6.0 
/*!
 * Precomputed signal bandwidth log values
 * Used to compute the Packet RSSI value.
 */
const double SignalBwLog[] =
{
  3.8927900303521316335038277369285,  // 7.8 kHz
  4.0177301567005500940384239336392,  // 10.4 kHz
  4.193820026016112828717566631653,   // 15.6 kHz
  4.31875866931372901183597627752391, // 20.8 kHz
  4.4948500216800940239313055263775,  // 31.2 kHz
  4.6197891057238405255051280399961,  // 41.6 kHz
  4.795880017344075219145044421102,   // 62.5 kHz
  5.0969100130080564143587833158265,  // 125 kHz
  5.397940008672037609572522210551,   // 250 kHz
  5.6989700043360188047862611052755   // 500 kHz
};

const double RssiOffsetLF[] =
{   // These values need to be specify in the Lab
  -155.0,
  -155.0,
  -155.0,
  -155.0,
  -155.0,
  -155.0,
  -155.0,
  -155.0,
  -155.0,
  -155.0,
};
 

#define XTAL_FREQ       32000000
#define FREQ_STEP       61.03515625
 
bool Sx1278_Lora_Spi_Test(void)
{
  uint8_t test = 0;
  
  SX1276Write( REG_LR_OPMODE, RFLR_OPMODE_SLEEP );            
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_SLEEP );       
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY );
  
  SX1276Write( REG_LR_HOPPERIOD,0x91 );//ѡһ���ò����ļĴ���������֤
  SX1276Read( REG_LR_HOPPERIOD,&test);
  
  if(test!=0x91)
    return false;    
  else
    return true;
}

void Sx1278_Lora_Set_On(void)
{
  uint8_t RegOpMode = 0;
  
//  Sx1278_LoRa_Set_SleepMode();//loraģʽ����˯��ģʽ�¿����޸�
  SX1276Read( REG_LR_OPMODE,&RegOpMode );
  RegOpMode = ( RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
  SX1276Write( REG_LR_OPMODE, RegOpMode );
}

// 0 : û������ 1�� ������� 6�� ��С����
void Sx1278_Lora_Set_RxGain(void)
{
  uint8_t RegLna=0;
  
  SX1276Read( REG_LR_LNA, &RegLna );
  RegLna = (( RegLna & RFLR_LNA_GAIN_MASK ) | RFLR_LNA_GAIN_G1 );
  SX1276Write( REG_LR_LNA,RegLna );
}

int8_t Sx1278_Lora_Get_SNR (void)
{  
  uint8_t rxSnrEstimate;	
  int8_t RxPacketSnrEstimate;
  /************* ȡ���һ�����ݵ������ ************/        
  SX1276Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate ); /* ����� */ 
  if( rxSnrEstimate & 0x80 ) 
  { // The SNR sign bit is 1 /* �����С��0 ����� ����     */ 	
    RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2; /* Invert and divide by 4 */
    RxPacketSnrEstimate = -RxPacketSnrEstimate;
  } 
  else
  {                          /* ����ȴ���0 ����� ����   */ 
    RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;          /* Divide by 4 */
  }
  return RxPacketSnrEstimate;
}

//���һ�������ź�ǿ��  �ź�ǿ��Խ�� ��ֵԽ�� ���128dB 
uint8_t Sx1278_Lora_Get_Packet_RSSI( int8_t RxPacketSnrEstimate )
{  
    double RxPacketRssiValue;
    uint8_t RegPktRssiValue=0;
    uint16_t BW = 0;
    
    BW = Sx1278_LoRa_Get_SignalBandwidth()>>4;
    /* ���� ��P=-174(dBm) + BW(dB) + NF(dB) + SNR(dB);  ��������û�źŵ������ ���Ϲ�ʽ���� RSSI */
    if( RxPacketSnrEstimate < 0 ) /* �źű�������û */ 
    { 
      RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[ BW ] + NOISE_FIGURE_LF + ( double )RxPacketSnrEstimate;	
    } 
    else                           /* �ź�ǿ������  */ 
    { 
      SX1276Read( REG_LR_PKTRSSIVALUE, &RegPktRssiValue );
      RxPacketRssiValue = RssiOffsetLF[BW] + ( double )RegPktRssiValue;
    }
    BW = (uint8_t)(RxPacketRssiValue+256);
    if(BW >= 256)
      return(0);
    else
      return(BW);
}

uint8_t Sx1278_LoRa_Read_InterruptFlag(void)
{
  uint8_t data=0;
  
  SX1276Read( REG_LR_IRQFLAGS,&data);//��ȡ�����ж�״̬�Ĵ�����0x12Ϊ״̬�Ĵ�����ַ��
  return data;
}
/*
uint8_t flag ȡֵ
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 

Sx1278_LoRa_Reset_InterruptFlag��RFLR_IRQFLAGS_RXDONE���������������жϱ�־
*/
void Sx1278_LoRa_Reset_InterruptFlag(uint8_t flag)
{
  SX1276Write( REG_LR_IRQFLAGS, flag  );
}

//��ǰ�ź�ǿ�ȣ�
uint8_t Sx1278_LoRa_Read_Rssi( void )
{
  uint8_t RegRssiValue;
  //���� RSSI ֵ�� dBm��RSSI[dBm]=-137+Rssi
  SX1276Read( REG_LR_RSSIVALUE, &RegRssiValue );/* �Ĵ���0x1B ��ǰ�ź�ǿ��*/
  return(RegRssiValue);	
  /*�˴�ֻ�ܲ������뵽���ߵĹ��� �����źź����� 
    ����źŲ�һ�������ź�ǿ�ȿ���ͨ��snr����ԭ����֮�µ��ź�ǿ��*/ 
}

void Sx1278_LoRa_Set_StandbyMode(void)
{
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY ); 
}

void Sx1278_LoRa_Set_SleepMode(void)
{
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY );//0x80 �ǹ�����LORAģʽ��
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_SLEEP );  //0x80 �ǹ�����LORAģʽ��
}

void Sx1278_LoRa_Set_RxMode(uint8_t RxSingleOn)
{
  uint8_t RegIrqFlagsMask;
  
  Sx1278_LoRa_Set_StandbyMode();
  SX1276Write( REG_LR_PREAMBLEMSB,200);//ǰ����Ҫ��һ�� ���ڽ������ݵ�ǰ����
  SX1276Write( REG_LR_PREAMBLELSB,255);//ǰ����Ҫ��һ�� ���ڽ������ݵ�ǰ����
  RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                    //RFLR_IRQFLAGS_RXDONE |    /*  ??�䨰?a?����?����3��?a???D??     */ 
                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                    RFLR_IRQFLAGS_VALIDHEADER |
                    RFLR_IRQFLAGS_TXDONE |
                    RFLR_IRQFLAGS_CADDONE |
                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                    RFLR_IRQFLAGS_CADDETECTED;
  SX1276Write( REG_LR_IRQFLAGSMASK, RegIrqFlagsMask); 
  SX1276Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );//Rx Done ӳ�䵽 DIO0
  LoraRx_Enable();
  
  if( RxSingleOn == true )               // Rx single mode
  {
    SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_RECEIVER_SINGLE );
  }
  else                                  // Rx continuous mode
  {
    Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_RXDONE);
    SX1276Write( REG_LR_FIFOADDRPTR,0x00);                                           
    SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_RECEIVER ); //0x80 �ǹ�����LORAģʽ�� 
  }
}

//RegIrqFlagsMask�ж����� 
//CadDetected ӳ�䵽 DIO1
//CadDone     ӳ�䵽 DIO3
void Sx1278_LoRa_Set_Cad_Init(void)
{
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY ); 
  SX1276Write( REG_LR_IRQFLAGSMASK,~(RFLR_IRQFLAGS_CADDONE|RFLR_IRQFLAGS_CADDETECTED));//CadDetected ӳ�䵽 DIO1
  SX1276Write( REG_LR_DIOMAPPING1,RFLR_DIOMAPPING1_DIO3_00 | RFLR_DIOMAPPING1_DIO1_10);//CadDone     ӳ�䵽 DIO3
  Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_CADDETECTED);
  Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_CADDONE);
  SX1276Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_00|RFLR_DIOMAPPING2_DIO5_00 );//   
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_CAD ); 
}
//���15  ��Ӧ20db
void Sx1278_LoRa_Set_RFFrequency( uint32_t freq )
{
  freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
  SX1276Write( REG_LR_FRFMSB,( uint8_t )( ( freq >> 16 ) & 0xFF ));//Carrier Freq 470M
  SX1276Write( REG_LR_FRFMID,( uint8_t )( ( freq >> 8 ) & 0xFF ));
  SX1276Write( REG_LR_FRFLSB,( uint8_t )( freq & 0xFF ));
}
 
void Sx1278_LoRa_Set_RFPower( int8_t power )
{
  uint8_t RegPaConfig = 0 ,RegPaDac = 0;
  
  //OCP = PA ������������
  SX1276Write( REG_LR_OCP, 0x3f ); /*Ĭ�ϲ���PA������100ma�����20dBmʱ����Ҫ120ma ���Ա�������0x0b�Ĵ���*/
  Sx1278_LoRa_Set_PAOutput( RFLR_PACONFIG_PASELECT_PABOOST );//1->PA_BOOST ���š�������ʲ��ó���+20dBm
  Sx1278_LoRa_Set_Pa20dBm( true );
  
  Sx1278_LoRa_Set_StandbyMode(); 
  SX1276Read( REG_LR_PADAC,&RegPaDac );
  RegPaDac &= ~( ( 1<<2 ) | ( 1<<1 ) | ( 1<<0 ) );
  RegPaDac |= ( 0x07 << 0 );
  SX1276Write( REG_LR_PADAC, RegPaDac );
  
  SX1276Read( REG_LR_PACONFIG,&RegPaConfig );
  RegPaConfig |= ( 1<<7 );
  RegPaConfig &= ~( ( 1<<3 ) | ( 1<<2 ) | ( 1<<1 ) | ( 1<<0 ) );
  RegPaConfig |= ( power << 0 );
  SX1276Write( REG_LR_PACONFIG, RegPaConfig );
}

void Sx1278_LoRa_Set_SignalBandwidth( uint8_t bw )
{
  uint8_t RegModemConfig1 = 0;

  SX1276Read( REG_LR_MODEMCONFIG1, &RegModemConfig1 );
  RegModemConfig1 = ( (RegModemConfig1 & RFLR_MODEMCONFIG1_BW_MASK ) |  bw );
  SX1276Write( REG_LR_MODEMCONFIG1, RegModemConfig1 );

}

uint8_t Sx1278_LoRa_Get_SignalBandwidth( void )
{
  uint8_t RegModemConfig1 ;
  
  SX1276Read( REG_LR_MODEMCONFIG1, &RegModemConfig1 );
  return( RegModemConfig1 & ~RFLR_MODEMCONFIG1_BW_MASK );
}

void Sx1278_LoRa_Set_NbTrigPeaks( uint8_t value )
{
  uint8_t RegTestReserved31 = 0;

  SX1276Read( 0x31, &RegTestReserved31 );
  RegTestReserved31 = ( RegTestReserved31 & 0xF8 ) | value;
  SX1276Write( 0x31, RegTestReserved31 );
} 
 
void Sx1278_LoRa_Set_SpreadingFactor( SpreadingFactor_TypeDef factor )
{
  uint8_t RegModemConfig2 = 0;
  
  if( factor > SF12 )
  {
    factor = SF12;
  }
  else if( factor < SF06 )
  {
    factor = SF06;
  }
  
  if( factor == SF06 )
  {
    Sx1278_LoRa_Set_NbTrigPeaks( 5 );
  }
  else
  {
    Sx1278_LoRa_Set_NbTrigPeaks( 3 );
  }
  
  SX1276Read( REG_LR_MODEMCONFIG2, &RegModemConfig2 );   
  RegModemConfig2 = (( RegModemConfig2 & RFLR_MODEMCONFIG2_SF_MASK) | factor );
  SX1276Write( REG_LR_MODEMCONFIG2, RegModemConfig2 );     
}

void Sx1278_LoRa_Set_ErrorCoding( uint8_t value )
{
  uint8_t RegModemConfig1 = 0;
  
  SX1276Read( REG_LR_MODEMCONFIG1, &RegModemConfig1 );
  RegModemConfig1 = ( ( RegModemConfig1 & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | value );
  SX1276Write( REG_LR_MODEMCONFIG1, RegModemConfig1 );
  
}
 
void Sx1278_LoRa_Set_PacketCrcOn( bool enable )
{
  uint8_t RegModemConfig2 = 0;
  
  SX1276Read( REG_LR_MODEMCONFIG2, &RegModemConfig2 );
  RegModemConfig2 = ( RegModemConfig2 & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
  SX1276Write( REG_LR_MODEMCONFIG2, RegModemConfig2 );
}

void Sx1278_LoRa_Set_PreambleLength( uint16_t len )
{
  uint8_t RegPreambleMsb =0 , RegPreambleLsb = 0;

  RegPreambleMsb = ( len >> 8 ) & 0x00FF;
  RegPreambleLsb = len & 0xFF;
  SX1276Write( REG_LR_PREAMBLEMSB, (uint8_t)RegPreambleMsb );
  SX1276Write( REG_LR_PREAMBLELSB, (uint8_t)RegPreambleLsb );
}

void Sx1278_LoRa_Set_ImplicitHeaderOn( uint8_t enable )
{
  uint8_t RegModemConfig1 = 0;
  
  SX1276Read( REG_LR_MODEMCONFIG1, &RegModemConfig1 );
  RegModemConfig1 = ( RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );
  SX1276Write( REG_LR_MODEMCONFIG1, RegModemConfig1 );
}

void Sx1278_LoRa_Set_PayloadLength( uint8_t value )
{
  uint8_t RegPayloadLength = 0;

  RegPayloadLength = value;
  SX1276Write( REG_LR_PAYLOADLENGTH, RegPayloadLength );
}

void Sx1278_LoRa_Set_Pa20dBm( bool enale )
{
  uint8_t RegPaDac =0, RegPaConfig =0;
  
  SX1276Read( REG_LR_PADAC   , &RegPaDac );
  SX1276Read( REG_LR_PACONFIG, &RegPaConfig );
  
  if( ( RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
  {    
    if( enale == true )
    {
      RegPaDac = 0x87;
    }
  }
  else
  {
    RegPaDac = 0x84;
  }
  SX1276Write( REG_LR_PADAC, RegPaDac );  /*����PA_BOOST����*/
}

void Sx1278_LoRa_Set_PAOutput( uint8_t outputPin )
{
  uint8_t RegPaConfig =0;
  
  SX1276Read( REG_LR_PACONFIG, &RegPaConfig );
  RegPaConfig = (RegPaConfig & RFLR_PACONFIG_PASELECT_MASK ) | outputPin;
  SX1276Write( REG_LR_PACONFIG, RegPaConfig );
}

/*
��PA Ramp��ʱ�䣬����û�LDO���ܿ������������������������ʵ�����PA Rampʱ��
�����Rampʱ����̳�����LDO������ʱ������ֽ���TX��ϵͳ����Ϊ�������������RF�źŲ����ֵ�����
#define RFLR_PARAMP_3400_US                         0x00 
#define RFLR_PARAMP_2000_US                         0x01 
#define RFLR_PARAMP_1000_US                         0x02
#define RFLR_PARAMP_0500_US                         0x03 
#define RFLR_PARAMP_0250_US                         0x04 
#define RFLR_PARAMP_0125_US                         0x05 
#define RFLR_PARAMP_0100_US                         0x06 
#define RFLR_PARAMP_0062_US                         0x07 
#define RFLR_PARAMP_0050_US                         0x08 
#define RFLR_PARAMP_0040_US                         0x09 // Default
#define RFLR_PARAMP_0031_US                         0x0A 
#define RFLR_PARAMP_0025_US                         0x0B 
#define RFLR_PARAMP_0020_US                         0x0C 
#define RFLR_PARAMP_0015_US                         0x0D 
#define RFLR_PARAMP_0012_US                         0x0E 
#define RFLR_PARAMP_0010_US                         0x0F 
*/
void Sx1278_LoRa_Set_PaRamp( uint8_t value )
{
  uint8_t RegPaRamp =0 ;

  SX1276Read( REG_LR_PARAMP, &RegPaRamp );
  RegPaRamp = ( RegPaRamp & RFLR_PARAMP_MASK ) | ( value & ~RFLR_PARAMP_MASK );
  SX1276Write( REG_LR_PARAMP,RegPaRamp );
}

//���õ�һ����ģʽ�� �������ʱ��
void Sx1278_LoRa_Set_SymbTimeout( uint16_t value )
{
  uint8_t RegModemConfig2 =0;
  uint8_t RegSymbTimeoutLsb =0;

  SX1276Read( REG_LR_MODEMCONFIG2  ,&RegModemConfig2   );
  SX1276Read( REG_LR_SYMBTIMEOUTLSB,&RegSymbTimeoutLsb );
  
  RegModemConfig2   = ( RegModemConfig2 & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
  RegSymbTimeoutLsb = value & 0xFF;
  
  SX1276Write( REG_LR_MODEMCONFIG2  ,RegModemConfig2   );
  SX1276Write( REG_LR_SYMBTIMEOUTLSB,RegSymbTimeoutLsb );
}

void Sx1278_LoRa_Set_LowDatarateOptimize( bool enable )
{//���ų��ȳ��� 16msʱ����򿪵����Ż�
  uint8_t RegModemConfig3 = 0;
  
  SX1276Read( REG_LR_MODEMCONFIG3, &RegModemConfig3 );
  RegModemConfig3 = ( RegModemConfig3 & RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) | ( enable << 3 );
  SX1276Write( REG_LR_MODEMCONFIG3, RegModemConfig3 );
}

void Sx1278_LoRa_Set_AgcAutoOn(bool enable)
{
  uint8_t RegModemConfig3 = 0;
  
  SX1276Read( REG_LR_MODEMCONFIG3, &RegModemConfig3 );
  RegModemConfig3 = ((RegModemConfig3 & RFLR_MODEMCONFIG3_AGCAUTO_MASK ) | (enable << 2));
  SX1276Write( REG_LR_MODEMCONFIG3,RegModemConfig3);
}


