#include "includes.h"




void bsp_InitN720_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* ��1������GPIOʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
  /* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	/* ���븡��ģʽ */
  
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
�� �� ��: N720_SendAT
����˵��: ��4Gģ�鷢��AT��� �������Զ���AT�ַ���������<CR>�ַ�
��    ��: _Str : AT�����ַ�����������ĩβ�Ļس�<CR>. ���ַ�0����
�� �� ֵ: ��
ע    �ͣ�\n = ���� \r = �س� 
strlen�����Ľ�����һ���������Ĺ����������ڴ��ĳ��λ��
���������ַ�����ͷ���м�ĳ��λ�ã�������ĳ����ȷ�����ڴ�����
��ʼɨ�裬ֱ��������һ���ַ���������'\0'Ϊֹ��Ȼ�󷵻ؼ�����
ֵ(���Ȳ�����'\0')
**********************************************************************
*/

void N720_SendAT(char *_Cmd)
{
  comClearRxFifo(COM_N720);	/* ���㴮�ڽ��ջ����� */	
  comClearTxFifo(COM_N720);	/* ���㴮�ڷ��ͻ����� */	
  
  comSendBuf(COM_N720, (uint8_t *)_Cmd, strlen(_Cmd));
  comSendBuf(COM_N720, "\r", 1);
  comSendBuf(COM_N720, "\n", 1);  
}

/*
***********************************************************************
�� �� ��: N720_SendData
����˵��: ��4Gģ�鷢������ һ������TCP����͸��
��    ��: _Str : ��Ҫ���͵����ݣ�Len : ���ݳ���
�� �� ֵ: ��
ע    �ͣ�\n = ���� \r = �س� 
**********************************************************************
*/

void N720_SendData(char *_Str, uint8_t Len)
{
  comClearRxFifo(COM_N720);	/* ���㴮�ڽ��ջ����� */	
  comClearTxFifo(COM_N720);	/* ���㴮�ڷ��ͻ����� */	
  
  comSendBuf(COM_N720, (uint8_t *)_Str, Len);
  comSendBuf(COM_N720, "\r", 1);
  comSendBuf(COM_N720, "\n", 1);  
}

/*
**********************************************************************
�� �� ��: N720_WaitResponse
����˵��: �ȴ�N720����ָ����Ӧ���ַ���. ����ȴ� OK
��    ��: reply  : Ӧ����ַ����� ���Ȳ��ó���255
TimeOut : ����ִ�г�ʱ��0��ʾһֱ�ȴ�. >����ʾ��ʱʱ�䣬��λ1ms
�� �� ֵ: 1 ��ʾ�ɹ�  0 ��ʾʧ��
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
�� �� ��: N720_Check_Report
����˵��: ����ģ�鴮��ͨѶ 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Check_Report(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT");      //AT���� 
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
�� �� ��: N720_Forbidden_Command_Display
����˵��: �رջ��Թ��� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Forbidden_Command_Display(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("ATE0");      //�رջ���
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
�� �� ��: N720_Check_SIMCard
����˵��: ���SIM��״̬ 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Check_SIMCard(void)    //����SIM����ʼ����Ҫ��ʱ��Ͼã���ѭ����ѯ5�Σ�1s/��
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<5;num++)
  {
    N720_SendAT("AT+CPIN?");      //���SIM��״̬
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
�� �� ��: N720_Signal_Power
����˵��: ��ȡ�ź�ǿ�� 
��    ��: ��  
�� �� ֵ: �ź�ǿ�� ��0-31��
ע    �ͣ�
1�� ������ +CSQ: 99,99 ,�������źš� ��ѭ����ѯ40�Σ� 1s/�Σ�
2�� ������ +CSQ: 0< X <=31,�����ź�,����ѭ����ѯ��X < 12ʱ���ٽ�����һ��;
3�� ����ѯ40�κ������źţ� �򷵻� �� ������ߣ� ����ģ�顣
*********************************************************************
*/
uint8_t N720_Signal_Power(void)    //����SIM����ʼ����Ҫ��ʱ��Ͼã���ѭ����ѯ5�Σ�1s/�Σ�
{
  uint8_t rssi = 0;
  uint8_t ucRxBuf[32] = {0};
  INT8U   err;
  char cmd_str[16] = {0};
  
  N720_SendAT("AT+CSQ");      //��ȡ�ź�ǿ��
  OSSemPend(SempN720Rx, 5000, &err);
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
    {
      sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &rssi);    //��ȡ�ź�ǿ��
    }
  }
  
  return(rssi);
}

