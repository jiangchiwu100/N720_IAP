#include "includes.h"



  OS_STK AppTaskTempStk[APP_TASK_Temp_STK_SIZE];	




/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED2闪烁			  			  
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
void AppTaskTemperature(void *p_arg)
{
  (void)p_arg;		/* 避免编译器告警 */
  int16_t tempmsg = 0;
  INT8U   err;
  
  while (1) 
  {
    OSTimeDlyHMSM(0, 0, 1, 0);	
    ADC_Cmd(ADC1, ENABLE);//使能指定的ADC1  
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    tempmsg = *(INT8U *)(OSMboxPend(MboxGateWayTemp, 1000 , &err));//等待消息邮箱 温度信息
    //如果小于 - 10℃ 就打开加热
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



