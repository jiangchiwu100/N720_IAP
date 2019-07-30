
#ifndef __N720MessageQueues_H
#define __N720MessageQueues_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
  
  
void N720_Mess_Q_Init(void);//4G上行消息队列初始化 
uint8_t Insert_N720_Message_Q(uint8_t *dat,uint8_t len);
uint8_t Get_N720_Message_Q_Dat(uint8_t *buf);
uint8_t Get_N720_Message_Q_Num(void);
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
 

#ifdef __cplusplus
}
#endif
#endif
