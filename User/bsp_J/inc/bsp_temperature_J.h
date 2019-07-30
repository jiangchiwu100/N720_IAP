#ifndef __bsp_temperature_J_H
#define __bsp_temperature_J_H

#ifdef __cplusplus
extern "C" {
#endif 

 #define ADC1_DR_Address    ((uint32_t)0x4001244C)
  

  

void bsp_InitTemperature_J(void);
void Temp_DMA_Config(void)  ;
void Temp_ADC_Config(void)  ;
  
  
  
  
  
  
  

#ifdef __cplusplus
}
#endif
#endif