#ifndef __ListenToBeacon_H
#define __ListenToBeacon_H

#ifdef __cplusplus
extern "C" {
#endif 
 
  
  
#define FirstTime  0x01
#define SecondTime 0x02
  
  
/*******************************************************************************
  �ŵ��������� ����״̬��
******************************************************************************/
  typedef enum 
  {  
    ListenToBeacon_Enable  ,//ʹ������beacon
    ListenToBeacon_Rx      ,//����Beacon
  }ENUM_ListenToBeacon_Status;

extern SCH_TCB ListenToBeaconTaskTcb;
 

 
  
void Listen_To_Beacon_Task(void) ;
uint16_t Listen_To_Beacon(void);
 
  
  
  
  
  
#ifdef __cplusplus
}
#endif
#endif