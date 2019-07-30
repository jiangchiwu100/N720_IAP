#ifndef __bsp_EEPROM_J_H
#define __bsp_EEPROM_J_H


#ifdef __cplusplus
extern "C" {
#endif 

 
#define DeviceAddrLow   0xA0  //小于等于 0xFFFF 时的器件地址        
#define DeviceAddrHigh  0xA2  //大于     0xFFFF 时的器件地址 

#define EEPROM_M24C02 1
#define EEPROM_M24C16 0
#define EEPROM_M24M01 0
  
  
#if  EEPROM_M24C16 
  
#define Eeprom_Max_Page_Size 16    //每页地址数
#define Eeprom_Max_Rom_Size  2048  //最大地址数
 
#elif EEPROM_M24C02 
  
#define Eeprom_Max_Page_Size 16    //每页地址数
#define Eeprom_Max_Rom_Size  256  //最大地址数
  
#elif EEPROM_M24M01
  
  
  
  
  
#endif
  
void EEPROM_Init(void);

bool EEPROM_Array_Write(uint16_t addr,uint8_t *buff,uint16_t lengh);
bool EEPROM_Array_Read(uint16_t addr,uint8_t *buff,uint16_t lengh);

bool M24C02_Page_Write(uint16_t addr,uint8_t *buff,uint8_t lengh);
bool M24C02_Array_Read(uint16_t addr,uint8_t *buff,uint8_t lengh);

bool M24C16_Page_Write(uint16_t addr,uint8_t *buff,uint8_t lengh);
bool M24C16_Array_Read(uint16_t addr,uint8_t *buff,uint8_t lengh);
  
bool M24M01_Page_Write(uint32_t addr,uint8_t *buff,uint16_t lengh);
bool M24M01_Array_Read(uint32_t addr,uint8_t *buff,uint16_t lengh);



  
    #ifdef __cplusplus
}
#endif
#endif



