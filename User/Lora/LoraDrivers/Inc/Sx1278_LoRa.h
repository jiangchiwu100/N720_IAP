#ifndef __Sx1278_LoRa_H
#define __Sx1278_LoRa_H


#ifdef __cplusplus
extern "C" {
#endif 

/*!
 * SX1276 LoRa General parameters definition
 */
  
 
typedef union 
{
  uint8_t LoRaSettingsBuf[18];
  struct 
  {
    uint32_t                    RFFrequency;    //频率
    uint8_t                     Power;          //功率
    BandWidth_TypeDef           SignalBw;       // LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                                                // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]  
    SpreadingFactor_TypeDef     SpreadingFactor; 
    CodingRate_TypeDef          ErrorCoding;    // LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    uint8_t                     CrcOn;          // [0: OFF, 1: ON]
    uint8_t                     ImplicitHeaderOn; // 隐式报头模式[0: OFF, 1: ON]
    uint8_t                     RxSingleOn;     // [0: Continuous, 1 Single]
    uint16_t                    PayloadLength;  // 
    uint8_t                     HopPeriod;      // Hops every frequency hopping period symbols
    float                      SymbTime;       //码元时间 
  }Str_LoRaSettings ;
}Union_LoRaSettings ;

extern Union_LoRaSettings LoRaSettings;




void Lora_Rx_Init(void);


void Sx1278_Init(void);
void Sx1278_Lora_Init( Union_LoRaSettings *settings );
float Sx1278_Calculated_SymbTime(Union_LoRaSettings *setting);
uint16_t Sx1278_Calculated_Cad_Preamble(float time);
void Sx1278_LoRa_Interrupt_Process_DIO0(void);
void Sx1278_LoRa_Interrupt_Process_DIO1(void);
void Sx1278_LoRa_Interrupt_Process_DIO3(void);
void Sx1278_Lora_Tx_Data(uint8_t *PayloadBuf, uint8_t Len,uint16_t PreambleNum);
void Sx1278_LoRa_Get_Rx_Data( uint8_t *pbuf, uint8_t *size );






  #ifdef __cplusplus
}
#endif
#endif