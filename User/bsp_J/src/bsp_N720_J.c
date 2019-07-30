#include "includes.h"




void bsp_InitN720_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 第1步：打开GPIO时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
  /* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	/* 输入浮空模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_RST;
  GPIO_Init(GPIO_PORT_N720_RST, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_PCTRL;
  GPIO_Init(GPIO_PORT_N720_PCTRL, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_ONOFF;
  GPIO_Init(GPIO_PORT_N720_ONOFF, &GPIO_InitStructure);
  
  N720_RST_GPIO(true);
  N720_PCTRL_GPIO(false);
  N720_ONOFF_GPIO(false);
  GPIOB->BRR  = GPIO_Pin_8;
  
}
 
void N720_RST_GPIO(bool dat)
{
  if(dat == true)
  {
    GPIO_PORT_N720_RST->BSRR = GPIO_PIN_N720_RST;
  }
  else
  {
    GPIO_PORT_N720_RST->BRR  = GPIO_PIN_N720_RST;
  }
}

void N720_PCTRL_GPIO(bool dat)
{
  if(dat == true)
  {
    GPIO_PORT_N720_PCTRL->BSRR = GPIO_PIN_N720_PCTRL;
  }
  else
  {
    GPIO_PORT_N720_PCTRL->BRR  = GPIO_PIN_N720_PCTRL;
  }
}

void N720_ONOFF_GPIO(bool dat)
{
  if(dat == true)
  {
    GPIO_PORT_N720_ONOFF->BSRR = GPIO_PIN_N720_ONOFF;
  }
  else
  {
    GPIO_PORT_N720_ONOFF->BRR  = GPIO_PIN_N720_ONOFF;
  }
}

/*
***********************************************************************
函 数 名: N720_SendAT
功能说明: 向4G模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
形    参: _Str : AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
返 回 值: 无
注    释：\n = 换行 \r = 回车 
strlen所作的仅仅是一个计数器的工作，它从内存的某个位置
（可以是字符串开头，中间某个位置，甚至是某个不确定的内存区域）
开始扫描，直到碰到第一个字符串结束符'\0'为止，然后返回计数器
值(长度不包含'\0')
**********************************************************************
*/

void N720_SendAT(char *_Cmd)
{
  comClearRxFifo(COM_N720);	/* 清零串口接收缓冲区 */	
  comClearTxFifo(COM_N720);	/* 清零串口发送缓冲区 */	
  
  comSendBuf(COM_N720, (uint8_t *)_Cmd, strlen(_Cmd));
  comSendBuf(COM_N720, "\r", 1);
  comSendBuf(COM_N720, "\n", 1);  
}

/*
***********************************************************************
函 数 名: N720_SendData
功能说明: 向4G模块发送数据 一般用于TCP数据透传
形    参: _Str : 需要发送的数据，Len : 数据长度
返 回 值: 无
注    释：\n = 换行 \r = 回车 
**********************************************************************
*/

void N720_SendData(char *_Str, uint8_t Len)
{
  comClearRxFifo(COM_N720);	/* 清零串口接收缓冲区 */	
  comClearTxFifo(COM_N720);	/* 清零串口发送缓冲区 */	
  
  comSendBuf(COM_N720, (uint8_t *)_Str, Len);
  comSendBuf(COM_N720, "\r", 1);
  comSendBuf(COM_N720, "\n", 1);  
}

/*
**********************************************************************
函 数 名: N720_WaitResponse
功能说明: 等待N720返回指定的应答字符串. 比如等待 OK
形    参: reply  : 应答的字符串， 长度不得超过255
TimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
返 回 值: 1 表示成功  0 表示失败
*********************************************************************
*/
uint8_t N720_WaitResponse(uint8_t *reply,uint16_t TimeOut)
{
  static uint8_t ucRxBuf[256]={0};
  INT8U   err;
  
  OSSemPend(SempN720Rx, TimeOut, &err);
    
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,reply) != NULL)
    {
      return(true);
    }
  }
   
  return(false);
}

