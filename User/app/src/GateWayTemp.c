#include "includes.h"



  OS_STK AppTaskTempStk[APP_TASK_Temp_STK_SIZE];	




/*
*********************************************************************************************************
*	�� �� ��: AppTaskLED
*	����˵��: LED2��˸			  			  
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
void AppTaskTemperature(void *p_arg)
{
  (void)p_arg;		/* ����������澯 */
  int16_t tempmsg = 0;
  INT8U   err;
  
  while (1) 
  {
    OSTimeDlyHMSM(0, 0, 1, 0);	
    ADC_Cmd(ADC1, ENABLE);//ʹ��ָ����ADC1  
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    tempmsg = *(INT8U *)(OSMboxPend(MboxGateWayTemp, 1000 , &err));//�ȴ���Ϣ���� �¶���Ϣ
    //���С�� - 10�� �ʹ򿪼���
    if(err == OS_ERR_NONE)
    {
      if(tempmsg < -10)
      {
        tempmsg = 0;
      }
    }
    else if(err == OS_ERR_TIMEOUT)
    {
      tempmsg = 0;
    }

  }
}



