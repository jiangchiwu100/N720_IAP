
#include "includes.h"

 


 
  DIR DirInf _exram;
  FILINFO FileInf _exram;
  BYTE ReadBuffer[1024] _exram={0};    /* �������� */
  FATFS fs _exram;       /* FatFs�ļ�ϵͳ���� */      
  FRESULT result _exram;              /* �ļ�������� */
  FATFS *pfs _exram;
  DWORD fre_clust _exram, fre_sect _exram, tot_sect _exram;
  FIL fnew _exram;     /* �ļ����� */
  UINT fnum _exram;    /* �ļ��ɹ���д���� */

 

/* �������ļ��ڵ��õĺ������� */
static void DispMenu(void);
void Mount_File_System(void);
static void ViewRootDir(void);
static void CreateNewFile(void);
static void ReadFileData(void);
static void GetDeviceInformation(void);
static void SeekNewFile(void);
static void DeleteDirFile(void);
static void ViewRootDir(void);
void DiskFormatting(void);
void DemoFatFS(void);






/*
*********************************************************************************************************
*	�� �� ��: DemoFatFS
*	����˵��: FatFS�ļ�ϵͳ��ʾ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DemoFatFS(void)
{
  uint8_t cmd;
  
  /* ��ӡ�����б��û�����ͨ�����ڲ���ָ�� */
//    bsp_Idle();		/* ���������bsp.c�ļ����û������޸��������ʵ��CPU���ߺ�ι�� */
    
    if (comGetChar(COM1, &cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
    {
      printf("\r\n");
      switch (cmd)
      {
      case '0':
        { 
           printf("׼�������ļ�ϵͳ\r\n"); 
           Mount_File_System();
        }
        break;
      case '1':
        {
          printf("��1 - GetDeviceInformation��\r\n");
          GetDeviceInformation();		/* ��ʾSD����Ŀ¼�µ��ļ��� */
        }
        break;
      case '2':
        {
          printf("��2 - CreateNewFile��\r\n");
          CreateNewFile();		/* ����һ�����ļ�,д��һ���ַ��� */
        }
        break;
        
      case '3':
        {
          printf("��3 - ReadFileData��\r\n");
          SeekNewFile();		/* ��ȡ��Ŀ¼��armfly.txt������ */
        }
        break;
        
      case '4':
        {
          printf("��4 - CreateDir��\r\n");
          ReadFileData();		/* ����Ŀ¼ */
        }
        break;
        
      case '5':
        {
          printf("��5 - DeleteDirFile��\r\n");
          DeleteDirFile();	/* ɾ��Ŀ¼���ļ� */
        }
        break;
        
      case '6':
        {
          printf("��6 - ViewRootDir��\r\n");
          ViewRootDir();	/* �ٶȲ��� */
        }
        break;
       case '7':
        {
          printf("��7 - DiskFormatting��\r\n");
          DiskFormatting();	/* �ٶȲ��� */
        }
        break;
      default:
        DispMenu();
        break;
      }
    }
}

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
  printf("\r\n--------------------------\r\n");
  printf("��ѡ���������:\r\n");
  printf("0 - �����ļ�ϵͳ\r\n");
  printf("1 - ��ʾ�豸��Ϣ\r\n");
  printf("2 - ����һ�����ļ�MeterLib.txt\r\n");
  printf("3 - ��λMeterLib.txt�ļ�д��������\r\n");
  printf("4 - ��MeterLib.txt�ļ�������\r\n");
  printf("5 - ɾ���ļ���Ŀ¼\r\n");
  printf("6 - ��ȡ��Ŀ¼�ļ�\r\n");
  printf("7 - ���̸�ʽ��\r\n");
}

void DiskFormatting(void)
{
   /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
 


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
      printf("��FLASH�ѳɹ������ļ�ϵͳ��\r\n");
    }
    else
    {
      printf("������ʽ��ʧ�ܡ�����\r\n");
 
    }
  /* ж���ļ�ϵͳ */
}
/*
*********************************************************************************************************
*	�� �� ��: Mount_File_System
*	����˵��: �����ļ�ϵͳ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Mount_File_System(void)
{
  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
 
 

  /* �����ļ�ϵͳ */
  result = f_mount(&fs,"1:",1);//1��������ִ��
  if(result == FR_NO_FILESYSTEM)
  {
    printf("flash��û���ļ�ϵͳ��������ʽ�� (%d)\r\n", result);
    /* ��ʽ�� */
    result=f_mkfs(FS_VOLUME_FLASH,0,0);	
    if(result == FR_OK)
    {
      printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
      result = f_mount(NULL,FS_VOLUME_FLASH,1);			
      /* ���¹���	*/			
      result = f_mount(&fs,FS_VOLUME_FLASH,1);
      printf("��FLASH�ѳɹ������ļ�ϵͳ��\r\n");
    }
    else
    {
      printf("������ʽ��ʧ�ܡ�����\r\n");
      while(1);
    }
  }
  else if(result != FR_OK)
  {
    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
  }
  else
  {
    printf("�����ļ�ϵͳ�ɹ� (%d)\r\n", result);
  }
  
  /* ж���ļ�ϵͳ */
  f_mount(NULL, "1:", 0);
}


 

