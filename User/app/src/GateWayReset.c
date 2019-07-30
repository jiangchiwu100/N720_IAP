#include "includes.h"



OS_STK AppTaskGateWayResetStk[APP_TASK_GateWayInit_STK_SIZE];



void AppTaskGateWayReset(void *p_arg)
{
  static Str_Calendar rtc;
  
  (void)p_arg;		/* ����������澯 */
  
  while (1) 
  {
    OSTimeDlyHMSM(0, 0, 1, 0);
    
    RS8025T_Get_Calendar_Time(&rtc);
    
    if((rtc.Hours == 0x23) ||(rtc.Minutes == 0x30))//������λ 
    {
      
      //ά��lora����
      //ά��4G����
      //ά���ļ�ϵͳ ������ֱ���
      //�����ļ�
      //�鿴�ļ�ϵͳ�Ƿ��ڹ��� 
      //�鿴���������Ƿ��ڹ��� 
      //����
      //��ɹ���֮����ȥ��λ
    }
    else if((rtc.Hours == 0x23) || (rtc.Minutes == 0x35))//����BEACON�·� 
    {
      OSTaskResume(APP_TASK_LoraBeacon_PRIO);            //����beacon���� 
    }
    else if((rtc.Hours == 0x00) && (rtc.Minutes == 0x00)) //0�� �����ֳ�����  
    {
      OSTaskResume(APP_TASK_LoraPing_PRIO);
      OSTaskResume(APP_TASK_LoraPolling_PRIO);
    } 
    else if((rtc.Hours == 0x04) && (rtc.Minutes == 0x00) )//�����㲥Уʱ����
    {
      
    }
    else if((rtc.Hours == 0x08) && (rtc.Minutes == 0x00) )//�����㲥Уʱ����
    {
      
    }
    else if((rtc.Hours == 0x12) && (rtc.Minutes == 0x00) )//�����㲥Уʱ����
    {
      
    }
    else if((rtc.Hours == 0x16) && (rtc.Minutes == 0x00) )//�����㲥Уʱ����
    {
      
    }
    else if((rtc.Hours == 0x20) && (rtc.Minutes == 0x00) )//�����㲥Уʱ����
    {
      
    }
    
    
    
    
  }
}