#include "includes.h"


/************************************************
** Function name:           N720_Mess_Q_Init
** Descriptions:            4G消息队列初始化 
** input parameters:         
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-9-30       
************************************************/
void N720_Mess_Q_Init(void) 
{
  queueCreate(N720MessageNum,300,0,0); 
  queueCreate(N720MessageQue,32768,0,0); 
}


/************************************************
** Function name:           Write_Uart_One
** Descriptions:            写串口1数据 
** input parameters:        *data ： 数据数组  len：数据长度
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-9-30       
************************************************/
uint8_t Insert_N720_Message_Q(uint8_t *dat,uint8_t len)
{  
  if(len>0 && len<0xff)
  {
    for(uint8_t num=0;num<len;num++)
    {
      queueWrite(N720MessageQue,*(dat+num));
    }
    queueWrite(N720MessageNum,len);
    return(true);
  }
  else
  {
    return(false);
  }
}


uint8_t Get_N720_Message_Q_Num(void)
{
  return(queueNData(N720MessageNum));
}


uint8_t Get_N720_Message_Q_Dat(uint8_t *buf)
{
  uint8_t len  = 0;
  uint8_t data = 0;
  
  if(queueNData(N720MessageNum) > 0)
  {
    queueRead(N720MessageNum,&len);
    
    for(uint16_t num=0;num<len;num++)
    {
      queueRead(N720MessageQue,&data);
      *(buf+num) = data;  
    }
  }
 return(len);
}

















