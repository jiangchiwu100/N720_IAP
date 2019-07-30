#include "includes.h"



/* �ļ�ϵͳ �������� */
DIR DirInf exram;
FILINFO FileInf exram;

FATFS *pfs exram;
DWORD fre_clust exram, fre_sect exram, tot_sect exram;
FIL fnew exram;     /* �ļ����� */
UINT fnum exram;    /* �ļ��ɹ���д���� */
FATFS fs exram;       /* FatFs�ļ�ϵͳ���� */      
FRESULT result exram;              /* �ļ�������� */ 

 
/* ������ ȫ�ֱ���  ���豣�����ļ�ϵͳ���� */
uint32_t  SysState = 0;
Str_SMS_Buf SMSbuf;     //SMS��Ϣ���
Str_TCP_Buf TCPbuf;     //TCP��Ϣ���

/* ������ ȫ�ֱ���  �豣�����ļ�ϵͳ���� */

Str_MeterLib_Parm MeterLib[MeterLibMaxNum] exram;

uint8_t  N720MessageNum[300] exram;//���������Ϣ���� ���֧��256������ ÿ�����ݵĳ��ȴ�������
uint8_t  N720MessageQue[32768] exram; //���������Ϣ���� 

uint16_t GW3761DatFrameSeqNum = 0;

Str_ConfigInfo_PARM ConfigInfo;

void GateWay_Variable_Init(void)
{
  ReadGateWayConfigInfoFile();
  ReadGateWayMeterLibFile();
}

 
 


/*
*********************************************************************************************************
*	�� �� ��: DiskFormatting
*	����˵��: ���̸�ʽ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void DiskFormatting(void)
 {
   result = f_mount(&fs,"1:",1);//1��������ִ��
   /* �����ļ�ϵͳ */
   printf("��FLASH������ʽ����\r\n");
   
   /* ��ʽ�� */
   result=f_mkfs(FS_VOLUME_FLASH,0,0);	
   if(result == FR_OK)
   {
     printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
     /* ��ʽ������ȡ������ */
     result = f_mount(NULL,FS_VOLUME_FLASH,1);			
     /* ���¹���	*/			
     result = f_mount(&fs,FS_VOLUME_FLASH,1);
     if (result != FR_OK)
     {
       printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
     }
     else
     {
       printf("��FLASH�ѳɹ������ļ�ϵͳ��\r\n");
     }
 
   }
   else
   {
     printf("������ʽ��ʧ�ܡ�����\r\n");
   }
   /* ж���ļ�ϵͳ */
 }


/*
*********************************************************************************************************
*	�� �� ��: CreateGateWayIDFile
*	����˵��: ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CreateGateWayConfigInfoFile(void)
{
  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */

    /* �����ļ�ϵͳ */
  result = f_mount(&fs,"1:",1);//1��������ִ��
  if (result != FR_OK)
  {
    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
  }
  else
  {
    /* ��/�����ļ� */
    printf("\r\n****** ������/GateWayConfigInfo.txt�ļ�******\r\n");	
    result = f_open(&fnew, "1:GateWayConfigInfo.txt",FA_CREATE_ALWAYS | FA_WRITE );//ע���ļ��� 1��
    if(result == FR_OK)
    {
      result=f_write(&fnew,&ConfigInfo ,sizeof(ConfigInfo),&fnum);/* дһ������ */
      if (result == FR_OK)
      {
        printf("GateWayID.txt �ļ�д��ɹ�\r\n");
      }
      else
      {
        printf("GateWayID.txt �ļ�д��ʧ��\r\n");
      }
    }
    else
    {
      printf("������/����GateWayID.txt�ļ��ļ�ʧ�ܣ�(%d)\n",result);
    }
    
  }
  /* �ر��ļ�*/
  f_close(&fnew);
}


