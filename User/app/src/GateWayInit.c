#include "includes.h"



OS_STK AppTaskGateWayInitStk[APP_TASK_GateWayInit_STK_SIZE];	


/*
*********************************************************************************************************
*	�� �� ��: DispMenu
*	����˵��: ��ʾ������ʾ�˵�
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-II������Ϣ��ӡ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
  OS_TCB      *ptcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
  OS_STK_DATA stk;	        /* ���ڶ�ȡ����ջʹ����� */
  
  ptcb    = &OSTCBTbl[0];		/* ָ���1��������ƿ�(TCB) */
  
  /* ��ӡ���� */
  printf("==================================================\r\n");
  printf("  ���ȼ�   ʹ��ջ  ʣ��ջ  �ٷֱ�   ������\r\n");
  printf("  Prio     Used    Free    Per      Taskname\r\n");
  
  OSTimeDly(10);
  
  /* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
  while (ptcb != NULL)
  {
    /* 
    ptcb->OSTCBPrio : ������ƿ��б������������ȼ�
    ptcb->OSTCBTaskName : ������ƿ��б�����������ƣ���Ҫ�ڴ��������ʱ��
    ����OSTaskNameSet()ָ���������ƣ����磺
    OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
    */
    OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* �������ջ���ÿռ� */
    printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
           stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
           ptcb->OSTCBTaskName);		
    ptcb = ptcb->OSTCBPrev;		            /* ָ����һ��������ƿ� */
  }
}

 
/*
*********************************************************************************************************
*	�� �� ��: AppTaskLED
*	����˵��: LED2��˸			  			  
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
void AppTaskGateWayInit(void *p_arg)
{
  static INT8U   err;
  static uint8_t tem = 0;
  static uint8_t ucRxBuf[256] ={0} ;
  static char *addr;
  static Str_Calendar rtc;
  
  
  (void)p_arg;		/* ����������澯 */
  
  while (1) 
  {
    OSSemPend(SempDEBUGRx, 0, &err);
    
    comGetBuf(COM_DEBUG, ucRxBuf);
    if(strstr(ucRxBuf,"GateWay Disp Menu") != NULL)//��ӡĿ¼
    {
      DispMenu();
    }
    else if(strstr(ucRxBuf,"GateWay Init") != NULL)//���̳�ʼ��
    {
      DiskFormatting();
      CreateGateWayMeterLibFile();
	  CreateGateWayConfigInfoFile();
    }
	else if(strstr(ucRxBuf,"GateWay ConfigInfo?") != NULL)
	{
				
		
	}
    else if(strstr(ucRxBuf,"GateWay RTC:") != NULL)//����RTC 
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
        printf(" ********��д��8025T*******\n\r");
      }
    }
    else if(strstr(ucRxBuf,"GateWay RTC?") != NULL)//��ȡRTC
    {
      RS8025T_Get_Calendar_Time(&rtc);
      
      printf ("20%x��",rtc.Year);
      printf ("%x��",rtc.Month);
      printf ("%x��",rtc.DayOfMonth);
      printf ("%xʱ",rtc.Hours);
      printf ("%x��",rtc.Minutes);
      printf ("%x��",rtc.Seconds);
      printf("\r\n");
    }
    else if(strstr(ucRxBuf,"File System Menu?") != NULL)//��ȡ�ļ�Ŀ¼
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
//          printf("������ɹ� (%d)\r\n", stn);
          if(Insert_Meter_Lib_Data(&id[0],&dat[0],sizeof(dat)) == true)
          {
            printf("�����·����ݳɹ�\n\r");
          }
          else
          {
            printf("�����·�����ʧ��\n\r");
          }
//        }
//        else
//        {
//          printf("������ʧ��\r\n");
//        }
//      }
//      else
//      {
//        printf("������ʧ��\r\n");
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
          printf("������ɹ� (%d)\r\n", stn);
          if(Insert_Meter_Lib_Data(&id[0],&dat[0],sizeof(dat)) == true)
          {
            printf("�����·����ݳɹ�\n\r");
          }
          else
          {
            printf("�����·�����ʧ��\n\r");
          }
        }
        else
        {
          printf("������ʧ��\r\n");
        }
      }
      else
      {
        printf("������ʧ��\r\n");
      }
      
//      
      
    }
  }

}