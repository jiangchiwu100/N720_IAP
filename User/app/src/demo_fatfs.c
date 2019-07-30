
#include "includes.h"

 


 
  DIR DirInf _exram;
  FILINFO FileInf _exram;
  BYTE ReadBuffer[1024] _exram={0};    /* 读缓冲区 */
  FATFS fs _exram;       /* FatFs文件系统对象 */      
  FRESULT result _exram;              /* 文件操作结果 */
  FATFS *pfs _exram;
  DWORD fre_clust _exram, fre_sect _exram, tot_sect _exram;
  FIL fnew _exram;     /* 文件对象 */
  UINT fnum _exram;    /* 文件成功读写数量 */

 

/* 仅允许本文件内调用的函数声明 */
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
*	函 数 名: DemoFatFS
*	功能说明: FatFS文件系统演示主程序
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void DemoFatFS(void)
{
  uint8_t cmd;
  
  /* 打印命令列表，用户可以通过串口操作指令 */
//    bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */
    
    if (comGetChar(COM1, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
    {
      printf("\r\n");
      switch (cmd)
      {
      case '0':
        { 
           printf("准备挂载文件系统\r\n"); 
           Mount_File_System();
        }
        break;
      case '1':
        {
          printf("【1 - GetDeviceInformation】\r\n");
          GetDeviceInformation();		/* 显示SD卡根目录下的文件名 */
        }
        break;
      case '2':
        {
          printf("【2 - CreateNewFile】\r\n");
          CreateNewFile();		/* 创建一个新文件,写入一个字符串 */
        }
        break;
        
      case '3':
        {
          printf("【3 - ReadFileData】\r\n");
          SeekNewFile();		/* 读取根目录下armfly.txt的内容 */
        }
        break;
        
      case '4':
        {
          printf("【4 - CreateDir】\r\n");
          ReadFileData();		/* 创建目录 */
        }
        break;
        
      case '5':
        {
          printf("【5 - DeleteDirFile】\r\n");
          DeleteDirFile();	/* 删除目录和文件 */
        }
        break;
        
      case '6':
        {
          printf("【6 - ViewRootDir】\r\n");
          ViewRootDir();	/* 速度测试 */
        }
        break;
       case '7':
        {
          printf("【7 - DiskFormatting】\r\n");
          DiskFormatting();	/* 速度测试 */
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
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
  printf("\r\n--------------------------\r\n");
  printf("请选择操作命令:\r\n");
  printf("0 - 挂载文件系统\r\n");
  printf("1 - 显示设备信息\r\n");
  printf("2 - 创建一个新文件MeterLib.txt\r\n");
  printf("3 - 定位MeterLib.txt文件写入新内容\r\n");
  printf("4 - 读MeterLib.txt文件的内容\r\n");
  printf("5 - 删除文件和目录\r\n");
  printf("6 - 读取根目录文件\r\n");
  printf("7 - 磁盘格式化\r\n");
}

void DiskFormatting(void)
{
   /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
 


  result = f_mount(&fs,"1:",1);//1代表立刻执行
  /* 挂载文件系统 */
    printf("》FLASH即将格式化。\r\n");
 
    /* 格式化 */
    result=f_mkfs(FS_VOLUME_FLASH,0,0);	
    if(result == FR_OK)
    {
      printf("》FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
      result = f_mount(NULL,FS_VOLUME_FLASH,1);			
      /* 重新挂载	*/			
      result = f_mount(&fs,FS_VOLUME_FLASH,1);
      printf("》FLASH已成功挂载文件系统。\r\n");
    }
    else
    {
      printf("《《格式化失败。》》\r\n");
 
    }
  /* 卸载文件系统 */
}
/*
*********************************************************************************************************
*	函 数 名: Mount_File_System
*	功能说明: 挂载文件系统
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void Mount_File_System(void)
{
  /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
 
 

  /* 挂载文件系统 */
  result = f_mount(&fs,"1:",1);//1代表立刻执行
  if(result == FR_NO_FILESYSTEM)
  {
    printf("flash还没有文件系统，即将格式化 (%d)\r\n", result);
    /* 格式化 */
    result=f_mkfs(FS_VOLUME_FLASH,0,0);	
    if(result == FR_OK)
    {
      printf("》FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
      result = f_mount(NULL,FS_VOLUME_FLASH,1);			
      /* 重新挂载	*/			
      result = f_mount(&fs,FS_VOLUME_FLASH,1);
      printf("》FLASH已成功挂载文件系统。\r\n");
    }
    else
    {
      printf("《《格式化失败。》》\r\n");
      while(1);
    }
  }
  else if(result != FR_OK)
  {
    printf("挂载文件系统失败 (%d)\r\n", result);
  }
  else
  {
    printf("挂载文件系统成功 (%d)\r\n", result);
  }
  
  /* 卸载文件系统 */
  f_mount(NULL, "1:", 0);
}


 

static void GetDeviceInformation(void)
{


  
  result = f_mount(&fs,"1:",1);
  
  if(result!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",result);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行测试\r\n");    
    printf("\n*************** 设备信息获取 ***************\r\n");
    /* 获取设备信息和空簇大小 */
    result = f_getfree("1:", &fre_clust, &pfs);
    /* 计算得到总的扇区个数和空扇区个数 */
    tot_sect = (pfs->n_fatent - 2) * pfs->csize;
    fre_sect = fre_clust * pfs->csize;
      /* 打印信息(4096 字节/扇区) */
    printf("》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n", tot_sect *4, fre_sect *4);

  }

}





/*
*********************************************************************************************************
*	函 数 名: CreateNewFile
*	功能说明: 在SD卡创建一个新文件，文件内容填写“www.armfly.com”
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateNewFile(void)
{
  /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */

  char WriteBuffer[] ={0x12,0x34,0x56,0x78}; /* 写缓冲区*/
  
    /* 挂载文件系统 */
  result = f_mount(&fs,"1:",1);//1代表立刻执行
  if (result != FR_OK)
  {
    printf("挂载文件系统失败 (%d)\r\n", result);
  }
  else
  {
    /* 打开/创建文件 */
    printf("\r\n****** 即将打开/创建MeterLib.txt文件 ... ******\r\n");	
    result = f_open(&fnew, "1:MeterLib.txt",FA_CREATE_ALWAYS | FA_WRITE );//注意文件名 1：
    if(result == FR_OK)
    {
      printf("！！打开/创建MeterLib.txt文件文件成功：(%d)\n",result);
      /* 写一串数据 */
      result=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
      if (result == FR_OK)
      {
        printf("armfly.txt 文件写入成功\r\n");
        printf("》文件写入成功，写入字节数据：%d\n",fnum);
        printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
      }
      else
      {
        printf("armfly.txt 文件写入失败\r\n");
      }
    }
    else
    {
      printf("！！打开/创建MeterLib.txt文件文件失败：(%d)\n",result);
    }
    
  }
  /* 关闭文件*/
  f_close(&fnew);
}

/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 读取文件armfly.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadFileData(void)
{
  /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */

  
  /* 挂载文件系统 */
  result = f_mount(&fs,"1:",1);//1代表立刻执行
  if (result != FR_OK)
  {
    printf("挂载文件系统失败 (%d)\r\n", result);
  }
  else
  {
    printf("挂载文件系统成功 (%d)\r\n", result);
    /* 打开文件 */
    result = f_open(&fnew, "1:MeterLib.txt",FA_OPEN_EXISTING | FA_READ );//注意 FA_READ
    if (result !=  FR_OK)
    {
      printf("没有找到 : MeterLib.txt\r\n");
    }
    else
    {
      printf("》打开文件成功。\r\n");
      result = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
      if(result == FR_OK)
      {
        printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
        printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
      }
      else
      {
        printf("！！文件读取失败：(%d)\n",result);
      }
    }
  }
  /* 关闭文件*/
  f_close(&fnew);
}




static void SeekNewFile(void)
{

  char WriteBuffer[] ={0x11,0x22,0x33,0x44}; /* 写缓冲区*/
  
  printf("\n******** 文件定位和格式化写入功能测试 ********\r\n");
  result = f_mount(&fs,"1:",1);
  if(result!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",result);
  }
  else
  {
    printf("！！外部Flash挂载文件系统成功。(%d)\r\n",result);
    result = f_open(&fnew, "1:MeterLib.txt",FA_OPEN_EXISTING|FA_WRITE|FA_READ );
    if ( result == FR_OK )
    { /*  文件定位 */
      result = f_lseek(&fnew, ((&fnew)->fsize));        // 定位到文件末尾
      if (result == FR_OK)
      {
        printf("！！ 文件定位成功。(%d)\r\n",result);
        f_printf(&fnew,WriteBuffer);
        /*  文件定位到文件起始位置 */
        result = f_lseek(&fnew,0);
        /* 读取文件所有内容到缓存区 */
        result = f_read(&fnew,ReadBuffer,f_size(&fnew),&fnum);
        if(result == FR_OK)
        {
          printf("》文件内容：\n%s\n",ReadBuffer);
        }
      }
      else
      {
        printf("！！ 文件定位失败。(%d)\r\n",result);
      }
    }
    else
    {
      printf("!! 打开文件失败：%d\n",result);
    }
  }
  
  f_close(&fnew);  
}


/*
*********************************************************************************************************
*	函 数 名: DeleteDirFile
*	功能说明: 删除SD卡根目录下的 armfly.txt 文件和 Dir1，Dir2 目录
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
 ;
  
  result = f_mount(&fs,"1:",1);
  if(result!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",result);
  }
  else
  {
    /* 删除文件 MeterLib.txt */  
    result = f_unlink("1:MeterLib.txt");
    result = f_unlink("1:FatFs读写测试文件.txt");
    if (result == FR_OK)
    {
      printf("删除文件 MeterLib.txt 成功\r\n");
    }
    else if (result == FR_NO_FILE)
    {
      printf("没有发现文件或目录 :%s\r\n", "armfly.txt");
    }
    else
    {
      printf("删除MeterLib.txt失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }
  }

  /* 卸载文件系统 */
  f_mount(NULL, "1:", 1);
}






///*
//*********************************************************************************************************
//*	函 数 名: WriteFileTest
//*	功能说明: 测试文件读写速度
//*	形    参：无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
////static void WriteFileTest(void)
////{
//  /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
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
//  /* 挂载文件系统 */
//  result = f_mount(&fs, FS_VOLUME_SD, 0);			/* Mount a logical drive */
//  if (result != FR_OK)
//  {
//    printf("挂载文件系统失败 (%d)\r\n", result);
//  }
//  
//  /* 打开根文件夹 */
//  result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
//  if (result != FR_OK)
//  {
//    printf("打开根目录失败 (%d)\r\n", result);
//    return;
//  }
//  
//  /* 打开文件 */
//  sprintf(TestFileName, "Speed%02d.txt", s_ucTestSn++);		/* 每写1次，序号递增 */
//  result = f_open(&file, TestFileName, FA_CREATE_ALWAYS | FA_WRITE);
//  
//  /* 写一串数据 */
//  printf("开始写文件%s %dKB ...\r\n", TestFileName, TEST_FILE_LEN / 1024);
//  runtime1 = bsp_GetRunTime();	/* 读取系统运行时间 */
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
//      printf("%s文件写失败\r\n", TestFileName);
//      break;
//    }
//  }
//  runtime2 = bsp_GetRunTime();	/* 读取系统运行时间 */
//  
//  if (err == 0)
//  {
//    timelen = (runtime2 - runtime1);
//    printf("\r\n  写耗时 : %dms   平均写速度 : %dB/S (%dKB/S)\r\n",
//           timelen,
//           (TEST_FILE_LEN * 1000) / timelen,
//           ((TEST_FILE_LEN / 1024) * 1000) / timelen);
//  }
//  
//  f_close(&file);		/* 关闭文件*/
//  
//  
//  /* 开始读文件测试 */
//  result = f_open(&file, TestFileName, FA_OPEN_EXISTING | FA_READ);
//  if (result !=  FR_OK)
//  {
//    printf("没有找到文件: %s\r\n", TestFileName);
//    return;
//  }
//  
//  printf("开始读文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);
//  runtime1 = bsp_GetRunTime();	/* 读取系统运行时间 */
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
//      /* 比较写入的数据是否正确，此语句会导致读卡速度结果降低到 3.5MBytes/S */
//      for (k = 0; k < sizeof(g_TestBuf); k++)
//      {
//        if (g_TestBuf[k] != (k / 512) + '0')
//        {
//          err = 1;
//          printf("Speed1.txt 文件读成功，但是数据出错\r\n");
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
//      printf("Speed1.txt 文件读失败\r\n");
//      break;
//    }
//  }
//  runtime2 = bsp_GetRunTime();	/* 读取系统运行时间 */
//  
//  if (err == 0)
//  {
//    timelen = (runtime2 - runtime1);
//    printf("\r\n  读耗时 : %dms   平均读速度 : %dB/S (%dKB/S)\r\n", timelen,
//           (TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
//  }
//  
//  /* 关闭文件*/
//  f_close(&file);
//  
//  /* 卸载文件系统 */
//  f_mount(NULL, FS_VOLUME_SD, 0);
//}
//