/*
*********************************************************************************************************
*	�� �� ��: ReadGateWayIDFile
*	����˵��:  
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ReadGateWayConfigInfoFile(void)
{
  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */

  /* �����ļ�ϵͳ */
  result = f_mount(&fs,"1:",1);//1��������ִ��
  if (result != FR_OK)
  {
    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
  }
  else
  {
    /* ���ļ� */
    result = f_open(&fnew, "1:GateWayConfigInfo.txt",FA_OPEN_EXISTING | FA_READ );//ע�� FA_READ
    if (result !=  FR_OK)
    {
      printf("û���ҵ� : GateWayConfigInfo.txt\r\n");
    }
    else
    {
      result = f_read(&fnew, &ConfigInfo, sizeof(ConfigInfo), &fnum); 
      if(result == FR_OK)
      {
 
      }
      else
      {
        printf("�����ļ���ȡʧ�ܣ�(%d)\n",result);
      }
    }
  }
  /* �ر��ļ�*/
  f_close(&fnew);
}

 
/*
*********************************************************************************************************
*	�� �� ��: CreateGateWayIDFile
*	����˵��: ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CreateGateWayMeterLibFile(void)
{
  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */

    /* �����ļ�ϵͳ */
  result = f_mount(&fs,"1:",1);//1��������ִ��
  if (result != FR_OK)
  {
    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
  }
  else
  {
    /* ��/�����ļ� */
    printf("\r\n****** ������/���� GateWayMeterLib.txt�ļ�******\r\n");	
    result = f_open(&fnew, "1:GateWayMeterLib.txt",FA_CREATE_ALWAYS | FA_WRITE );//ע���ļ��� 1��
    if(result == FR_OK)
    {
      result=f_write(&fnew,MeterLib,sizeof(MeterLib),&fnum);/* дһ������ */
      if (result == FR_OK)
      {
        printf("GateWayMeterLib.txt �ļ�д��ɹ�\r\n");
      }
      else
      {
        printf("GateWayMeterLib.txt �ļ�д��ʧ��\r\n");
      }
    }
    else
    {
      printf("������/���� GateWayMeterLib.txt�ļ��ļ�ʧ�ܣ�(%d)\n",result);
    }
    
  }
  /* �ر��ļ�*/
  f_close(&fnew);
}


void ReadGateWayMeterLibFile(void)
{
  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */

  
  /* �����ļ�ϵͳ */
  result = f_mount(&fs,"1:",1);//1��������ִ��
  if (result != FR_OK)
  {
    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
  }
  else
  {
    /* ���ļ� */
    result = f_open(&fnew, "1:GateWayMeterLib.txt",FA_OPEN_EXISTING | FA_READ );//ע�� FA_READ
    if (result !=  FR_OK)
    {
      printf("û���ҵ� : GateWayMeterLib.txt\r\n");
    }
    else
    {
      result = f_read(&fnew,&MeterLib, sizeof(MeterLib), &fnum); 
      if(result == FR_OK)
      {
        printf("�����ļ���ȡ�ɹ���(%d)\n",result);
      }
      else
      {
        printf("�����ļ���ȡʧ�ܣ�(%d)\n",result);
      }
    }
  }
  /* �ر��ļ�*/
  f_close(&fnew);
}






/*
*********************************************************************************************************
*	�� �� ��: ViewRootDir
*	����˵��: ��ʾSD����Ŀ¼�µ��ļ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ViewRootDir(void)
{

  uint32_t cnt = 0;
  char lfname[32];
  
  result = f_mount(&fs,"1:",1);
  if(result!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",result);
  }
  else
  {
    printf("�����ⲿFlash�����ļ�ϵͳ�ɹ���(%d)\r\n",result);
    /* �򿪸��ļ��� */
    result = f_opendir(&DirInf, "1:"); /* ���������������ӵ�ǰĿ¼��ʼ ע�⡰1:��*/
    if (result != FR_OK)
    {
      printf("�򿪸�Ŀ¼ʧ�� (%d)\r\n", result);
    }
    else
    {
      printf("�򿪸�Ŀ¼�ɹ� (%d)\r\n", result);
      FileInf.lfname = lfname;/* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
      FileInf.lfsize = 256;
      printf("���� | �ļ���С | ���ļ��� | ���ļ���\r\n");
      for (cnt = 0; ;cnt++)
      {
        result = f_readdir(&DirInf,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
          break;
        }
        
        if (FileInf.fname[0] == '.')
        {
          continue;
        }
        
        /* �ж����ļ�������Ŀ¼ */
        if (FileInf.fattrib & AM_DIR)
        {
          printf("(0x%02d)Ŀ¼  ", FileInf.fattrib);
        }
        else
        {
          printf("(0x%02d)�ļ�|", FileInf.fattrib);
        }
        
        /* ��ӡ�ļ���С, ���4G */
        printf("%10d | ", FileInf.fsize);
        
        printf("%s |", FileInf.fname);	/* ���ļ��� */
        
        printf("%s\r\n", (char *)FileInf.lfname);	/* ���ļ��� */
      }
    }
  }
  /* ж���ļ�ϵͳ */
  f_mount(NULL, "1:", 1);
}