/*
**********************************************************************
函 数 名: N720_Check_Report
功能说明: 测试模块串口通讯 
形    参: 无  
返 回 值: true表示成功  false 表示失败 
*********************************************************************
*/
uint8_t N720_Check_Report(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT");      //AT测试 
    if( N720_WaitResponse("OK",1000) == true)
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Forbidden_Command_Display
功能说明: 关闭回显功能 
形    参: 无  
返 回 值: true表示成功  false 表示失败 
*********************************************************************
*/
uint8_t N720_Forbidden_Command_Display(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("ATE0");      //关闭回显
    if( N720_WaitResponse("OK",1000) == true)
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}


/*
**********************************************************************
函 数 名: N720_Check_SIMCard
功能说明: 检查SIM卡状态 
形    参: 无  
返 回 值: true表示成功  false 表示失败 
*********************************************************************
*/
uint8_t N720_Check_SIMCard(void)    //部分SIM卡初始化需要的时间较久，可循环查询5次，1s/次
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<5;num++)
  {
    N720_SendAT("AT+CPIN?");      //检查SIM卡状态
    if( N720_WaitResponse("READY",1000) == true)
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Signal_Power
功能说明: 读取信号强度 
形    参: 无  
返 回 值: 信号强度 （0-31）
注    释：
1） 若返回 +CSQ: 99,99 ,表明无信号。 则循环查询40次， 1s/次；
2） 若返回 +CSQ: 0< X <=31,则有信号,建议循环查询：X < 12时，再进入下一步;
3） 若查询40次后仍无信号， 则返回 ： 检查天线， 重启模块。
*********************************************************************
*/
uint8_t N720_Signal_Power(void)    //部分SIM卡初始化需要的时间较久，可循环查询5次，1s/次；
{
  uint8_t rssi = 0;
  uint8_t ucRxBuf[32] = {0};
  INT8U   err;
  char cmd_str[16] = {0};
  
  N720_SendAT("AT+CSQ");      //读取信号强度
  OSSemPend(SempN720Rx, 5000, &err);
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
    {
      sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &rssi);    //提取信号强度
    }
  }
  
  return(rssi);
}

/*
**********************************************************************
函 数 名: N720_Query_Network_Registration
功能说明: 查询网络注册情况 
形    参: 无  
返 回 值: 网络注册情况 （1-5）
注    释：
1） 若返回: +CREG: 0,1"或 +CREG: 0,5"表示模块已注册上CS域， 可进入下一步；
2） 若返回 +CREG:0,<0,2,3,4>"， 则未注册上CS域， 循环查询40次， 1s/次；
3） 若循环查询40次后仍注册不上网络， 返回步骤 ： 重启模块；
*********************************************************************
*/
uint8_t N720_Query_Network_Registration(void)    
{
  uint8_t log = 0;
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[32] = {0};
  INT8U   err;
  
  N720_SendAT("AT+CREG?");      //查询网络注册情况
  OSSemPend(SempN720Rx, 5000, &err);
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
    {
      sscanf(ucRxBuf, "%[^,],%d", cmd_str, &log);    //注册情况
      if (log == 1 || log == 5)
      {
          log = true;
      }
      else 
      {
          log = false;
      }
    } 
    else 
    {
        log = false;
    }
  }
  
  else 
  {
      log = false;
  }

  return(log);
}


/*
**********************************************************************
函 数 名: N720_Query_Network_Attach
功能说明: 查询网络附着情况 
形    参: 无  
返 回 值: 网络附着情况 （0-1）
注    释：
1） 若返回 +CGATT: 1 ， 则网络附着成功， 可进入下一步；
2） 若返回 +CGATT: 0 ， 则循环查询10次， 1s/次；
3） 查询10次后， 仍返回 +CGATT:0 ， 可进行主动附着， 具体步骤为：
A、 先去附着： 发送AT+CGATT=0， 等待返回OK；
B、（去附着返回OK后） 再主动附着： AT+CGATT=1；
*********************************************************************
*/
uint8_t N720_Query_Network_Attach(void)    
{
    uint8_t flag = 0;
    uint8_t attach = 0;
    uint8_t ucRxBuf[128] = {0};
    char cmd_str[16] = {0};
    INT8U   err;
    
    N720_SendAT("AT+CGATT?");      //查询网络附着情况
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {       
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
        {
            sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &attach);    //提取附着情况
            if (attach == 1)
            {
                flag = true;
            }
            else 
            {
                flag = false;
            }
        }
        else 
        {
            flag = false;
        }
    }
    else 
    {
        flag = false;
    }
    
    return(flag);
}