static void GetDeviceInformation(void)
{


  
  result = f_mount(&fs,"1:",1);
  
  if(result!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",result);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��в���\r\n");    
    printf("\n*************** �豸��Ϣ��ȡ ***************\r\n");
    /* ��ȡ�豸��Ϣ�Ϳմش�С */
    result = f_getfree("1:", &fre_clust, &pfs);
    /* ����õ��ܵ����������Ϳ��������� */
    tot_sect = (pfs->n_fatent - 2) * pfs->csize;
    fre_sect = fre_clust * pfs->csize;
      /* ��ӡ��Ϣ(4096 �ֽ�/����) */
    printf("���豸�ܿռ䣺%10lu KB��\n�����ÿռ䣺  %10lu KB��\n", tot_sect *4, fre_sect *4);

  }

}





/*
*********************************************************************************************************
*	�� �� ��: CreateNewFile
*	����˵��: ��SD������һ�����ļ����ļ�������д��www.armfly.com��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateNewFile(void)
{
  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */

  char WriteBuffer[] ={0x12,0x34,0x56,0x78}; /* д������*/
  
    /* �����ļ�ϵͳ */
  result = f_mount(&fs,"1:",1);//1��������ִ��
  if (result != FR_OK)
  {
    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
  }
  else
  {
    /* ��/�����ļ� */
    printf("\r\n****** ������/����MeterLib.txt�ļ� ... ******\r\n");	
    result = f_open(&fnew, "1:MeterLib.txt",FA_CREATE_ALWAYS | FA_WRITE );//ע���ļ��� 1��
    if(result == FR_OK)
    {
      printf("������/����MeterLib.txt�ļ��ļ��ɹ���(%d)\n",result);
      /* дһ������ */
      result=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
      if (result == FR_OK)
      {
        printf("armfly.txt �ļ�д��ɹ�\r\n");
        printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
        printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
      }
      else
      {
        printf("armfly.txt �ļ�д��ʧ��\r\n");
      }
    }
    else
    {
      printf("������/����MeterLib.txt�ļ��ļ�ʧ�ܣ�(%d)\n",result);
    }
    
  }
  /* �ر��ļ�*/
  f_close(&fnew);
}

/*
*********************************************************************************************************
*	�� �� ��: ReadFileData
*	����˵��: ��ȡ�ļ�armfly.txtǰ128���ַ�������ӡ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ReadFileData(void)
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
    printf("�����ļ�ϵͳ�ɹ� (%d)\r\n", result);
    /* ���ļ� */
    result = f_open(&fnew, "1:MeterLib.txt",FA_OPEN_EXISTING | FA_READ );//ע�� FA_READ
    if (result !=  FR_OK)
    {
      printf("û���ҵ� : MeterLib.txt\r\n");
    }
    else
    {
      printf("�����ļ��ɹ���\r\n");
      result = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
      if(result == FR_OK)
      {
        printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
        printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
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




static void SeekNewFile(void)
{

  char WriteBuffer[] ={0x11,0x22,0x33,0x44}; /* д������*/
  
  printf("\n******** �ļ���λ�͸�ʽ��д�빦�ܲ��� ********\r\n");
  result = f_mount(&fs,"1:",1);
  if(result!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",result);
  }
  else
  {
    printf("�����ⲿFlash�����ļ�ϵͳ�ɹ���(%d)\r\n",result);
    result = f_open(&fnew, "1:MeterLib.txt",FA_OPEN_EXISTING|FA_WRITE|FA_READ );
    if ( result == FR_OK )
    { /*  �ļ���λ */
      result = f_lseek(&fnew, ((&fnew)->fsize));        // ��λ���ļ�ĩβ
      if (result == FR_OK)
      {
        printf("���� �ļ���λ�ɹ���(%d)\r\n",result);
        f_printf(&fnew,WriteBuffer);
        /*  �ļ���λ���ļ���ʼλ�� */
        result = f_lseek(&fnew,0);
        /* ��ȡ�ļ��������ݵ������� */
        result = f_read(&fnew,ReadBuffer,f_size(&fnew),&fnum);
        if(result == FR_OK)
        {
          printf("���ļ����ݣ�\n%s\n",ReadBuffer);
        }
      }
      else
      {
        printf("���� �ļ���λʧ�ܡ�(%d)\r\n",result);
      }
    }
    else
    {
      printf("!! ���ļ�ʧ�ܣ�%d\n",result);
    }
  }
  
  f_close(&fnew);  
}


/*
*********************************************************************************************************
*	�� �� ��: DeleteDirFile
*	����˵��: ɾ��SD����Ŀ¼�µ� armfly.txt �ļ��� Dir1��Dir2 Ŀ¼
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
 ;
  
  result = f_mount(&fs,"1:",1);
  if(result!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",result);
  }
  else
  {
    /* ɾ���ļ� MeterLib.txt */  
    result = f_unlink("1:MeterLib.txt");
    result = f_unlink("1:FatFs��д�����ļ�.txt");
    if (result == FR_OK)
    {
      printf("ɾ���ļ� MeterLib.txt �ɹ�\r\n");
    }
    else if (result == FR_NO_FILE)
    {
      printf("û�з����ļ���Ŀ¼ :%s\r\n", "armfly.txt");
    }
    else
    {
      printf("ɾ��MeterLib.txtʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", result);
    }
  }

  /* ж���ļ�ϵͳ */
  f_mount(NULL, "1:", 1);
}






