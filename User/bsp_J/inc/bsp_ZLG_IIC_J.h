#ifndef __bsp_ZLG_IIC_J_H
#define __bsp_ZLG_IIC_J_H

#ifdef __cplusplus
extern "C" {
#endif 
  

  


  
#define SCL_Port        GPIOE  
#define SCL_Pin         GPIO_Pin_2  
#define SCL_HIGH()      GPIO_SetBits(SCL_Port, SCL_Pin)
#define SCL_LOW()       GPIO_ResetBits(SCL_Port, SCL_Pin)
#define SDA_Port        GPIOF  
#define SDA_Pin         GPIO_Pin_10 
#define SDA_HIGH()      GPIO_SetBits(SDA_Port, SDA_Pin)
#define SDA_LOW()       GPIO_ResetBits(SDA_Port, SDA_Pin)
#define ReadSDA()	GPIO_ReadInputDataBit(SDA_Port,SDA_Pin) 
  
  
  void IIC_GPIO_Init(void);
  void Start_IIc(void);
  void Stop_IIc(void);
  bool  SendByte(uint8_t c);
  uint8_t  RcvByte(void);
  void Ack_IIc(const bool ack);
  bool ISendByte(uint8_t sla,uint8_t c);
  bool ISendStr(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t no) ;
  bool IRcvByte(uint8_t sla,uint8_t *c);
  bool IRcvStr(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t no);
  
#ifdef __cplusplus
}
#endif
#endif
