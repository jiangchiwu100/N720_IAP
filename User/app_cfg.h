/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/






/*
*********************************************************************************************************
*
*	模块名称 : uCOS-II的应用配置
*	文件名称 : app_cfg.c
*	版    本 : V1.0
*	说    明 : ucos-ii的应用配置
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-08-02 Eric2013  正式发布
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/






#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT







/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  APP_TASK_START_NAME                          "Start Task"
#define  APP_TASK_Temp_NAME                           "Temperature Task"
#define  APP_TASK_N720Init_NAME                        "N720 Init Task"
#define  APP_TASK_SMSComm_NAME                         "SMS Comm Task"
#define  APP_TASK_TCPComm_NAME                         "TCP Comm Task"
#define  APP_TASK_GateWayInit_NAME                    "GateWay Init Task"
#define  APP_TASK_GateWayReset_NAME                   "GateWay Reset Task"
#define  APP_TASK_LoraPing_NAME                      "Lora Ping Task"
#define  APP_TASK_LoraPolling_NAME                   "Lora Polling Task"
#define  APP_TASK_LoraBeacon_NAME                    "Lora Beacon Task"
/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
/* 启动任务 */
#define  APP_TASK_START_PRIO                               0
#define  APP_TASK_LoraBeacon_PRIO                          3 //lora polling 任务 要高于 polling任务
#define  APP_TASK_LoraPing_PRIO                            5 //lora ping 任务 要高于 polling任务
#define  APP_TASK_LoraPolling_PRIO                         8 //lora polling 任务 要高于 polling任务

#define  APP_TASK_Temp_PRIO                                32 
#define  APP_TASK_SMSComm_PRIO                             20
#define  APP_TASK_TCPComm_PRIO                             21
#define  APP_TASK_N720Init_PRIO                            22

#define  APP_TASK_GateWayInit_PRIO                         31
#define  APP_TASK_GateWayReset_PRIO                        30

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                          256
#define  APP_TASK_LoraBeacon_STK_SIZE                     256
#define  APP_TASK_LoraPing_STK_SIZE                       256
#define  APP_TASK_LoraPolling_STK_SIZE                    256
#define  APP_TASK_Temp_STK_SIZE                           128

#define  APP_TASK_N720Init_STK_SIZE                       256
#define  APP_TASK_SMSComm_STK_SIZE                        64
#define  APP_TASK_TCPComm_STK_SIZE                        512

#define  APP_TASK_GateWayInit_STK_SIZE                    256
#define  APP_TASK_GateWayReset_STK_SIZE                   128


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif

#include <cpu.h>
void  App_SerPrintf  (CPU_CHAR *format, ...);

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
#define  APP_TRACE_LEVEL                        TRACE_LEVEL_DBG
#else
#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#endif
#define  APP_TRACE                              App_SerPrintf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
