#include "includes.h"



/* 文件系统 变量部分 */
DIR DirInf exram;
FILINFO FileInf exram;

FATFS *pfs exram;
DWORD fre_clust exram, fre_sect exram, tot_sect exram;
FIL fnew exram;     /* 文件对象 */
UINT fnum exram;    /* 文件成功读写数量 */
FATFS fs exram;       /* FatFs文件系统对象 */      
FRESULT result exram;              /* 文件操作结果 */ 

 
/* 集中器 全局变量  不需保存在文件系统部分 */
uint32_t  SysState = 0;
Str_SMS_Buf SMSbuf;     //SMS消息存放
Str_TCP_Buf TCPbuf;     //TCP消息存放

/* 集中器 全局变量  需保存在文件系统部分 */

Str_MeterLib_Parm MeterLib[MeterLibMaxNum] exram;

uint8_t  N720MessageNum[300] exram;//存放上行消息数量 最多支持256条数据 每条数据的长度存在其中
uint8_t  N720MessageQue[32768] exram; //存放上行消息数据 

uint16_t GW3761DatFrameSeqNum = 0;

Str_ConfigInfo_PARM ConfigInfo;

void GateWay_Variable_Init(void)
{
  ReadGateWayConfigInfoFile();
  ReadGateWayMeterLibFile();
}

 
 


/*
*********************************************************************************************************
*	函 数 名: DiskFormatting
*	功能说明: 磁盘格式化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void DiskFormatting(void)
 {
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
     if (result != FR_OK)
     {
       printf("挂载文件系统失败 (%d)\r\n", result);
     }
     else
     {
       printf("》FLASH已成功挂载文件系统。\r\n");
     }
 
   }
   else
   {
     printf("《《格式化失败。》》\r\n");
   }
   /* 卸载文件系统 */
 }


/*
*********************************************************************************************************
*	函 数 名: CreateGateWayIDFile
*	功能说明: 创建
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CreateGateWayConfigInfoFile(void)
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
    /* 打开/创建文件 */
    printf("\r\n****** 即将打开/GateWayConfigInfo.txt文件******\r\n");	
    result = f_open(&fnew, "1:GateWayConfigInfo.txt",FA_CREATE_ALWAYS | FA_WRITE );//注意文件名 1：
    if(result == FR_OK)
    {
      result=f_write(&fnew,&ConfigInfo ,sizeof(ConfigInfo),&fnum);/* 写一串数据 */
      if (result == FR_OK)
      {
        printf("GateWayID.txt 文件写入成功\r\n");
      }
      else
      {
        printf("GateWayID.txt 文件写入失败\r\n");
      }
    }
    else
    {
      printf("！！打开/创建GateWayID.txt文件文件失败：(%d)\n",result);
    }
    
  }
  /* 关闭文件*/
  f_close(&fnew);
}


/*
*********************************************************************************************************
*	函 数 名: ReadGateWayIDFile
*	功能说明:  
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ReadGateWayConfigInfoFile(void)
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
    /* 打开文件 */
    result = f_open(&fnew, "1:GateWayConfigInfo.txt",FA_OPEN_EXISTING | FA_READ );//注意 FA_READ
    if (result !=  FR_OK)
    {
      printf("没有找到 : GateWayConfigInfo.txt\r\n");
    }
    else
    {
      result = f_read(&fnew, &ConfigInfo, sizeof(ConfigInfo), &fnum); 
      if(result == FR_OK)
      {
 
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

 
/*
*********************************************************************************************************
*	函 数 名: CreateGateWayIDFile
*	功能说明: 创建
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CreateGateWayMeterLibFile(void)
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
    /* 打开/创建文件 */
    printf("\r\n****** 即将打开/创建 GateWayMeterLib.txt文件******\r\n");	
    result = f_open(&fnew, "1:GateWayMeterLib.txt",FA_CREATE_ALWAYS | FA_WRITE );//注意文件名 1：
    if(result == FR_OK)
    {
      result=f_write(&fnew,MeterLib,sizeof(MeterLib),&fnum);/* 写一串数据 */
      if (result == FR_OK)
      {
        printf("GateWayMeterLib.txt 文件写入成功\r\n");
      }
      else
      {
        printf("GateWayMeterLib.txt 文件写入失败\r\n");
      }
    }
    else
    {
      printf("！！打开/创建 GateWayMeterLib.txt文件文件失败：(%d)\n",result);
    }
    
  }
  /* 关闭文件*/
  f_close(&fnew);
}


void ReadGateWayMeterLibFile(void)
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
    /* 打开文件 */
    result = f_open(&fnew, "1:GateWayMeterLib.txt",FA_OPEN_EXISTING | FA_READ );//注意 FA_READ
    if (result !=  FR_OK)
    {
      printf("没有找到 : GateWayMeterLib.txt\r\n");
    }
    else
    {
      result = f_read(&fnew,&MeterLib, sizeof(MeterLib), &fnum); 
      if(result == FR_OK)
      {
        printf("！！文件读取成功：(%d)\n",result);
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






/*
*********************************************************************************************************
*	函 数 名: ViewRootDir
*	功能说明: 显示SD卡根目录下的文件名
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ViewRootDir(void)
{

  uint32_t cnt = 0;
  char lfname[32];
  
  result = f_mount(&fs,"1:",1);
  if(result!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",result);
  }
  else
  {
    printf("！！外部Flash挂载文件系统成功。(%d)\r\n",result);
    /* 打开根文件夹 */
    result = f_opendir(&DirInf, "1:"); /* 如果不带参数，则从当前目录开始 注意“1:”*/
    if (result != FR_OK)
    {
      printf("打开根目录失败 (%d)\r\n", result);
    }
    else
    {
      printf("打开根目录成功 (%d)\r\n", result);
      FileInf.lfname = lfname;/* 读取当前文件夹下的文件和目录 */
      FileInf.lfsize = 256;
      printf("属性 | 文件大小 | 短文件名 | 长文件名\r\n");
      for (cnt = 0; ;cnt++)
      {
        result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
          break;
        }
        
        if (FileInf.fname[0] == '.')
        {
          continue;
        }
        
        /* 判断是文件还是子目录 */
        if (FileInf.fattrib & AM_DIR)
        {
          printf("(0x%02d)目录  ", FileInf.fattrib);
        }
        else
        {
          printf("(0x%02d)文件|", FileInf.fattrib);
        }
        
        /* 打印文件大小, 最大4G */
        printf("%10d | ", FileInf.fsize);
        
        printf("%s |", FileInf.fname);	/* 短文件名 */
        
        printf("%s\r\n", (char *)FileInf.lfname);	/* 长文件名 */
      }
    }
  }
  /* 卸载文件系统 */
  f_mount(NULL, "1:", 1);
}