/*
**********************************************************************
函 数 名: N720_Active_Attachment
功能说明: 模块主动进行网络附着 
形    参: 无  
返 回 值: true表示成功  false 表示失败
注    释：先去附着，再进行附着
*********************************************************************
*/
uint8_t N720_Active_Attachment(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT+CGATT=0");      //网络去附着
    if( N720_WaitResponse("OK",300) == true)
    {
      N720_SendAT("AT+CGATT=1");      //网络附着
      if( N720_WaitResponse("OK",3000) == true)
      {
        flag = true;
        break;
      }
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}



/*
**********************************************************************
函 数 名: N720_Get_Serial_Number
功能说明: 获取 SIM 卡序列号 
形    参: num  
返 回 值: true表示成功  false 表示失败 
注    释：
*********************************************************************
*/
uint8_t N720_Get_Serial_Number(uint8_t *num)    
{
    uint8_t ucRxBuf[128] = {0};
    char cmd_str[16] = {0};
    uint8_t flag = 0;
    INT8U   err;

    N720_SendAT("AT$MYCCID");      //获取 SIM 卡序列号 
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
        {
            sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, num);    //提取附着情况
            flag = 1;
        }
        else 
        {
            flag = 0;
        }
    }
    
    return flag;
}

/*
**********************************************************************
函 数 名: N720_Get_IMEI
功能说明: 获取模块IMEI号 
形    参: *imei  
返 回 值: true表示成功  false 表示失败 
注    释：
*********************************************************************
*/
uint8_t N720_Get_IMEI(uint8_t *imei)    
{
    uint8_t ucRxBuf[128] = {0};
    char cmd_str[16] = {0};
    uint8_t flag = 0;
    INT8U   err;    
    
    N720_SendAT("AT+CGSN");      //获取 SIM 卡序列号 
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
        {
            sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, imei);    //提取附着情况
            flag = 1;
        }
        else 
        {
            flag = 0;
        }
    }
    
    return flag;
}


/*
**********************************************************************
函 数 名: N720_Set_APN
功能说明: 设置APN参数 
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 根据模块所使用的SIM卡， 设置对应的APN
*********************************************************************
*/
uint8_t N720_Set_APN(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
#if     SIM_Operator == Mobile
    N720_SendAT("AT$MYNETCON=0,\"APN\",\"CMNET\"");      //设置APN参数
#else
    N720_SendAT("AT$MYNETCON=0,\"APN\",\"3gnet\"");      //设置APN参数
#endif
    
    if( N720_WaitResponse("OK",300) == true)
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Set_USERPWD
功能说明: 设置APN用户名和密码 
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 设置数据业务用户名和密码， 不同地区数据业务用户名和密码不同；
2） 此条指令必须设置；
*********************************************************************
*/
uint8_t N720_Set_USERPWD(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETCON=0,\"USERPWD\",\"gsm,1234\"");      //设置用户名和密码
    
    if( N720_WaitResponse("OK",300) == true)
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Enable_Reporting
功能说明: 开启主动上报 
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 在内部协议栈下， 开启主动上报功能；
2） 模块会自动上报网络激活、 数据读取、 链路断开等信息；
*********************************************************************
*/
uint8_t N720_Enable_Reporting(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETURC=1");      //开启主动上报
    
    if( N720_WaitResponse("OK",300) == true)
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Network_Connection
功能说明: 激活网络连接
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 激活连接后等待回码， 期间不要执行其他任何操作， 超时时间为30s;
2） 若返回OK则表示指令执行成功， 需等待后续的回码， 以判断激活网络连接成功与否；
3） 若返回$MYURCACT: 0,1,"IP"或者ERROR 902， 则激活网络连接成功,可进入下一步；
4） 若超时时间到仍未返回$MYURCACT: 0,1,"IP"， 则继续激活3次， 10s/次；
5） 若重新激活3次仍未获得IP， 则重启模块；
*********************************************************************
*/
uint8_t N720_Network_Connection(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETACT=0,1");      //激活网络连接
    
    if( (N720_WaitResponse("$MYURCACT",10000) == true))
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}


/*
**********************************************************************
函 数 名: N720_Check_Connection
功能说明: 查询网络连接
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 若返回$MYNETACT: 0,1,"IP"， 则网络连接已激活， 且未断开；
2） 若返回$MYNETACT: 0,1,"0,0,0,0"， 则网络连接暂未激活， 可继续查询5次， 1s/次；
3） 若查询5次， 仍返回$MYNETACT: 0,1,"0,0,0,0"， 则重新激活网络连接；
*********************************************************************
*/
uint8_t N720_Check_Connection(void)
{
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[16] = {0};
  char ip_str[16] = {0};
  uint8_t flag = 0;
  INT8U   err;    

  for (uint8_t i = 0; i < 5; i++)
  {
    N720_SendAT("AT$MYNETACT?");      //查询网络连接 
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
        {
            sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, ip_str);    //提取ip段
            if(strstr(ip_str,"0.0.0.0") != NULL)
            {
                flag = false;
            }
            else 
            {
                flag = true;
                break;
            }
            
        }
        else 
        {
            flag = false;
        }
        
    }
    
  }
  return flag;
}

