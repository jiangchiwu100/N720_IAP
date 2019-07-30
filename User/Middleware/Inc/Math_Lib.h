#ifndef __Math_Lib_H
#define __Math_Lib_H


#ifdef __cplusplus
extern "C" {
#endif 


  
#include "stdint.h"
#include "stdbool.h"
  //大小端转换宏定义
 #define EndianConversion_32Bit(x) (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))   
 #define EndianConversion_16Bit(x) ((short)((((short)(x) & (short)0x00ffU) << 8 ) |(((short)(x) & (short)0xff00U) >> 8 ) ))
  
#define BIT0    (0x0001u)
#define BIT1    (0x0002u)
#define BIT2    (0x0004u)
#define BIT3    (0x0008u)
#define BIT4    (0x0010u)
#define BIT5    (0x0020u)
#define BIT6    (0x0040u)
#define BIT7    (0x0080u)
#define BIT8    (0x0100u)
#define BIT9    (0x0200u)
#define BITA    (0x0400u)
#define BITB    (0x0800u)
#define BITC    (0x1000u)
#define BITD    (0x2000u)
#define BITE    (0x4000u)
#define BITF    (0x8000u)

#define BIT16   (0x00000001<<16)
#define BIT17   (0x00000001<<17)
#define BIT18   (0x00000001<<18)
#define BIT19   (0x00000001<<19)
#define BIT20   (0x00000001<<20)
#define BIT21   (0x00000001<<21)
#define BIT22   (0x00000001<<22)
#define BIT23   (0x00000001<<23)
#define BIT24   (0x00000001<<24)
#define BIT25   (0x00000001<<25)
#define BIT26   (0x00000001<<26)
#define BIT27   (0x00000001<<27)
#define BIT28   (0x00000001<<28)
#define BIT29   (0x00000001<<29)
#define BIT30   (0x00000001<<30)
#define BIT31   (0x00000001<<31)

  
#define B0000_0000      0x00
#define B0000_0001      0x01
#define B0000_0010      0x02
#define B0000_0011      0x03
#define B0000_0100      0x04
#define B0000_1000      0x08
#define B0001_0000      0x10
#define B0010_0000      0x20 
#define B0100_0000      0x40 
#define B1000_0000      0x80 
 
void SysCtlDelay(unsigned long ulCount);

void Delay_us(uint32_t dat);
void Delay_ms(uint16_t dat);
char Hex_To_Ascii(uint8_t dat);
uint8_t Ascii_To_Hex(char uchar);
void Random_Seed(void);
uint8_t Count_CheckSum(uint8_t *dat,uint8_t len);
uint32_t Bin_To_Bcd_Bit32(uint32_t dec_data);
uint32_t Bcd_To_Bin_Bit32(uint32_t hex_data);
uint8_t Bin_To_Bcd_Bit8(uint8_t value);
uint8_t Bcd_To_Bin_Bit8(uint8_t value);
uint32_t Glide_Average_Filter(uint32_t *point,uint32_t data,uint32_t num,uint32_t *sum_point);
bool _memcmp( void *dest, const uint8_t src, uint8_t count);
  
    #ifdef __cplusplus
}
#endif
#endif
