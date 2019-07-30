#include "includes.h"


/************************************************
** Function name:           N720_Mess_Q_Init
** Descriptions:            4G��Ϣ���г�ʼ�� 
** input parameters:         
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-9-30       
************************************************/
void N720_Mess_Q_Init(void) 
{
  queueCreate(N720MessageNum,300,0,0); 
  queueCreate(N720MessageQue,32768,0,0); 
}


/************************************************
** Function name:           Write_Uart_One
** Descriptions:            д����1���� 
** input parameters:        *data �� ��������  len�����ݳ���
** output parameters:       void
** Returned value:          void 
** Created by:              ������
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

