/*
**********************************************************************
函 数 名: N720_Close_Ftp
功能说明: 关闭Ftp连接
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 先关一下这个FTP服务,避免建立FTP连接失败；
2） 此条指令不对回码进行判断， 收到回码后， 延时5-20s即可进入下一步；
*********************************************************************
*/
uint8_t N720_Close_Ftp(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYFTPCLOSE");      //关闭Ftp连接
    
    if( (N720_WaitResponse("OK",6000) == true))
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Connect_Ftp
功能说明: 打开Ftp连接
形    参: ip1，ip2，ip3，ip4，port1，port2  
返 回 值: true表示成功  false 表示失败 
注释：
1） 开启服务后等待回码， 超时时间为10s， 期间不要执行其他任何操作；
2） 若返回 OK ,表示开启服务成功， 可以进入下一步；
3） 若返回 ERROR:<错误代码> ， 参考表1重新开启服务（2s/次）， 重复开启3次；
4） 若重复开启3次仍失败， 则返回步骤 查询模块是否激活网络连接；
*********************************************************************
*/
uint8_t N720_Connect_Ftp(uint8_t *ip_port, uint8_t *ftp_name, uint8_t *ftp_pwd)
{
  uint8_t flag = 0;
  static uint8_t cmd_str[64] = {0};
  
  snprintf(cmd_str, 64, "AT$MYFTPOPEN=0,\"%s\",\"%s\",\"%s\",1,30,1", ip_port, ftp_name, ftp_pwd);
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT(cmd_str);      //打开Ftp连接
    
    if( (N720_WaitResponse("OK",10000) == true))
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  
  return(flag);
}


/*
**********************************************************************
函 数 名: N720_Check_Ftp_Connect
功能说明: 查询FTP服务开启状态
形    参: 
返 回 值: true表示成功  false 表示失败 
注释：
1） 若返回 $MYFTPOPEN:0 则表示FTP连接已经建立， 可以进入下一步；
2） 若返回 ERROR 则表示FTP连接未建立， 需重新建立连接；
*********************************************************************
*/
uint8_t N720_Check_Ftp_Connect(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYFTPOPEN?");      //查询FTP服务开启状态
  
  if( (N720_WaitResponse("$MYFTPOPEN",1000) == true))
  {
    flag = true;
  }
  else
  {
    flag = false;
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_Check_File_Size
功能说明: 查询Ftp服务器上文件大小
形    参: * file_name 文件名字   
返 回 值: 大于1表示成功  false表示失败 
注释：
1） 若返回 $MYFTPSIZE： <File_length> ,则成功获取FTP文件大小， 可进入下一步：
2） 若返回 ERROR ， 则需要继续查询10次（1s/次）；
3） 若查询10次， 仍未返回 $MYFTPSIZE： <File_length> ， 则返回查询FTP连接状态；
*********************************************************************
*/
uint32_t N720_Check_File_Size(uint8_t *file_name)
{
    uint32_t file_size = 0;
    uint8_t cmd_str[32] = {0};
    uint8_t ucRxBuf[128] = {0};
    INT8U   err;    
    
    snprintf(cmd_str, 32, "AT$MYFTPSIZE=\"%s\"", file_name);
    
    N720_SendAT(cmd_str);      //查询Ftp服务器上文件大小
    OSSemPend(SempN720Rx, 5000, &err);
    if( err != OS_ERR_TIMEOUT )
    {        
        comGetBuf(COM_N720, ucRxBuf);
        
        if(strstr(ucRxBuf,"$MYFTPSIZE") != NULL)   //
        {
            sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &file_size);    //提取文件大小信息
            if (file_size != NULL)
            {
                return(file_size);
            }
            else 
            {
                return 0;
            }
        }
        else 
        {
            return 0;
        }
    }
    
    return(file_size);
}

/*
**********************************************************************
函 数 名: N720_DownLoad_File
功能说明: 下载数据
形    参: * file_name 文件名字  addr 文件开始地址   size 长度     *buf 下载的内容
返 回 值: 文件字节数  false表示失败 
注释：
1） 该指分段令支持分段下载；
当<data_offset>,<data_Length>都缺省时下载整个文件；
当仅缺省<data_Length>时， 下载<data_offset>到文件结尾的全部数据；
2） 若返回 CONNECT ,则开始下载数据数据；
3） 也可使用 +++ 退出FTP上传模式;
3） 发送该指令后， 若返回 ERROR: <错误代码> ,则参考表1再重新下载数据；
*********************************************************************
*/
uint32_t N720_DownLoad_File(uint8_t *buf)
{
    uint16_t data_len = 0;
    char *addr = 0;
                
        if(R_State(&SysState,N720RcvOK) == true)
        {
            W_State(&SysState,N720RcvOK,false);           
            
            data_len = comGetBuf(COM_N720, buf);       
            addr = strstr(buf,"CONNECT");
            //*buf = (char*)(addr + 7 + 2);   /********7是CONNECT长度 2是\n\r******/
            if (data_len >= 6)
            {
                /********7是CONNECT长度 2是\n\r******/
                memcpy(buf, &*(addr + 7 + 2), data_len-6);//接收无线的数据帧
                printf("\ndata_len:%d", data_len);    
            }
            else 
            {
                data_len = false;
            }
        } 
        else 
        {
            data_len = false;
        }
    return(data_len);    
}




/*
**********************************************************************
函 数 名: N720_Set_TCP
功能说明: 设置TCP链路信息
形    参: ip1，ip2，ip3，ip4，port1，port2  
返 回 值: true表示成功  false 表示失败 
注释：
1）发送该指令若返回“OK”，则指令发送成功，进入下一步
*********************************************************************
*/
uint8_t N720_Set_TCP(uint8_t *ip_port)
{
  uint8_t flag = 0;
  uint8_t cmd_str[48] = {0};
  
  snprintf(cmd_str, 48, "AT$MYNETSRV=0,0,0,0,\"%s\"", ip_port);
  
  for(uint8_t num = 0; num < 3; num++)
  {
      N720_SendAT(cmd_str);      //设置TCP链路信息
      
      if( (N720_WaitResponse("OK",1000) == true))
      {
          flag = true;
          break;
      }
      else
      {
          flag = false;
      }
  }
  
  return(flag);
}
/*
**********************************************************************
函 数 名: N720_Open_TCP
功能说明: 打开TCP连接
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
1） 若返回 $MYNETOPEN: 0,2000 则表示TCP连接已经建立， 可以进入下一步；
2） 若返回 ERROR 则表示FTP连接未建立，需重新建立连接；
*********************************************************************
*/
uint8_t N720_Open_TCP(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYNETOPEN=0");      //打开TCP连接
  
  for(uint8_t num=0;num<3;num++)
  {
    if( (N720_WaitResponse("2000",3000) == true))
    {
      flag = true;
      break;
    }
    else
    {
      flag = false;
    }
  }
  return(flag);
}


/*
**********************************************************************
函 数 名: N720_Close_TCP
功能说明: 关闭TCP连接
形    参: 无  
返 回 值: true表示成功  false 表示失败 
注释：
*********************************************************************
*/
uint8_t N720_Close_TCP(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYNETCLOSE=0");      //关闭TCP连接
  
  if( (N720_WaitResponse(NULL,3000) == true))
  {
    flag = true;
  }
  else
  {
    flag = false;
  }
  return(flag);
}


/*
**********************************************************************
函 数 名: N720_TCP_Send
功能说明: 发送TCP数据
形    参:  *buf 要发送的内容  len 数据长度 
返 回 值: true表示成功  false 表示失败 
注释：
*********************************************************************
*/
uint8_t N720_TCP_Send(uint8_t *dat, uint16_t len)
{
    uint8_t flag = 0;
    uint8_t cmd_str[24] = {0};
    
    if(len >= 256)       
    {
        return false;
    }
    else
    {
        snprintf(cmd_str, 24, "AT$MYNETWRITE=0,%d", len);
        N720_SendAT(cmd_str);      //发送TCP数据
        
        if( (N720_WaitResponse("$MYNETWRITE",3000) == true))
        {
            N720_SendData(dat, len);      //发送数据
            if ( N720_WaitResponse("OK",3000) == true )
            {
                flag = true;
            }
        }
        else
        {
            flag = false;
        }        
    }
    return(flag);
}

/*
**********************************************************************
函 数 名: N720_TCP_RX
功能说明: 接收TCP消息
形    参:  *buf 接收缓存区 *len 接收数据长度
返 回 值: Get_TCP_Success表示成功 TCP_Error表示数据错误 Get_TCP_Fail表示失败 
注释：
*********************************************************************
*/
Enum_Message_JudgeAndGet N720_TCP_RX(uint8_t *buf, uint8_t *len)
{
    static char ucRxBuf[300] = {0}; //串口数据存放
    uint8_t buf_str[128] = {0};     //无用信息存放

    INT8U   err;    
    static uint16_t TCP_Msg_Len = 0;
    char *addr = 0;
    uint8_t AT_CR_LF[2] = {0x0D, 0x0A};     //回车和换行符号
    
    N720_SendAT("AT$MYNETREAD=0,128");      //接收TCP数据
    
    OSSemPend(SempN720Rx, 5000, &err);    
    if( err != OS_ERR_TIMEOUT )
    {        
        comGetBuf(COM_N720, ucRxBuf);
        sscanf(ucRxBuf, "%[^,],%d", buf_str, &TCP_Msg_Len);
        *len = TCP_Msg_Len;
        if(TCP_Msg_Len > 0)     //数据不为空
        {
            addr = strstr(&ucRxBuf[2], AT_CR_LF);
            /*2是CR和LF长度*/
            if(addr != NULL)
            {
                memcpy(buf, &*(addr + 2), *len);//接收无线的数据帧  
                return Get_TCP_Success;		//获取数据成功
            }
            else
            {
                return TCP_Error;		//没有找到回车换行 获取数据错误
            }
        }
        else
        {
            return TCP_Error;		//数据长度为0 获取数据错误
        }
    }
    
    else 
    {
        return Get_TCP_Fail;		//超时 获取数据失败
    }
}

/**********************************************************************
函 数 名: N720_Set_SMS_Mode
功能说明: 设置短信模式
形    参: 
返 回 值: true表示成功  false 表示失败 
*********************************************************************
*/
uint8_t N720_Set_SMS_Mode(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CMGF=1");      //设置短信模式为文本模式
  
  if( (N720_WaitResponse("OK",1000) == true))
  {
    flag = true;
  }
  else
  {
    flag = false;
  }
  
  return(flag);
}

/**********************************************************************
函 数 名: N720_Set_Char
功能说明: 设置字符集
形    参: 
返 回 值: true表示成功  false 表示失败 
*********************************************************************
*/
uint8_t N720_Set_Char(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CSCS=\"GSM\"");      //设置字符集
  
  if( (N720_WaitResponse("OK",1000) == true))
  {
    flag = true;
  }
  else
  {
    flag = false;
  }
  
  return(flag);
}

/**********************************************************************
函 数 名: N720_SMSReporting_Mode
功能说明: 设置短信到来主动上报模式
形    参: 
返 回 值: true表示成功  false 表示失败 
注释：新短信指示代码格式为+CMTI: "MT",<index>,短信内容存贮而不直接显示
***********************************************************************
*/
uint8_t N720_Reporting_Mode(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CNMI=2,1,0,0,0");      //设置主动上报模式
  
  if( (N720_WaitResponse("OK",1000) == true))
  {
    flag = true;
  }
  else
  {
    flag = false;
  }
  
  return(flag);
}

/**********************************************************************
函 数 名: N720_SMS_Delete
功能说明: 删除所有短信
形    参: 
返 回 值: true表示成功  false 表示失败 
注释：
***********************************************************************
*/
uint8_t N720_SMS_Delete(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CMGD=0,4");      //删除所有短信
  
  if( (N720_WaitResponse("OK",3000) == true))
  {
    flag = true;
  }
  else
  {
    flag = false;
  }
  
  return(flag);
}

/*
**********************************************************************
函 数 名: N720_SMS_Send
功能说明: 发送短信
形    参: *buf 需要发送的内容  
返 回 值: true表示成功  false 表示失败 
注释：
*********************************************************************
*/
uint8_t N720_SMS_Send(uint8_t *buf)
{
  uint8_t flag = 0;
  uint8_t cmd_str[32] = {0};
  uint8_t message[128] = {0};
  uint8_t str[1] = {0x1A};
  
  snprintf(cmd_str, 48, "AT+CMGS=\"%s\"", Phone_Number);
  
  N720_SendAT(cmd_str);      //设置接收短信手机号

  if( (N720_WaitResponse(">",1000) == true))
  {
      snprintf(message, 128, "%s%s", buf, str);
      
      N720_SendAT(message);      //发送短信内容
      if(N720_WaitResponse(str,1000) == true)
      {
          if(N720_WaitResponse("+CMGS:",3000) == true)
          {
              flag = true;
          }
          else 
          {
              flag = false;
          }
      }
      else 
      {
          flag = false;
      }      
  }
  else
  {
    flag = false;
  }
  
  return(flag);
}


/*********************************************************************************
函 数 名: N720_SMS_RX
功能说明: 获取短信内容 存放到数组中
形    参: State 当前短信编号 *buf 存放数据
返 回 值: Get_SMS_Success表示成功 SMS_Error表示错误短信内容 Get_SMS_Fail表示失败 
注释：新短信指示代码格式为+CMTI: "MT",<index>,短信内容存贮而不直接显示
**********************************************************************************
*/
Enum_Message_JudgeAndGet N720_SMS_RX(uint8_t state, uint8_t *buf, uint8_t *len)
{
    static char ucRxBuf[255] = {0};//{0x11,0x22,0x31,0x39,0x39,0x31,0x30,0x36,0x31,0x35,0x32,0x31,0x37,0x31,0x32,0x31,0x2e,0x34,0x33,0x2e,0x31,0x39,0x36,0x2e,0x31,0x31,0x36,0x3a,0x32,0x31,0x0,0x0,0x0,0x0,0x0,0x66,0x74,0x70,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x37,0x39,0x65,0x34,0x66,0x63,0x38,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x74,0x65,0x73,0x74,0x2e,0x62,0x69,0x6e,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    INT8U   err;    
    static uint8_t cmd_str[16] = {0};   //AT指令拼接

    uint8_t SMS_Info_len = 0;       //短信内容长度
    
    snprintf(cmd_str, 16, "AT+CMGR=%d", state);    
    N720_SendAT(cmd_str);         //读取短信
    
    OSSemPend(SempN720Rx, 5000, &err);
    if( err != OS_ERR_TIMEOUT )
    {        
        SMS_Info_len = comGetBuf(COM_N720, ucRxBuf);
        *len = SMS_Info_len;
        //comClearRxFifo(COM_N720);
        if (SMS_Info_len > 6)
        {            
            memcpy(buf, ucRxBuf, SMS_Info_len);//接收SMS的数据帧
            return Get_SMS_Success;     //获取SMS内容成功                       
        }
        else
        {
            return SMS_Error;   //错误SMS内容
        }
    }
    else 
    {
        return Get_SMS_Fail;    //等待消息超时 获取SMS内容失败
    }
}


/**********************************************************************
函 数 名: N720_Judge_Type
功能说明: 判断到来消息类型
形    参: *Num 解析出当前短信编号并放入Num变量中
返 回 值: Null_Type表示无消息 SMS_Type表示短信消息到来  TCP_Type表示TCP消息到来
注释：1、新短信指示代码格式为+CMTI: "MT",<index>,短信内容存贮而不直接显示
	  2、TCP新消息到来指示代码格式为$MYURCREAD: 0
***********************************************************************
*/
Enum_Message_JudgeAndGet N720_Judge_Type(uint8_t *num)
{
    static uint8_t ucRxBuf[32] = {0};    
    static char buf_str[48] = {0};
    uint8_t msg_num = 0;
        
    INT8U   err;    
    
    OSSemPend(SempN720Rx, 100000, &err);
    if( err != OS_ERR_TIMEOUT )
    {        
    	comGetBuf(COM_N720, ucRxBuf);
    	if(strstr(ucRxBuf, "+CMTI") != NULL)
        {
            sscanf(ucRxBuf, "%[^,],%d", buf_str, &msg_num);		//提取当前短信标号
            *num = msg_num;
            return SMS_Type;		//当前消息是SMS
        }
        else if(strstr(ucRxBuf, "$MYURCREAD") != NULL)
        {
            sscanf(ucRxBuf, "%[^ ] %d", buf_str, &msg_num);		//提取当前TCP ID号
            *num = msg_num;
            return TCP_Type;		//当前消息是TCP
        }
        else
        {
            return Null_Type;
        }
    }
    else
    {
        return Null_Type;
    }      
}

/**********************************************************************
函 数 名: SMS_MsgResolve
功能说明: 解析短信操作
形    参: *buf 短信内容
返 回 值:   SMS_Info_Set 配置信息
            SMS_Software_Reset 清除表库
            SMS_Hardware_Reset 硬件复位
            SMS_IAP_Ctrl 进行IAP
            Ctrl_Error   操作码错误
注释：解析出相应操作码并且将数据写入文件系统或者Flash
***********************************************************************
*/
Enum_SMS_Ctrl SMS_MsgResolve(uint8_t *buf, uint8_t len)
{
    static uint8_t BUF[256] = {0};
    uint8_t buf_str[128] = {0};         //无用信息存放
    uint8_t message_buf[128] = {0};     //短信内容存放
    static uint8_t msg_buf[96] = {0};   //配置信息存放
    
    static Union_BootLoader_PARM BootLoader;       
    
    static uint8_t phone_number[12] = {0};  //手机号
    
    static uint8_t msg_pwd[6] = {0};     //通讯密码
    static uint8_t msg_ctrl = 0;         //控制域
    
    static uint8_t ftp_ip_port[22] = {0};   //FTP的ip和端口号
    static uint8_t ftp_name[16] = {0};      //FTP的用户名
    static uint8_t ftp_pwd[16] = {0};       //FTP的密码
    static uint8_t ftp_filename[16] = {0};  //FTP中需要下载的文件名
    
    static uint8_t dev_id[5] = {0};      //设备ID号
    static uint8_t channel = 0;          //信道号
    static uint8_t tcp_ip_port[22] = {0};//TCP IP Port
    
    /*端口号组合成十六进制*/
    static uint16_t port;
    static B16_B08 Port;    
    
    memcpy(BUF, buf, len);
    
    /*****************提取手机号和短信内容**************************/
    sscanf(BUF, "%[^,],\"%[^\"]%[^\n]\n%[^;]", buf_str, phone_number, buf_str, message_buf);    
    
    /*****************判断手机号是否有"+86"*************************/
    if(strstr(phone_number, "+86") != NULL)
    {
        memcpy(BootLoader.Str_BootLoader_PARM.PhoneNum, &phone_number[3], sizeof(phone_number)-3); 
        memcpy(ConfigInfo.PhoneNum, &phone_number[3], sizeof(phone_number)-3);
    }
    else
    {
        memcpy(BootLoader.Str_BootLoader_PARM.PhoneNum, phone_number, sizeof(phone_number));                
        memcpy(ConfigInfo.PhoneNum, phone_number, sizeof(phone_number));         
    }           
    
    /*******************提取通讯密码和控制位***************************/
    sscanf(message_buf, "%[^:]:%[^,],%d,%[^;]", buf_str, msg_pwd, &msg_ctrl, msg_buf);     
    
    
    if(strstr(msg_pwd, MsgPwd) != NULL)     //判断密码是否正确
    {
        if(msg_ctrl == SMS_Info_Set)        //配置信息
        {
            /*****************************提取配置信息****************************/
            sscanf(msg_buf, "%x,%d,%[^,]",dev_id, &channel, tcp_ip_port);
            
            /**************************写配置信息*********************************/
            memcpy(ConfigInfo.DevID, dev_id, sizeof(dev_id));
            ConfigInfo.LoraChannel = channel;
            memcpy(ConfigInfo.TcpIpPort, tcp_ip_port, sizeof(tcp_ip_port));  
            
            /***********************转换ip地址和端口号****************************/
            sscanf(ConfigInfo.TcpIpPort, "%d.%d.%d.%d:%d",&ConfigInfo.IpPort[0], &ConfigInfo.IpPort[1], &ConfigInfo.IpPort[2], &ConfigInfo.IpPort[3], &port);
            Port.B16 = port;
            ConfigInfo.IpPort[4] = Port.B8[1];
            ConfigInfo.IpPort[5] = Port.B8[0];
            
            return SMS_Info_Set;                    
        }              
        
        else if(msg_ctrl == SMS_Software_Reset)
        {
            return SMS_Software_Reset;
        }
        else if(msg_ctrl == SMS_Hardware_Reset)
        {
            return SMS_Hardware_Reset;
        }
        else if(msg_ctrl == SMS_IAP_Ctrl)       //IAP
        {   /**********************写升级标志位***********************/
            BootLoader.Str_BootLoader_PARM.IAPState[0] = 0x11;  
            BootLoader.Str_BootLoader_PARM.IAPState[1] = 0x22;
            
            /*******************提取FTP服务器配置信息*****************/
            sscanf(msg_buf, "%[^,],%[^,],%[^,],%[^,]", ftp_ip_port, ftp_name, ftp_pwd, ftp_filename);   
            
            /*******************写FTP服务器配置信息*******************/
            memcpy(BootLoader.Str_BootLoader_PARM.FtpIpPort, ftp_ip_port, sizeof(ftp_ip_port));
            memcpy(BootLoader.Str_BootLoader_PARM.FtpName, ftp_name, sizeof(ftp_name));
            memcpy(BootLoader.Str_BootLoader_PARM.FtpPwd, ftp_pwd, sizeof(ftp_pwd));
            memcpy(BootLoader.Str_BootLoader_PARM.FtpFileName, ftp_filename, sizeof(ftp_filename));
            
            bsp_WriteCpuFlash(FLASH_IAP_info, BootLoader.buf, 100);
            return SMS_IAP_Ctrl;
        } 
        else
        {
            return Ctrl_Error;
        }        
    }
    else
    {
        return PWD_Error;
    }
}