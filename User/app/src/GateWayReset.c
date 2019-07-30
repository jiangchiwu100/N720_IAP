#include "includes.h"



OS_STK AppTaskGateWayResetStk[APP_TASK_GateWayInit_STK_SIZE];



void AppTaskGateWayReset(void *p_arg)
{
  static Str_Calendar rtc;
  
  (void)p_arg;		/* 避免编译器告警 */
  
  while (1) 
  {
    OSTimeDlyHMSM(0, 0, 1, 0);
    
    RS8025T_Get_Calendar_Time(&rtc);
    
    if((rtc.Hours == 0x23) ||(rtc.Minutes == 0x30))//启动复位 
    {
      
      //维护lora网络
      //维护4G网络
      //维护文件系统 保存各种变量
      //保存文件
      //查看文件系统是否在工作 
      //查看其它外设是否在工作 
      //保存
      //完成工作之后再去复位
    }
    else if((rtc.Hours == 0x23) || (rtc.Minutes == 0x35))//启动BEACON下发 
    {
      OSTaskResume(APP_TASK_LoraBeacon_PRIO);            //启动beacon任务 
    }
    else if((rtc.Hours == 0x00) && (rtc.Minutes == 0x00)) //0点 启动轮抄任务  
    {
      OSTaskResume(APP_TASK_LoraPing_PRIO);
      OSTaskResume(APP_TASK_LoraPolling_PRIO);
    } 
    else if((rtc.Hours == 0x04) && (rtc.Minutes == 0x00) )//启动广播校时任务
    {
      
    }
    else if((rtc.Hours == 0x08) && (rtc.Minutes == 0x00) )//启动广播校时任务
    {
      
    }
    else if((rtc.Hours == 0x12) && (rtc.Minutes == 0x00) )//启动广播校时任务
    {
      
    }
    else if((rtc.Hours == 0x16) && (rtc.Minutes == 0x00) )//启动广播校时任务
    {
      
    }
    else if((rtc.Hours == 0x20) && (rtc.Minutes == 0x00) )//启动广播校时任务
    {
      
    }
    
    
    
    
  }
}