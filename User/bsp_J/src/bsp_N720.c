#include "includes.h"




void bsp_InitN720_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 第1步：打开GPIO时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
  /* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	/* 输入浮空模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_RST;
  GPIO_Init(GPIO_PORT_N720_RST, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_PCTRL;
  GPIO_Init(GPIO_PORT_N720_PCTRL, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_ONOFF;
  GPIO_Init(GPIO_PORT_N720_ONOFF, &GPIO_InitStructure);
  
  
  
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
函 数 名: NB_iot_SendAT
功能说明: 向GSM模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
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
  uint8_t ucRxBuf[256]={0};
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
    }
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
1） 若返回 +CGATT:1 ， 则网络附着成功， 可进入下一步；
2） 若返回 +CGATT:0 ， 则循环查询10次， 1s/次；
3） 查询10次后， 仍返回 +CGATT:0 ， 可进行主动附着， 具体步骤为：
A、 先去附着： 发送AT+CGATT=0， 等待返回OK；
B、（去附着返回OK后） 再主动附着： AT+CGATT=1；
*********************************************************************
*/
uint8_t N720_Query_Network_Attach(void)    
{
  uint8_t attach = 0;
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[16] = {0};
  
  N720_SendAT("AT+CGATT?");      //查询网络附着情况
  
  bsp_StartTimer(N720_BT_ID, 300);		/* 使用软件定时器3，作为超时控制 */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
      {
        sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &attach);    //提取附着情况
        if (attach != NULL)
        {
          break;
        }
      }
    }
    /* 检查定时器0时间是否到 */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      attach = 0;
      break;/* 3秒定时到后退出本状态 */
    }
  }
  
  /* 任务结束时，应该关闭定时器，因为他们会占用后台的资源 */
  bsp_StopTimer(N720_BT_ID);	 //单次定时器如果超时到过一次后，可以不必关闭
  
  return(attach);
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
      if( N720_WaitResponse("OK",300) == true)
      {
        flag = 1;
        break;
      }
    }
    else
    {
      flag = 0;
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
  
  N720_SendAT("AT$MYCCID");      //获取 SIM 卡序列号 
  
  bsp_StartTimer(N720_BT_ID, 300);		/* 使用软件定时器3，作为超时控制 */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
      {
        sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, num);    //提取附着情况
        flag = 1;
        break;
      }
      else 
      {
        flag = 0;
      }
    }
    /* 检查定时器0时间是否到 */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      flag = 0;
      break;/* 3秒定时到后退出本状态 */
    }
  }
  
  /* 任务结束时，应该关闭定时器，因为他们会占用后台的资源 */
  bsp_StopTimer(N720_BT_ID);	 //单次定时器如果超时到过一次后，可以不必关闭
  
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
  
  N720_SendAT("AT+CGSN");      //获取 SIM 卡序列号 
  
  bsp_StartTimer(N720_BT_ID, 300);		/* 使用软件定时器3，作为超时控制 */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
      {
        sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, imei);    //提取附着情况
        flag = 1;
        break;
      }
      else 
      {
        flag = 0;
      }
    }
    /* 检查定时器0时间是否到 */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      flag = 0;
      break;/* 3秒定时到后退出本状态 */
    }
  }
  
  /* 任务结束时，应该关闭定时器，因为他们会占用后台的资源 */
  bsp_StopTimer(N720_BT_ID);	 //单次定时器如果超时到过一次后，可以不必关闭
  
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
  for (uint8_t i = 0; i < 5; i++)
  {
    N720_SendAT("AT$MYNETACT?");      //查询网络连接 
    
    bsp_StartTimer(N720_BT_ID, 1000);		/* 使用软件定时器3，作为超时控制 */
    W_State(&SysState,N720RcvOK,false);
    while (1)
    {			
      if(R_State(&SysState,N720RcvOK) == true)
      {
        W_State(&SysState,N720RcvOK,false);
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //回复OK
        {
          sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, ip_str);    //提取ip段
          //flag = 1;
          break;
        }
        else 
        {
          flag = 0;
        }
      }
      /* 检查定时器0时间是否到 */
      if (bsp_CheckTimer(N720_BT_ID))
      {
        flag = 0;
        break;/* 3秒定时到后退出本状态 */
      }
    }
    /* 任务结束时，应该关闭定时器，因为他们会占用后台的资源 */
    bsp_StopTimer(N720_BT_ID);	 //单次定时器如果超时到过一次后，可以不必关闭
    
    if(strstr(ip_str,"0.0.0.0") == NULL)
    {
      flag = 1;
      break;
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
    
    if( (N720_WaitResponse("OK",3000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
uint8_t N720_Connect_Ftp(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint8_t port1, uint8_t port2)
{
  uint8_t flag = 0;
  uint8_t cmd_str[64] = {0};
  B16_B08 Port;
  
  
  Port.B8[0] = port1;
  Port.B8[1] = port2;
  
  snprintf(cmd_str, 64, "AT$MYFTPOPEN=0,\"%d.%d.%d.%d:%d\",\"ftp\",\"ftp\",1,30,1", ip1, ip2, ip3, ip4, Port.B16);
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT(cmd_str);      //打开Ftp连接
    
    if( (N720_WaitResponse("OK",10000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
  
  if( (N720_WaitResponse("$MYFTPOPEN",300) == true))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
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
  
  snprintf(cmd_str, 32, "AT$MYFTPSIZE=\"%s\"", file_name);
  
  N720_SendAT(cmd_str);      //查询Ftp服务器上文件大小
  
  bsp_StartTimer(N720_BT_ID, 1000);		/* 使用软件定时器3，作为超时控制 */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      
      if(strstr(ucRxBuf,"$MYFTPSIZE") != NULL)   //
      {
        sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &file_size);    //提取文件大小信息
        if (file_size != NULL)
        {
          break;
        }
      }
    }
    /* 检查定时器0时间是否到 */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      file_size = 0;
      break;/* 3秒定时到后退出本状态 */
    }
    
  }
  
  /* 任务结束时，应该关闭定时器，因为他们会占用后台的资源 */
  bsp_StopTimer(N720_BT_ID);	 //单次定时器如果超时到过一次后，可以不必关闭
  
  return(file_size);
  
}

/*
**********************************************************************
函 数 名: N720_DownLoad_File
功能说明: 下载数据
形    参: * file_name 文件名字  addr 文件开始地址   size 长度     *buf 下载的内容
返 回 值: true表示成功  false表示失败 
注释：
1） 该指分段令支持分段下载；
当<data_offset>,<data_Length>都缺省时下载整个文件；
当仅缺省<data_Length>时， 下载<data_offset>到文件结尾的全部数据；
2） 若返回 CONNECT ,则开始下载数据数据；
3） 也可使用 +++ 退出FTP上传模式;
3） 发送该指令后， 若返回 ERROR: <错误代码> ,则参考表1再重新下载数据；
*********************************************************************
*/

uint8_t check = 0;
uint8_t check1 = 0;
uint8_t N720_DownLoad_File(uint8_t *file_name, uint32_t addr, uint32_t size, uint8_t *buf)
{
  uint8_t flag = 1;
  uint8_t cmd_str[48] = {0};
  uint8_t ucRxBuf[3*1024] = {0};
  uint16_t data_len = 0;
  uint16_t data_size = 0;
  uint16_t data_size_last = 0;
  snprintf(cmd_str, 48, "AT$MYFTPGET=\"%s\",%d,%d", file_name, addr, size);
  
  N720_SendAT(cmd_str);      //下载数据
  
  
  if( (N720_WaitResponse("CONNECT",1000) == true))
  {
    bsp_StartTimer(N720_BT_ID, 10000);		/* 使用软件定时器3，作为超时控制 */
    W_State(&SysState,N720RcvOK,false);
    comClearRxFifo(COM_N720);	/* 清零串口接收缓冲区 */	
    while (1)
    {			
      if(R_State(&SysState,N720RcvOK) == true)
      {
        W_State(&SysState,N720RcvOK,false);
        data_len = comGetBuf(COM_N720, ucRxBuf);
        
        if (data_len >= size)
        {
          memcpy(&(buf[0]), ucRxBuf, data_len-6);//接收无线的数据帧
          flag = 1;
          break;
        }
        
        else 
        {
          data_size_last = data_size;
          data_size += data_len;
          if (data_size >= size)
          {
            memcpy(&(buf[0 + data_size_last]), ucRxBuf, data_len-6);//接收无线的数据帧
            flag = 1;
            break;
          }
          
          memcpy(&(buf[0 + data_size_last]), ucRxBuf, data_len);//接收无线的数据帧
          
        }
        
        comClearRxFifo(COM_N720);	/* 清零串口接收缓冲区 */	 
      }
      /* 检查定时器0时间是否到 */
      if (bsp_CheckTimer(N720_BT_ID))
      {
        flag = 0;
        break;/* 3秒定时到后退出本状态 */
      }
    }  
  }
  else
  {
    flag = 0;
  }
  bsp_StopTimer(N720_BT_ID);	 //单次定时器如果超时到过一次后，可以不必关闭
  
  return(flag);
  
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
uint8_t N720_Set_TCP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint8_t port1, uint8_t port2)
{
  uint8_t flag = 0;
  uint8_t cmd_str[48] = {0};
  B16_B08 Port;
  
  
  Port.B8[0] = port1;
  Port.B8[1] = port2;
  
  snprintf(cmd_str, 48, "AT$MYNETSRV=0,0,0,0,\"%d.%d.%d.%d:%d\"", ip1, ip2, ip3, ip4, Port.B16);
  
  N720_SendAT(cmd_str);      //设置TCP链路信息
  
  if( (N720_WaitResponse("OK",300) == true))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
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
    if( (N720_WaitResponse("2000",10000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
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
  
  if( (N720_WaitResponse(NULL,300) == true))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
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
uint8_t N720_TCP_Send(uint8_t *dat, uint8_t len)
{
  uint8_t flag = 0;
  uint8_t cmd_str[32] = {0};
  
  
  snprintf(cmd_str, 32, "AT$MYNETWRITE=0,%d", len);
  N720_SendAT(cmd_str);      //发送TCP数据
  
  if( (N720_WaitResponse("$MYNETWRITE",10000) == true))
  {
    N720_SendAT(dat);      //发送数据
    if ( N720_WaitResponse("OK",1000) == true )
    {
      flag = 1;
    }
  }
  else
  {
    flag = 0;
  }
  return(flag);
}




