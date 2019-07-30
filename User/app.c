/*
*********************************************************************************************************
*	                                  
 
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*


OSTaskSuspend(TASK1_PRIO);//挂起任务 优先级
OSTaskResume(TASK1_PRIO);//恢复任务 优先级 

*********************************************************************************************************
*/
#include "includes.h"


/*
********************************************************************************************************
*                              任务堆栈
********************************************************************************************************
*/

/* 定义每个任务的堆栈空间，app_cfg.h文件中宏定义栈大小 */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

 
			
/*
*******************************************************************************************************
*                              函数声明
*******************************************************************************************************
*/

 
static void AppTaskStart(void *p_arg);
 
//static void DispTaskInfo(void);
 /*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/

/*创建一个信号量*/
OS_EVENT *SempN720Rx;
OS_EVENT *SempDEBUGRx;

OS_EVENT *SempTCPRx;//TCP 接收到的数据
OS_EVENT *SempSMSRx;//SMS 接收到的数据

OS_EVENT *SempLoraRx;
OS_EVENT *SempLoraTx;

/* 定义一个邮箱， 这只是一个邮箱指针， OSMboxCreate函数会创建邮箱必需的资源 */
OS_EVENT *MboxGateWayTemp;//用于
OS_EVENT *MboxLoraPing;
/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{ 
  printf("********************测试打印输出*********************\n\r");
} 
/*
*******************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
int main(void)
{
  INT8U  err;
  
  /* 初始化"uC/OS-II"内核 */
  OSInit();
  
  /* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
  OSTaskCreateExt(AppTaskStart,	/* 启动任务函数指针 */
                  (void *)0,		/* 传递给任务的参数 */
                  (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* 指向任务栈栈顶的指针 */
                  APP_TASK_START_PRIO,	/* 任务的优先级，必须唯一，数字越低优先级越高 */
                  APP_TASK_START_PRIO,	/* 任务ID，一般和任务优先级相同 */
                  (OS_STK *)&AppTaskStartStk[0],/* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                  APP_TASK_START_STK_SIZE, /* 任务栈大小 */
                  (void *)0,	/* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                  （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
  
  /*  定义如下：
  OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
  OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
  OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
  */                  
  
  /* 指定任务的名称，用于调试。这个函数是可选的 */
  OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);
  
  /*ucosII的节拍计数器清0    节拍计数器是0-4294967295*/ 
  OSTimeSet(0);	
  
  /* 启动多任务系统，控制权交给uC/OS-II */
  OSStart();
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{	
  /* 仅用于避免编译器告警，编译器不会产生任何目标代码 */	
  (void)p_arg;
  INT8U  err;
  /* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
  bsp_Init();
  CPU_Init();          
  BSP_Tick_Init();   
  
  /* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率 */
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();
#endif	
  
  /* 创建信号量 */
  SempN720Rx  = OSSemCreate(0);	  /* 创建一个信号量 实现同步功能 */
  SempDEBUGRx = OSSemCreate(0);	  /* 创建一个信号量 实现同步功能 */
  
  SempLoraRx  = OSSemCreate(0);	  /* 创建一个信号量 实现同步功能 */
  SempLoraTx  = OSSemCreate(0);	  /* 创建一个信号量 实现同步功能 */
  
  SempTCPRx   = OSSemCreate(0);	  /* 创建一个信号量 实现同步功能 */
  SempSMSRx   = OSSemCreate(0);	  /* 创建一个信号量 实现同步功能 */

  /* 创建邮箱(MBOX) */
  MboxGateWayTemp = OSMboxCreate((void *)0);
  MboxLoraPing    = OSMboxCreate((void *)0);/* 创建一个信号量 实现同步功能 */
  
  
  /* 创建应用程序的任务 */
  OSTaskCreateExt(AppTaskLoraBeacon,
                  (void *)0,
                  (OS_STK *)&AppTaskLoraBeaconStk[APP_TASK_LoraBeacon_STK_SIZE - 1],
                  APP_TASK_LoraBeacon_PRIO,
                  APP_TASK_LoraBeacon_PRIO,
                  (OS_STK *)&AppTaskLoraBeaconStk[0],
                  APP_TASK_LoraBeacon_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_LoraBeacon_PRIO, APP_TASK_LoraBeacon_NAME, &err);
  OSTaskSuspend(APP_TASK_LoraBeacon_PRIO);
  
  OSTaskCreateExt(AppTaskLoraPing,
                  (void *)0,
                  (OS_STK *)&AppTaskLoraPingStk[APP_TASK_LoraPing_STK_SIZE - 1],
                  APP_TASK_LoraPing_PRIO,
                  APP_TASK_LoraPing_PRIO,
                  (OS_STK *)&AppTaskLoraPingStk[0],
                  APP_TASK_LoraPing_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_LoraPing_PRIO, APP_TASK_LoraPing_NAME, &err);
//  OSTaskSuspend(APP_TASK_LoraPing_PRIO);
  
  OSTaskCreateExt(AppTaskLoraPolling,
                  (void *)0,
                  (OS_STK *)&AppTaskLoraPollingStk[APP_TASK_LoraPolling_STK_SIZE - 1],
                  APP_TASK_LoraPolling_PRIO,
                  APP_TASK_LoraPolling_PRIO,
                  (OS_STK *)&AppTaskLoraPollingStk[0],
                  APP_TASK_LoraPolling_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_LoraPolling_PRIO, APP_TASK_LoraPolling_NAME, &err);
//  OSTaskSuspend(APP_TASK_LoraPolling_PRIO);
//  
//  OSTaskCreateExt(AppTaskTemperature,
//                  (void *)0,
//                  (OS_STK *)&AppTaskTempStk[APP_TASK_Temp_STK_SIZE - 1],
//                  APP_TASK_Temp_PRIO,
//                  APP_TASK_Temp_PRIO,
//                  (OS_STK *)&AppTaskTempStk[0],
//                  APP_TASK_Temp_STK_SIZE,
//                  (void *)0,
//                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
//  
//  OSTaskNameSet(APP_TASK_Temp_PRIO, APP_TASK_Temp_NAME, &err);
  OSTaskCreateExt(AppTaskGateWayInit,
                  (void *)0,
                  (OS_STK *)&AppTaskGateWayInitStk[APP_TASK_GateWayInit_STK_SIZE - 1],
                  APP_TASK_GateWayInit_PRIO,
                  APP_TASK_GateWayInit_PRIO,
                  (OS_STK *)&AppTaskGateWayInitStk[0],
                  APP_TASK_GateWayInit_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_GateWayInit_PRIO, APP_TASK_GateWayInit_NAME, &err);
//  OSTaskCreateExt(AppTaskGateWayReset,
//                  (void *)0,
//                  (OS_STK *)&AppTaskGateWayResetStk[APP_TASK_GateWayReset_STK_SIZE - 1],
//                  APP_TASK_GateWayReset_PRIO,
//                  APP_TASK_GateWayReset_PRIO,
//                  (OS_STK *)&AppTaskGateWayResetStk[0],
//                  APP_TASK_GateWayReset_STK_SIZE,
//                  (void *)0,
//                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
//  
//  OSTaskNameSet(APP_TASK_GateWayReset_PRIO, APP_TASK_GateWayReset_NAME, &err);
//  
  OSTaskCreateExt(AppTaskN720Init,
                  (void *)0,
                  (OS_STK *)&AppTaskN720InitStk[APP_TASK_N720Init_STK_SIZE - 1],
                  APP_TASK_N720Init_PRIO,
                  APP_TASK_N720Init_PRIO,
                  (OS_STK *)&AppTaskN720InitStk[0],
                  APP_TASK_N720Init_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_N720Init_PRIO, APP_TASK_N720Init_NAME, &err);
  
  OSTaskCreateExt(AppTaskSMSComm,
                  (void *)0,
                  (OS_STK *)&AppTaskSMSCommStk[APP_TASK_SMSComm_STK_SIZE - 1],
                  APP_TASK_SMSComm_PRIO,
                  APP_TASK_SMSComm_PRIO,
                  (OS_STK *)&AppTaskSMSCommStk[0],
                  APP_TASK_SMSComm_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_SMSComm_PRIO, APP_TASK_SMSComm_NAME, &err);
  
  OSTaskCreateExt(AppTaskTCPComm,
                  (void *)0,
                  (OS_STK *)&AppTaskTCPCommStk[APP_TASK_TCPComm_STK_SIZE - 1],
                  APP_TASK_TCPComm_PRIO,
                  APP_TASK_TCPComm_PRIO,
                  (OS_STK *)&AppTaskTCPCommStk[0],
                  APP_TASK_TCPComm_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  OSTaskNameSet(APP_TASK_TCPComm_PRIO, APP_TASK_TCPComm_NAME, &err);
  /* 任务主体，必须是一个死循环 */
  while (1)     
  {  
//      static uint8_t a[] = {0x22, 0x22, 0x33, 0x44};
//      static uint8_t b[123] = {0};
//      sscanf(a, "%d", b);
//      
//     cJSON *root,*dir1,*dir2,*dir3;
//
//     char *out;
//
//     //创建json数组型结构体
//
//     root = cJSON_CreateArray();
//
//     //为数组添加对象
//
//     cJSON_AddItemToArray(root,dir1=cJSON_CreateObject());
//
//     //为对象添加字符串键值对
//
//     cJSON_AddStringToObject(dir1,"strbuf",a);
//
//     cJSON_AddNumberToObject(dir1,"strlen",strlen(a));
//
//
//     //将json结构体转换为字符串
//
//     out=cJSON_Print(root);
//
//     //删除
//
//     cJSON_Delete(root);
//     
//     printf("%s\n", out);
//     
//     free(out);
//     static char server_buf[] = "{\"status\":\"Success\",\"timestamp\":\"2019-04-19 11:10:26\"}";
//     static char time_buf[] = {0};
//     static uint16_t year = 0;
//     static uint8_t time1, time2, time3, time4, time5;
//     
//	 sscanf(server_buf, "%*[^0]0%d-%d-%d %d:%d:%d", &year, &time1, &time2, &time3, &time4, &time5);
     
     
     
     bsp_LedOn(1);
     OSTimeDly(100);	
     bsp_LedOff(1);
     OSTimeDly(1000);	
  }      
}
 
 
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