/*
**********************************************************************
�� �� ��: N720_Query_Network_Registration
����˵��: ��ѯ����ע����� 
��    ��: ��  
�� �� ֵ: ����ע����� ��1-5��
ע    �ͣ�
1�� ������: +CREG: 0,1"�� +CREG: 0,5"��ʾģ����ע����CS�� �ɽ�����һ����
2�� ������ +CREG:0,<0,2,3,4>"�� ��δע����CS�� ѭ����ѯ40�Σ� 1s/�Σ�
3�� ��ѭ����ѯ40�κ���ע�᲻�����磬 ���ز��� �� ����ģ�飻
*********************************************************************
*/
uint8_t N720_Query_Network_Registration(void)    
{
  uint8_t log = 0;
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[32] = {0};
  INT8U   err;
  
  N720_SendAT("AT+CREG?");      //��ѯ����ע�����
  OSSemPend(SempN720Rx, 5000, &err);
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
    {
      sscanf(ucRxBuf, "%[^,],%d", cmd_str, &log);    //ע�����
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
�� �� ��: N720_Query_Network_Attach
����˵��: ��ѯ���總����� 
��    ��: ��  
�� �� ֵ: ���總����� ��0-1��
ע    �ͣ�
1�� ������ +CGATT: 1 �� �����總�ųɹ��� �ɽ�����һ����
2�� ������ +CGATT: 0 �� ��ѭ����ѯ10�Σ� 1s/�Σ�
3�� ��ѯ10�κ� �Է��� +CGATT:0 �� �ɽ����������ţ� ���岽��Ϊ��
A�� ��ȥ���ţ� ����AT+CGATT=0�� �ȴ�����OK��
B����ȥ���ŷ���OK�� ���������ţ� AT+CGATT=1��
*********************************************************************
*/
uint8_t N720_Query_Network_Attach(void)    
{
    uint8_t flag = 0;
    uint8_t attach = 0;
    uint8_t ucRxBuf[128] = {0};
    char cmd_str[16] = {0};
    INT8U   err;
    
    N720_SendAT("AT+CGATT?");      //��ѯ���總�����
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {       
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
        {
            sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &attach);    //��ȡ�������
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
�� �� ��: N720_Active_Attachment
����˵��: ģ�������������總�� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ��
ע    �ͣ���ȥ���ţ��ٽ��и���
*********************************************************************
*/
uint8_t N720_Active_Attachment(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT+CGATT=0");      //����ȥ����
    if( N720_WaitResponse("OK",300) == true)
    {
      N720_SendAT("AT+CGATT=1");      //���總��
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
�� �� ��: N720_Get_Serial_Number
����˵��: ��ȡ SIM �����к� 
��    ��: num  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע    �ͣ�
*********************************************************************
*/
uint8_t N720_Get_Serial_Number(uint8_t *num)    
{
    uint8_t ucRxBuf[128] = {0};
    char cmd_str[16] = {0};
    uint8_t flag = 0;
    INT8U   err;

    N720_SendAT("AT$MYCCID");      //��ȡ SIM �����к� 
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
        {
            sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, num);    //��ȡ�������
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
�� �� ��: N720_Get_IMEI
����˵��: ��ȡģ��IMEI�� 
��    ��: *imei  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע    �ͣ�
*********************************************************************
*/
uint8_t N720_Get_IMEI(uint8_t *imei)    
{
    uint8_t ucRxBuf[128] = {0};
    char cmd_str[16] = {0};
    uint8_t flag = 0;
    INT8U   err;    
    
    N720_SendAT("AT+CGSN");      //��ȡ SIM �����к� 
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
        {
            sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, imei);    //��ȡ�������
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
�� �� ��: N720_Set_APN
����˵��: ����APN���� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ����ģ����ʹ�õ�SIM���� ���ö�Ӧ��APN
*********************************************************************
*/
uint8_t N720_Set_APN(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
#if     SIM_Operator == Mobile
    N720_SendAT("AT$MYNETCON=0,\"APN\",\"CMNET\"");      //����APN����
#else
    N720_SendAT("AT$MYNETCON=0,\"APN\",\"3gnet\"");      //����APN����
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
�� �� ��: N720_Set_USERPWD
����˵��: ����APN�û��������� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ��������ҵ���û��������룬 ��ͬ��������ҵ���û��������벻ͬ��
2�� ����ָ��������ã�
*********************************************************************
*/
uint8_t N720_Set_USERPWD(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETCON=0,\"USERPWD\",\"gsm,1234\"");      //�����û���������
    
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
�� �� ��: N720_Enable_Reporting
����˵��: ���������ϱ� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ���ڲ�Э��ջ�£� ���������ϱ����ܣ�
2�� ģ����Զ��ϱ����缤� ���ݶ�ȡ�� ��·�Ͽ�����Ϣ��
*********************************************************************
*/
uint8_t N720_Enable_Reporting(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETURC=1");      //���������ϱ�
    
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
�� �� ��: N720_Network_Connection
����˵��: ������������
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� �������Ӻ�ȴ����룬 �ڼ䲻Ҫִ�������κβ����� ��ʱʱ��Ϊ30s;
2�� ������OK���ʾָ��ִ�гɹ��� ��ȴ������Ļ��룬 ���жϼ����������ӳɹ����
3�� ������$MYURCACT: 0,1,"IP"����ERROR 902�� �򼤻��������ӳɹ�,�ɽ�����һ����
4�� ����ʱʱ�䵽��δ����$MYURCACT: 0,1,"IP"�� ���������3�Σ� 10s/�Σ�
5�� �����¼���3����δ���IP�� ������ģ�飻
*********************************************************************
*/
uint8_t N720_Network_Connection(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETACT=0,1");      //������������
    
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
�� �� ��: N720_Check_Connection
����˵��: ��ѯ��������
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ������$MYNETACT: 0,1,"IP"�� �����������Ѽ�� ��δ�Ͽ���
2�� ������$MYNETACT: 0,1,"0,0,0,0"�� ������������δ��� �ɼ�����ѯ5�Σ� 1s/�Σ�
3�� ����ѯ5�Σ� �Է���$MYNETACT: 0,1,"0,0,0,0"�� �����¼����������ӣ�
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
    N720_SendAT("AT$MYNETACT?");      //��ѯ�������� 
    
    OSSemPend(SempN720Rx, 3000, &err);
    if( err != OS_ERR_TIMEOUT )
    {
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
        {
            sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, ip_str);    //��ȡip��
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
�� �� ��: N720_Close_Ftp
����˵��: �ر�Ftp����
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� �ȹ�һ�����FTP����,���⽨��FTP����ʧ�ܣ�
2�� ����ָ��Ի�������жϣ� �յ������ ��ʱ5-20s���ɽ�����һ����
*********************************************************************
*/
uint8_t N720_Close_Ftp(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYFTPCLOSE");      //�ر�Ftp����
    
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
�� �� ��: N720_Connect_Ftp
����˵��: ��Ftp����
��    ��: ip1��ip2��ip3��ip4��port1��port2  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ���������ȴ����룬 ��ʱʱ��Ϊ10s�� �ڼ䲻Ҫִ�������κβ�����
2�� ������ OK ,��ʾ��������ɹ��� ���Խ�����һ����
3�� ������ ERROR:<�������> �� �ο���1���¿�������2s/�Σ��� �ظ�����3�Σ�
4�� ���ظ�����3����ʧ�ܣ� �򷵻ز��� ��ѯģ���Ƿ񼤻��������ӣ�
*********************************************************************
*/
uint8_t N720_Connect_Ftp(uint8_t *ip_port, uint8_t *ftp_name, uint8_t *ftp_pwd)
{
  uint8_t flag = 0;
  static uint8_t cmd_str[64] = {0};
  
  snprintf(cmd_str, 64, "AT$MYFTPOPEN=0,\"%s\",\"%s\",\"%s\",1,30,1", ip_port, ftp_name, ftp_pwd);
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT(cmd_str);      //��Ftp����
    
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
�� �� ��: N720_Check_Ftp_Connect
����˵��: ��ѯFTP������״̬
��    ��: 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ������ $MYFTPOPEN:0 ���ʾFTP�����Ѿ������� ���Խ�����һ����
2�� ������ ERROR ���ʾFTP����δ������ �����½������ӣ�
*********************************************************************
*/
uint8_t N720_Check_Ftp_Connect(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYFTPOPEN?");      //��ѯFTP������״̬
  
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
�� �� ��: N720_Check_File_Size
����˵��: ��ѯFtp���������ļ���С
��    ��: * file_name �ļ�����   
�� �� ֵ: ����1��ʾ�ɹ�  false��ʾʧ�� 
ע�ͣ�
1�� ������ $MYFTPSIZE�� <File_length> ,��ɹ���ȡFTP�ļ���С�� �ɽ�����һ����
2�� ������ ERROR �� ����Ҫ������ѯ10�Σ�1s/�Σ���
3�� ����ѯ10�Σ� ��δ���� $MYFTPSIZE�� <File_length> �� �򷵻ز�ѯFTP����״̬��
*********************************************************************
*/
uint32_t N720_Check_File_Size(uint8_t *file_name)
{
    uint32_t file_size = 0;
    uint8_t cmd_str[32] = {0};
    uint8_t ucRxBuf[128] = {0};
    INT8U   err;    
    
    snprintf(cmd_str, 32, "AT$MYFTPSIZE=\"%s\"", file_name);
    
    N720_SendAT(cmd_str);      //��ѯFtp���������ļ���С
    OSSemPend(SempN720Rx, 5000, &err);
    if( err != OS_ERR_TIMEOUT )
    {        
        comGetBuf(COM_N720, ucRxBuf);
        
        if(strstr(ucRxBuf,"$MYFTPSIZE") != NULL)   //
        {
            sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &file_size);    //��ȡ�ļ���С��Ϣ
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
�� �� ��: N720_DownLoad_File
����˵��: ��������
��    ��: * file_name �ļ�����  addr �ļ���ʼ��ַ   size ����     *buf ���ص�����
�� �� ֵ: �ļ��ֽ���  false��ʾʧ�� 
ע�ͣ�
1�� ��ָ�ֶ���֧�ֶַ����أ�
��<data_offset>,<data_Length>��ȱʡʱ���������ļ���
����ȱʡ<data_Length>ʱ�� ����<data_offset>���ļ���β��ȫ�����ݣ�
2�� ������ CONNECT ,��ʼ�����������ݣ�
3�� Ҳ��ʹ�� +++ �˳�FTP�ϴ�ģʽ;
3�� ���͸�ָ��� ������ ERROR: <�������> ,��ο���1�������������ݣ�
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
            //*buf = (char*)(addr + 7 + 2);   /********7��CONNECT���� 2��\n\r******/
            if (data_len >= 6)
            {
                /********7��CONNECT���� 2��\n\r******/
                memcpy(buf, &*(addr + 7 + 2), data_len-6);//�������ߵ�����֡
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
�� �� ��: N720_Set_TCP
����˵��: ����TCP��·��Ϣ
��    ��: ip1��ip2��ip3��ip4��port1��port2  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�����͸�ָ�������ء�OK������ָ��ͳɹ���������һ��
*********************************************************************
*/
uint8_t N720_Set_TCP(uint8_t *ip_port)
{
  uint8_t flag = 0;
  uint8_t cmd_str[48] = {0};
  
  snprintf(cmd_str, 48, "AT$MYNETSRV=0,0,0,0,\"%s\"", ip_port);
  
  for(uint8_t num = 0; num < 3; num++)
  {
      N720_SendAT(cmd_str);      //����TCP��·��Ϣ
      
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
�� �� ��: N720_Open_TCP
����˵��: ��TCP����
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ������ $MYNETOPEN: 0,2000 ���ʾTCP�����Ѿ������� ���Խ�����һ����
2�� ������ ERROR ���ʾFTP����δ�����������½������ӣ�
*********************************************************************
*/
uint8_t N720_Open_TCP(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYNETOPEN=0");      //��TCP����
  
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
�� �� ��: N720_Close_TCP
����˵��: �ر�TCP����
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
*********************************************************************
*/
uint8_t N720_Close_TCP(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYNETCLOSE=0");      //�ر�TCP����
  
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
�� �� ��: N720_TCP_Send
����˵��: ����TCP����
��    ��:  *buf Ҫ���͵�����  len ���ݳ��� 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
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
        N720_SendAT(cmd_str);      //����TCP����
        
        if( (N720_WaitResponse("$MYNETWRITE",3000) == true))
        {
            N720_SendData(dat, len);      //��������
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
�� �� ��: N720_TCP_RX
����˵��: ����TCP��Ϣ
��    ��:  *buf ���ջ����� *len �������ݳ���
�� �� ֵ: Get_TCP_Success��ʾ�ɹ� TCP_Error��ʾ���ݴ��� Get_TCP_Fail��ʾʧ�� 
ע�ͣ�
*********************************************************************
*/
Enum_Message_JudgeAndGet N720_TCP_RX(uint8_t *buf, uint8_t *len)
{
    static char ucRxBuf[300] = {0}; //�������ݴ��
    uint8_t buf_str[128] = {0};     //������Ϣ���

    INT8U   err;    
    static uint16_t TCP_Msg_Len = 0;
    char *addr = 0;
    uint8_t AT_CR_LF[2] = {0x0D, 0x0A};     //�س��ͻ��з���
    
    N720_SendAT("AT$MYNETREAD=0,128");      //����TCP����
    
    OSSemPend(SempN720Rx, 5000, &err);    
    if( err != OS_ERR_TIMEOUT )
    {        
        comGetBuf(COM_N720, ucRxBuf);
        sscanf(ucRxBuf, "%[^,],%d", buf_str, &TCP_Msg_Len);
        *len = TCP_Msg_Len;
        if(TCP_Msg_Len > 0)     //���ݲ�Ϊ��
        {
            addr = strstr(&ucRxBuf[2], AT_CR_LF);
            /*2��CR��LF����*/
            if(addr != NULL)
            {
                memcpy(buf, &*(addr + 2), *len);//�������ߵ�����֡  
                return Get_TCP_Success;		//��ȡ���ݳɹ�
            }
            else
            {
                return TCP_Error;		//û���ҵ��س����� ��ȡ���ݴ���
            }
        }
        else
        {
            return TCP_Error;		//���ݳ���Ϊ0 ��ȡ���ݴ���
        }
    }
    
    else 
    {
        return Get_TCP_Fail;		//��ʱ ��ȡ����ʧ��
    }
}

/**********************************************************************
�� �� ��: N720_Set_SMS_Mode
����˵��: ���ö���ģʽ
��    ��: 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Set_SMS_Mode(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CMGF=1");      //���ö���ģʽΪ�ı�ģʽ
  
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
�� �� ��: N720_Set_Char
����˵��: �����ַ���
��    ��: 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Set_Char(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CSCS=\"GSM\"");      //�����ַ���
  
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
�� �� ��: N720_SMSReporting_Mode
����˵��: ���ö��ŵ��������ϱ�ģʽ
��    ��: 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ��¶���ָʾ�����ʽΪ+CMTI: "MT",<index>,�������ݴ�������ֱ����ʾ
***********************************************************************
*/
uint8_t N720_Reporting_Mode(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CNMI=2,1,0,0,0");      //���������ϱ�ģʽ
  
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
�� �� ��: N720_SMS_Delete
����˵��: ɾ�����ж���
��    ��: 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
***********************************************************************
*/
uint8_t N720_SMS_Delete(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT+CMGD=0,4");      //ɾ�����ж���
  
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
�� �� ��: N720_SMS_Send
����˵��: ���Ͷ���
��    ��: *buf ��Ҫ���͵�����  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
*********************************************************************
*/
uint8_t N720_SMS_Send(uint8_t *buf)
{
  uint8_t flag = 0;
  uint8_t cmd_str[32] = {0};
  uint8_t message[128] = {0};
  uint8_t str[1] = {0x1A};
  
  snprintf(cmd_str, 48, "AT+CMGS=\"%s\"", Phone_Number);
  
  N720_SendAT(cmd_str);      //���ý��ն����ֻ���

  if( (N720_WaitResponse(">",1000) == true))
  {
      snprintf(message, 128, "%s%s", buf, str);
      
      N720_SendAT(message);      //���Ͷ�������
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
�� �� ��: N720_SMS_RX
����˵��: ��ȡ�������� ��ŵ�������
��    ��: State ��ǰ���ű�� *buf �������
�� �� ֵ: Get_SMS_Success��ʾ�ɹ� SMS_Error��ʾ����������� Get_SMS_Fail��ʾʧ�� 
ע�ͣ��¶���ָʾ�����ʽΪ+CMTI: "MT",<index>,�������ݴ�������ֱ����ʾ
**********************************************************************************
*/
Enum_Message_JudgeAndGet N720_SMS_RX(uint8_t state, uint8_t *buf, uint8_t *len)
{
    static char ucRxBuf[255] = {0};//{0x11,0x22,0x31,0x39,0x39,0x31,0x30,0x36,0x31,0x35,0x32,0x31,0x37,0x31,0x32,0x31,0x2e,0x34,0x33,0x2e,0x31,0x39,0x36,0x2e,0x31,0x31,0x36,0x3a,0x32,0x31,0x0,0x0,0x0,0x0,0x0,0x66,0x74,0x70,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x37,0x39,0x65,0x34,0x66,0x63,0x38,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x74,0x65,0x73,0x74,0x2e,0x62,0x69,0x6e,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    INT8U   err;    
    static uint8_t cmd_str[16] = {0};   //ATָ��ƴ��

    uint8_t SMS_Info_len = 0;       //�������ݳ���
    
    snprintf(cmd_str, 16, "AT+CMGR=%d", state);    
    N720_SendAT(cmd_str);         //��ȡ����
    
    OSSemPend(SempN720Rx, 5000, &err);
    if( err != OS_ERR_TIMEOUT )
    {        
        SMS_Info_len = comGetBuf(COM_N720, ucRxBuf);
        *len = SMS_Info_len;
        //comClearRxFifo(COM_N720);
        if (SMS_Info_len > 6)
        {            
            memcpy(buf, ucRxBuf, SMS_Info_len);//����SMS������֡
            return Get_SMS_Success;     //��ȡSMS���ݳɹ�                       
        }
        else
        {
            return SMS_Error;   //����SMS����
        }
    }
    else 
    {
        return Get_SMS_Fail;    //�ȴ���Ϣ��ʱ ��ȡSMS����ʧ��
    }
}


/**********************************************************************
�� �� ��: N720_Judge_Type
����˵��: �жϵ�����Ϣ����
��    ��: *Num ��������ǰ���ű�Ų�����Num������
�� �� ֵ: Null_Type��ʾ����Ϣ SMS_Type��ʾ������Ϣ����  TCP_Type��ʾTCP��Ϣ����
ע�ͣ�1���¶���ָʾ�����ʽΪ+CMTI: "MT",<index>,�������ݴ�������ֱ����ʾ
	  2��TCP����Ϣ����ָʾ�����ʽΪ$MYURCREAD: 0
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
            sscanf(ucRxBuf, "%[^,],%d", buf_str, &msg_num);		//��ȡ��ǰ���ű��
            *num = msg_num;
            return SMS_Type;		//��ǰ��Ϣ��SMS
        }
        else if(strstr(ucRxBuf, "$MYURCREAD") != NULL)
        {
            sscanf(ucRxBuf, "%[^ ] %d", buf_str, &msg_num);		//��ȡ��ǰTCP ID��
            *num = msg_num;
            return TCP_Type;		//��ǰ��Ϣ��TCP
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
�� �� ��: SMS_MsgResolve
����˵��: �������Ų���
��    ��: *buf ��������
�� �� ֵ:   SMS_Info_Set ������Ϣ
            SMS_Software_Reset ������
            SMS_Hardware_Reset Ӳ����λ
            SMS_IAP_Ctrl ����IAP
            Ctrl_Error   ���������
ע�ͣ���������Ӧ�����벢�ҽ�����д���ļ�ϵͳ����Flash
***********************************************************************
*/
Enum_SMS_Ctrl SMS_MsgResolve(uint8_t *buf, uint8_t len)
{
    static uint8_t BUF[256] = {0};
    uint8_t buf_str[128] = {0};         //������Ϣ���
    uint8_t message_buf[128] = {0};     //�������ݴ��
    static uint8_t msg_buf[96] = {0};   //������Ϣ���
    
    static Union_BootLoader_PARM BootLoader;       
    
    static uint8_t phone_number[12] = {0};  //�ֻ���
    
    static uint8_t msg_pwd[6] = {0};     //ͨѶ����
    static uint8_t msg_ctrl = 0;         //������
    
    static uint8_t ftp_ip_port[22] = {0};   //FTP��ip�Ͷ˿ں�
    static uint8_t ftp_name[16] = {0};      //FTP���û���
    static uint8_t ftp_pwd[16] = {0};       //FTP������
    static uint8_t ftp_filename[16] = {0};  //FTP����Ҫ���ص��ļ���
    
    static uint8_t dev_id[5] = {0};      //�豸ID��
    static uint8_t channel = 0;          //�ŵ���
    static uint8_t tcp_ip_port[22] = {0};//TCP IP Port
    
    /*�˿ں���ϳ�ʮ������*/
    static uint16_t port;
    static B16_B08 Port;    
    
    memcpy(BUF, buf, len);
    
    /*****************��ȡ�ֻ��źͶ�������**************************/
    sscanf(BUF, "%[^,],\"%[^\"]%[^\n]\n%[^;]", buf_str, phone_number, buf_str, message_buf);    
    
    /*****************�ж��ֻ����Ƿ���"+86"*************************/
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
    
    /*******************��ȡͨѶ����Ϳ���λ***************************/
    sscanf(message_buf, "%[^:]:%[^,],%d,%[^;]", buf_str, msg_pwd, &msg_ctrl, msg_buf);     
    
    
    if(strstr(msg_pwd, MsgPwd) != NULL)     //�ж������Ƿ���ȷ
    {
        if(msg_ctrl == SMS_Info_Set)        //������Ϣ
        {
            /*****************************��ȡ������Ϣ****************************/
            sscanf(msg_buf, "%x,%d,%[^,]",dev_id, &channel, tcp_ip_port);
            
            /**************************д������Ϣ*********************************/
            memcpy(ConfigInfo.DevID, dev_id, sizeof(dev_id));
            ConfigInfo.LoraChannel = channel;
            memcpy(ConfigInfo.TcpIpPort, tcp_ip_port, sizeof(tcp_ip_port));  
            
            /***********************ת��ip��ַ�Ͷ˿ں�****************************/
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
        {   /**********************д������־λ***********************/
            BootLoader.Str_BootLoader_PARM.IAPState[0] = 0x11;  
            BootLoader.Str_BootLoader_PARM.IAPState[1] = 0x22;
            
            /*******************��ȡFTP������������Ϣ*****************/
            sscanf(msg_buf, "%[^,],%[^,],%[^,],%[^,]", ftp_ip_port, ftp_name, ftp_pwd, ftp_filename);   
            
            /*******************дFTP������������Ϣ*******************/
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