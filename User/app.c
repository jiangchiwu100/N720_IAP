/*
*********************************************************************************************************
*	                                  
 
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*


OSTaskSuspend(TASK1_PRIO);//�������� ���ȼ�
OSTaskResume(TASK1_PRIO);//�ָ����� ���ȼ� 

*********************************************************************************************************
*/
#include "includes.h"


/*
********************************************************************************************************
*                              �����ջ
********************************************************************************************************
*/

/* ����ÿ������Ķ�ջ�ռ䣬app_cfg.h�ļ��к궨��ջ��С */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

 
			
/*
*******************************************************************************************************
*                              ��������
*******************************************************************************************************
*/

 
static void AppTaskStart(void *p_arg);
 
//static void DispTaskInfo(void);
 /*
*******************************************************************************************************
*                               ����
*******************************************************************************************************
*/

/*����һ���ź���*/
OS_EVENT *SempN720Rx;
OS_EVENT *SempDEBUGRx;

OS_EVENT *SempTCPRx;//TCP ���յ�������
OS_EVENT *SempSMSRx;//SMS ���յ�������

OS_EVENT *SempLoraRx;
OS_EVENT *SempLoraTx;

/* ����һ�����䣬 ��ֻ��һ������ָ�룬 OSMboxCreate�����ᴴ������������Դ */
OS_EVENT *MboxGateWayTemp;//����
OS_EVENT *MboxLoraPing;
/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{ 
  printf("********************���Դ�ӡ���*********************\n\r");
} 
/*
*******************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*******************************************************************************************************
*/
int main(void)
{
  INT8U  err;
  
  /* ��ʼ��"uC/OS-II"�ں� */
  OSInit();
  
  /* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
  OSTaskCreateExt(AppTaskStart,	/* ����������ָ�� */
                  (void *)0,		/* ���ݸ�����Ĳ��� */
                  (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* ָ������ջջ����ָ�� */
                  APP_TASK_START_PRIO,	/* ��������ȼ�������Ψһ������Խ�����ȼ�Խ�� */
                  APP_TASK_START_PRIO,	/* ����ID��һ����������ȼ���ͬ */
                  (OS_STK *)&AppTaskStartStk[0],/* ָ������ջջ�׵�ָ�롣OS_STK_GROWTH ������ջ�������� */
                  APP_TASK_START_STK_SIZE, /* ����ջ��С */
                  (void *)0,	/* һ���û��ڴ�����ָ�룬����������ƿ�TCB����չ����
                  ���������л�ʱ����CPU����Ĵ��������ݣ���һ�㲻�ã���0���� */
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* ����ѡ���� */
  
  /*  �������£�
  OS_TASK_OPT_STK_CHK      ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
  OS_TASK_OPT_STK_CLR      �ڴ�������ʱ����������ջ
  OS_TASK_OPT_SAVE_FP      ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
  */                  
  
  /* ָ����������ƣ����ڵ��ԡ���������ǿ�ѡ�� */
  OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);
  
  /*ucosII�Ľ��ļ�������0    ���ļ�������0-4294967295*/ 
  OSTimeSet(0);	
  
  /* ����������ϵͳ������Ȩ����uC/OS-II */
  OSStart();
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(��BSP_Init��ʵ��)
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{	
  /* �����ڱ���������澯����������������κ�Ŀ����� */	
  (void)p_arg;
  INT8U  err;
  /* BSP ��ʼ���� BSP = Board Support Package �弶֧�ְ����������Ϊ�ײ�������*/
  bsp_Init();
  CPU_Init();          
  BSP_Tick_Init();   
  
  /* ���CPU������ͳ��ģ���ʼ�����ú�����������CPUռ���� */
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();
#endif	
  
  /* �����ź��� */
  SempN720Rx  = OSSemCreate(0);	  /* ����һ���ź��� ʵ��ͬ������ */
  SempDEBUGRx = OSSemCreate(0);	  /* ����һ���ź��� ʵ��ͬ������ */
  
  SempLoraRx  = OSSemCreate(0);	  /* ����һ���ź��� ʵ��ͬ������ */
  SempLoraTx  = OSSemCreate(0);	  /* ����һ���ź��� ʵ��ͬ������ */
  
  SempTCPRx   = OSSemCreate(0);	  /* ����һ���ź��� ʵ��ͬ������ */
  SempSMSRx   = OSSemCreate(0);	  /* ����һ���ź��� ʵ��ͬ������ */

  /* ��������(MBOX) */
  MboxGateWayTemp = OSMboxCreate((void *)0);
  MboxLoraPing    = OSMboxCreate((void *)0);/* ����һ���ź��� ʵ��ͬ������ */
  
  
  /* ����Ӧ�ó�������� */
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
  /* �������壬������һ����ѭ�� */
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
//     //����json�����ͽṹ��
//
//     root = cJSON_CreateArray();
//
//     //Ϊ������Ӷ���
//
//     cJSON_AddItemToArray(root,dir1=cJSON_CreateObject());
//
//     //Ϊ��������ַ�����ֵ��
//
//     cJSON_AddStringToObject(dir1,"strbuf",a);
//
//     cJSON_AddNumberToObject(dir1,"strlen",strlen(a));
//
//
//     //��json�ṹ��ת��Ϊ�ַ���
//
//     out=cJSON_Print(root);
//
//     //ɾ��
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
 
 
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