///*
//*********************************************************************************************************
//*	�� �� ��: WriteFileTest
//*	����˵��: �����ļ���д�ٶ�
//*	��    �Σ���
//*	�� �� ֵ: ��
//*********************************************************************************************************
//*/
////static void WriteFileTest(void)
////{
//  /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
//  FRESULT result;
//  FATFS fs;
//  FIL file;
//  DIR DirInf;
//  uint32_t bw;
//  uint32_t i,k;
//  uint32_t runtime1,runtime2,timelen;
//  uint8_t err = 0;
//  char TestFileName[13];
//  static uint8_t s_ucTestSn = 0;
//  
//  for (i = 0; i < sizeof(g_TestBuf); i++)
//  {
//    g_TestBuf[i] = (i / 512) + '0';
//  }
//  
//  /* �����ļ�ϵͳ */
//  result = f_mount(&fs, FS_VOLUME_SD, 0);			/* Mount a logical drive */
//  if (result != FR_OK)
//  {
//    printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
//  }
//  
//  /* �򿪸��ļ��� */
//  result = f_opendir(&DirInf, "/"); /* ���������������ӵ�ǰĿ¼��ʼ */
//  if (result != FR_OK)
//  {
//    printf("�򿪸�Ŀ¼ʧ�� (%d)\r\n", result);
//    return;
//  }
//  
//  /* ���ļ� */
//  sprintf(TestFileName, "Speed%02d.txt", s_ucTestSn++);		/* ÿд1�Σ���ŵ��� */
//  result = f_open(&file, TestFileName, FA_CREATE_ALWAYS | FA_WRITE);
//  
//  /* дһ������ */
//  printf("��ʼд�ļ�%s %dKB ...\r\n", TestFileName, TEST_FILE_LEN / 1024);
//  runtime1 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
//  for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
//  {
//    result = f_write(&file, g_TestBuf, sizeof(g_TestBuf), &bw);
//    if (result == FR_OK)
//    {
//      if (((i + 1) % 8) == 0)
//      {
//        printf(".");
//      }
//    }
//    else
//    {
//      err = 1;
//      printf("%s�ļ�дʧ��\r\n", TestFileName);
//      break;
//    }
//  }
//  runtime2 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
//  
//  if (err == 0)
//  {
//    timelen = (runtime2 - runtime1);
//    printf("\r\n  д��ʱ : %dms   ƽ��д�ٶ� : %dB/S (%dKB/S)\r\n",
//           timelen,
//           (TEST_FILE_LEN * 1000) / timelen,
//           ((TEST_FILE_LEN / 1024) * 1000) / timelen);
//  }
//  
//  f_close(&file);		/* �ر��ļ�*/
//  
//  
//  /* ��ʼ���ļ����� */
//  result = f_open(&file, TestFileName, FA_OPEN_EXISTING | FA_READ);
//  if (result !=  FR_OK)
//  {
//    printf("û���ҵ��ļ�: %s\r\n", TestFileName);
//    return;
//  }
//  
//  printf("��ʼ���ļ� %dKB ...\r\n", TEST_FILE_LEN / 1024);
//  runtime1 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
//  for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
//  {
//    result = f_read(&file, g_TestBuf, sizeof(g_TestBuf), &bw);
//    if (result == FR_OK)
//    {
//      if (((i + 1) % 8) == 0)
//      {
//        printf(".");
//      }
//      
//      /* �Ƚ�д��������Ƿ���ȷ�������ᵼ�¶����ٶȽ�����͵� 3.5MBytes/S */
//      for (k = 0; k < sizeof(g_TestBuf); k++)
//      {
//        if (g_TestBuf[k] != (k / 512) + '0')
//        {
//          err = 1;
//          printf("Speed1.txt �ļ����ɹ����������ݳ���\r\n");
//          break;
//        }
//      }
//      if (err == 1)
//      {
//        break;
//      }
//    }
//    else
//    {
//      err = 1;
//      printf("Speed1.txt �ļ���ʧ��\r\n");
//      break;
//    }
//  }
//  runtime2 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
//  
//  if (err == 0)
//  {
//    timelen = (runtime2 - runtime1);
//    printf("\r\n  ����ʱ : %dms   ƽ�����ٶ� : %dB/S (%dKB/S)\r\n", timelen,
//           (TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
//  }
//  
//  /* �ر��ļ�*/
//  f_close(&file);
//  
//  /* ж���ļ�ϵͳ */
//  f_mount(NULL, FS_VOLUME_SD, 0);
//}
//
