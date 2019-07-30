#include "includes.h"



OS_STK AppTaskGateWayInitStk[APP_TASK_GateWayInit_STK_SIZE];	


/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
  printf("\r\n------------\r\n");
  printf("GateWay Disp Menu\r\n");
  printf("GateWay Init\r\n");
  printf("Disk Format:\r\n");
  printf("GateWay ConfigInfo?\r\n");
  printf("GateWay RTC:xxxx,xx,xx,xx,xx,xx\r\n");
  printf("GateWay RTC?\r\n");
  printf("File System Menu?\r\n");
  printf("GateWay Task Info?\r\n");
  printf("Set Lora Net Info:ChannleNum xx,FatherID xx,SOF xx;\r\n");//Set Lora Net Info:ChannleNum 01,FatherID 13,SOF 68;
  printf("Lora Net Info?\r\n");
}

 /*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-II任务信息打印到串口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
  OS_TCB      *ptcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
  OS_STK_DATA stk;	        /* 用于读取任务栈使用情况 */
  
  ptcb    = &OSTCBTbl[0];		/* 指向第1个任务控制块(TCB) */
  
  /* 打印标题 */
  printf("==================================================\r\n");
  printf("  优先级   使用栈  剩余栈  百分比   任务名\r\n");
  printf("  Prio     Used    Free    Per      Taskname\r\n");
  
  OSTimeDly(10);
  
  /* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
  while (ptcb != NULL)
  {
    /* 
    ptcb->OSTCBPrio : 任务控制块中保存的任务的优先级
    ptcb->OSTCBTaskName : 任务控制块中保存的任务名称，需要在创建任务的时候
    调用OSTaskNameSet()指定任务名称，比如：
    OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
    */
    OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* 获得任务栈已用空间 */
    printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
           stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
           ptcb->OSTCBTaskName);		
    ptcb = ptcb->OSTCBPrev;		            /* 指向上一个任务控制块 */
  }
}

 
/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED2闪烁			  			  
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
void AppTaskGateWayInit(void *p_arg)
{
  static INT8U   err;
  static uint8_t tem = 0;
  static uint8_t ucRxBuf[256] ={0} ;
  static char *addr;
  static Str_Calendar rtc;
  
  
  (void)p_arg;		/* 避免编译器告警 */
  
  while (1) 
  {
    OSSemPend(SempDEBUGRx, 0, &err);
    
    comGetBuf(COM_DEBUG, ucRxBuf);
    if(strstr(ucRxBuf,"GateWay Disp Menu") != NULL)//打印目录
    {
      DispMenu();
    }
    else if(strstr(ucRxBuf,"GateWay Init") != NULL)//磁盘初始化
    {
      DiskFormatting();
      CreateGateWayMeterLibFile();
	  CreateGateWayConfigInfoFile();
    }
	else if(strstr(ucRxBuf,"GateWay ConfigInfo?") != NULL)
	{
				
		
	}
    else if(strstr(ucRxBuf,"GateWay RTC:") != NULL)//设置RTC 
    {//GateWay RTC:2018,12,30,11,37,00
      addr = strstr(ucRxBuf,"GateWay RTC:");
      if(addr != NULL)
      {
        tem = Ascii_To_Hex(*(addr+ 14 ));
        tem = tem<<4;
        tem +=Ascii_To_Hex(*(addr+ 15));
        rtc.Year = tem;
        
        tem = Ascii_To_Hex(*(addr+ 17 ));
        tem = tem<<4;
        tem +=Ascii_To_Hex(*(addr+ 18));
        rtc.Month = tem;
        
        tem = Ascii_To_Hex(*(addr+ 20 ));
        tem = tem<<4;
        tem +=Ascii_To_Hex(*(addr+ 21));
        rtc.DayOfMonth = tem;
        
        tem = Ascii_To_Hex(*(addr+ 23 ));
        tem = tem<<4;
        tem +=Ascii_To_Hex(*(addr+ 24));
        rtc.Hours = tem;
        
        tem = Ascii_To_Hex(*(addr+ 26 ));
        tem = tem<<4;
        tem +=Ascii_To_Hex(*(addr+ 27));
        rtc.Minutes = tem;
        
        tem = Ascii_To_Hex(*(addr+ 29 ));
        tem = tem<<4;
        tem +=Ascii_To_Hex(*(addr+ 30));
        rtc.Seconds = tem;
        
        RS8025T_Init_Calendar_Time(&rtc);
        printf(" ********已写入8025T*******\n\r");
      }
    }
    else if(strstr(ucRxBuf,"GateWay RTC?") != NULL)//读取RTC
    {
      RS8025T_Get_Calendar_Time(&rtc);
      
      printf ("20%x年",rtc.Year);
      printf ("%x月",rtc.Month);
      printf ("%x日",rtc.DayOfMonth);
      printf ("%x时",rtc.Hours);
      printf ("%x分",rtc.Minutes);
      printf ("%x秒",rtc.Seconds);
      printf("\r\n");
    }
    else if(strstr(ucRxBuf,"File System Menu?") != NULL)//读取文件目录
    {
      ViewRootDir();
    }
    else if(strstr(ucRxBuf,"GateWay Task Info?") != NULL)
    {
      DispTaskInfo();
    }
    else if(strstr(ucRxBuf,"Disk Format:") != NULL)
    {
        
    }
    else if(strstr(ucRxBuf,"Add Lib1:") != NULL)
    { 
      uint8_t id[]  = {0x78,0x56,0x34,0x12,0,0};
         uint16_t stn  = 0xffff;
      uint8_t dat[] = {0x68,0x17,0x00,0x10,0x52,0x00,0x00,0x68,0x01,0x02,0x34,0x23,0xA3,0x16};
      
//      if( Seek_Lib_ID(&id[0],&stn) == false )
//      {
//        if( Insert_Meter_Lib(&id[0],&stn) == true)
//        {
//          printf("插入表库成功 (%d)\r\n", stn);
          if(Insert_Meter_Lib_Data(&id[0],&dat[0],sizeof(dat)) == true)
          {
            printf("插入下发数据成功\n\r");
          }
          else
          {
            printf("插入下发数据失败\n\r");
          }
//        }
//        else
//        {
//          printf("插入表库失败\r\n");
//        }
//      }
//      else
//      {
//        printf("插入表库失败\r\n");
//      }

    }
    else if(strstr(ucRxBuf,"Add Lib2:") != NULL)
    { 
      uint8_t id[]  = {0x79,0x56,0x34,0x12,0,0};
         uint16_t stn  = 0xffff;
      uint8_t dat[] = {0x68,0x17,0x00,0x10,0x52,0x00,0x00,0x68,0x01,0x02,0x34,0x23,0xA3,0x16};
      
      if( Seek_Lib_ID(&id[0],&stn) == false )
      {
        if( Insert_Meter_Lib(&id[0],&stn) == true)
        {
          printf("插入表库成功 (%d)\r\n", stn);
          if(Insert_Meter_Lib_Data(&id[0],&dat[0],sizeof(dat)) == true)
          {
            printf("插入下发数据成功\n\r");
          }
          else
          {
            printf("插入下发数据失败\n\r");
          }
        }
        else
        {
          printf("插入表库失败\r\n");
        }
      }
      else
      {
        printf("插入表库失败\r\n");
      }
      
//      
      
    }
  }

}