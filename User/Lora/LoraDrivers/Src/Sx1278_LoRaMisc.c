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
  
  SX1276Write( REG_LR_HOPPERIOD,0x91 );//选一个用不到的寄存器来做验证
  SX1276Read( REG_LR_HOPPERIOD,&test);
  
  if(test!=0x91)
    return false;    
  else
    return true;
}

void Sx1278_Lora_Set_On(void)
{
  uint8_t RegOpMode = 0;
  
//  Sx1278_LoRa_Set_SleepMode();//lora模式仅在睡眠模式下可以修改
  SX1276Read( REG_LR_OPMODE,&RegOpMode );
  RegOpMode = ( RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
  SX1276Write( REG_LR_OPMODE, RegOpMode );
}

// 0 : 没有增益 1： 最大增益 6： 最小增益
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
  /************* 取最后一包数据的信噪比 ************/        
  SX1276Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate ); /* 信噪比 */ 
  if( rxSnrEstimate & 0x80 ) 
  { // The SNR sign bit is 1 /* 信噪比小于0 的情况 负数     */ 	
    RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2; /* Invert and divide by 4 */
    RxPacketSnrEstimate = -RxPacketSnrEstimate;
  } 
  else
  {                          /* 信噪比大于0 的情况 正数   */ 
    RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;          /* Divide by 4 */
  }
  return RxPacketSnrEstimate;
}

//最后一包数据信号强度  信号强度越大 数值越大 最大128dB 
uint8_t Sx1278_Lora_Get_Packet_RSSI( int8_t RxPacketSnrEstimate )
{  
    double RxPacketRssiValue;
    uint8_t RegPktRssiValue=0;
    uint16_t BW = 0;
    
    BW = Sx1278_LoRa_Get_SignalBandwidth()>>4;
    /* 功率 ：P=-174(dBm) + BW(dB) + NF(dB) + SNR(dB);  在噪声淹没信号的情况下 以上公式反推 RSSI */
    if( RxPacketSnrEstimate < 0 ) /* 信号被噪声淹没 */ 
    { 
      RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[ BW ] + NOISE_FIGURE_LF + ( double )RxPacketSnrEstimate;	
    } 
    else                           /* 信号强于噪声  */ 
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
  
  SX1276Read( REG_LR_IRQFLAGS,&data);//读取无线中断状态寄存器，0x12为状态寄存器地址。
  return data;
}
/*
uint8_t flag 取值
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 

Sx1278_LoRa_Reset_InterruptFlag（RFLR_IRQFLAGS_RXDONE）；清除接收完成中断标志
*/
void Sx1278_LoRa_Reset_InterruptFlag(uint8_t flag)
{
  SX1276Write( REG_LR_IRQFLAGS, flag  );
}

//当前信号强度，
uint8_t Sx1278_LoRa_Read_Rssi( void )
{
  uint8_t RegRssiValue;
  //电流 RSSI 值（ dBm）RSSI[dBm]=-137+Rssi
  SX1276Read( REG_LR_RSSIVALUE, &RegRssiValue );/* 寄存器0x1B 当前信号强度*/
  return(RegRssiValue);	
  /*此处只能测量输入到天线的功率 包括信号和噪声 
    与包信号不一样，包信号强度可以通过snr来还原噪声之下的信号强度*/ 
}

void Sx1278_LoRa_Set_StandbyMode(void)
{
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY ); 
}

void Sx1278_LoRa_Set_SleepMode(void)
{
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY );//0x80 是工作在LORA模式下
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_SLEEP );  //0x80 是工作在LORA模式下
}

void Sx1278_LoRa_Set_RxMode(uint8_t RxSingleOn)
{
  uint8_t RegIrqFlagsMask;
  
  Sx1278_LoRa_Set_StandbyMode();
  SX1276Write( REG_LR_PREAMBLEMSB,200);//前导吗要长一点 大于接收数据的前导吗
  SX1276Write( REG_LR_PREAMBLELSB,255);//前导吗要长一点 大于接收数据的前导吗
  RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                    //RFLR_IRQFLAGS_RXDONE |    /*  ??′ò?a?óê?íê3é?a???D??     */ 
                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                    RFLR_IRQFLAGS_VALIDHEADER |
                    RFLR_IRQFLAGS_TXDONE |
                    RFLR_IRQFLAGS_CADDONE |
                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                    RFLR_IRQFLAGS_CADDETECTED;
  SX1276Write( REG_LR_IRQFLAGSMASK, RegIrqFlagsMask); 
  SX1276Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );//Rx Done 映射到 DIO0
  LoraRx_Enable();
  
  if( RxSingleOn == true )               // Rx single mode
  {
    SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_RECEIVER_SINGLE );
  }
  else                                  // Rx continuous mode
  {
    Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_RXDONE);
    SX1276Write( REG_LR_FIFOADDRPTR,0x00);                                           
    SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_RECEIVER ); //0x80 是工作在LORA模式下 
  }
}

//RegIrqFlagsMask中断屏蔽 
//CadDetected 映射到 DIO1
//CadDone     映射到 DIO3
void Sx1278_LoRa_Set_Cad_Init(void)
{
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY ); 
  SX1276Write( REG_LR_IRQFLAGSMASK,~(RFLR_IRQFLAGS_CADDONE|RFLR_IRQFLAGS_CADDETECTED));//CadDetected 映射到 DIO1
  SX1276Write( REG_LR_DIOMAPPING1,RFLR_DIOMAPPING1_DIO3_00 | RFLR_DIOMAPPING1_DIO1_10);//CadDone     映射到 DIO3
  Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_CADDETECTED);
  Sx1278_LoRa_Reset_InterruptFlag(RFLR_IRQFLAGS_CADDONE);
  SX1276Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_00|RFLR_DIOMAPPING2_DIO5_00 );//   
  SX1276Write( REG_LR_OPMODE, 0x80|RFLR_OPMODE_CAD ); 
}
//最大15  对应20db
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
  
  //OCP = PA 开启过流保护
  SX1276Write( REG_LR_OCP, 0x3f ); /*默认参数PA最大电流100ma，输出20dBm时，需要120ma 所以必须配置0x0b寄存器*/
  Sx1278_LoRa_Set_PAOutput( RFLR_PACONFIG_PASELECT_PABOOST );//1->PA_BOOST 引脚。输出功率不得超过+20dBm
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
  SX1276Write( REG_LR_PADAC, RegPaDac );  /*设置PA_BOOST功率*/
}

void Sx1278_LoRa_Set_PAOutput( uint8_t outputPin )
{
  uint8_t RegPaConfig =0;
  
  SX1276Read( REG_LR_PACONFIG, &RegPaConfig );
  RegPaConfig = (RegPaConfig & RFLR_PACONFIG_PASELECT_MASK ) | outputPin;
  SX1276Write( REG_LR_PACONFIG, RegPaConfig );
}

/*
↑PA Ramp的时间，如果用户LDO不能快速输出大电流（泵能力），适当增加PA Ramp时间
↑如果Ramp时间过短超过了LDO的能力时，会出现进入TX后，系统电流为发射电流，但是RF信号不出现的现象
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

//设置单一接收模式下 接收溢出时间
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
{//符号长度超过 16ms时必须打开低速优化
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


