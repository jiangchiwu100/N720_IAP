#include "include.h"

/*
  EEPROM ��ʼ���ݾ�Ϊ0xFF  
  ʵ�� ����д128�ֽ� ��Լ��Ҫ20----30ms
  Memory array:
�C 16384 bit (2 Kbytes) of EEPROM
�C Page size: 16 bytes
*/
void EEPROM_Init(void)
{
  uint8_t buf[Eeprom_Max_Page_Size]={0};

#if   EEPROM_M24C16 
  for(uint8_t num = 0;num <(Eeprom_Max_Rom_Size / Eeprom_Max_Page_Size);num++)
  {
    M24C02_Page_Write(num*Eeprom_Max_Page_Size,&buf[0],Eeprom_Max_Page_Size);
  }
#elif EEPROM_M24C02
  for(uint8_t num = 0;num <(Eeprom_Max_Rom_Size / Eeprom_Max_Page_Size);num++)
  {
    M24C02_Page_Write(num*Eeprom_Max_Page_Size,&buf[0],Eeprom_Max_Page_Size);
  }
        
#elif EEPROM_M24M01
 
#endif
}
bool EEPROM_Array_Write(uint16_t addr,uint8_t *buff,uint16_t lengh)
{
  bool flag = false;
  uint16_t offset = 0;//buff ��ƫ����
  uint16_t tem = 0;//���ݵ�ʣ�೤��
  uint16_t LoopMaxNum = 0;
  
  LoopMaxNum = ( addr + lengh ) / Eeprom_Max_Page_Size;//ĩ��ַ��0��ַ���˼���ҳ
  LoopMaxNum = LoopMaxNum - addr/ Eeprom_Max_Page_Size;//�׵�ַ��0��ַ���˼��Σ�����õ���ҳ����
  
  for(uint16_t loop=0;loop <= (LoopMaxNum) ;loop++)
  {
    if(addr < Eeprom_Max_Page_Size)//��ҳ
    {
      if( lengh <= (Eeprom_Max_Page_Size-addr))//�����ڵ�һҳ�Ϳ�����ɴ洢
      {
        //�洢��һҳ
#if   EEPROM_M24C16 
        M24C16_Page_Write(addr,&*buff,lengh);
#elif EEPROM_M24C02
        M24C02_Page_Write(addr,&*buff,lengh);
#elif EEPROM_M24M01
 
#endif
        flag = true;//��ɴ洢
        break;
      }
      else
      {
        tem = (Eeprom_Max_Page_Size - addr);
        //�洢��һҳ ��ַΪ addr ����Ϊtem
#if  EEPROM_M24C16 
        M24C16_Page_Write(addr,&*buff,tem);
#elif EEPROM_M24C02
        M24C02_Page_Write(addr,&*buff,tem);
#elif EEPROM_M24M01
 
#endif
        offset = offset + tem;
        addr = addr + tem;//���µ�ַ
        lengh= lengh - tem;//���³���
      }
    }
    else                          // ����ҳ
    {
      tem = Eeprom_Max_Page_Size - (addr % Eeprom_Max_Page_Size);//��ҳʣ��ռ�
      
      if( lengh <= tem)//��ҳ����д��
      {
        //�洢���һҳ ��ַΪ addr ����Ϊlengh
#if   EEPROM_M24C16 
        M24C16_Page_Write(addr,&*(buff+offset),lengh);
#elif EEPROM_M24C02
        M24C02_Page_Write(addr,&*(buff+offset),lengh);
#elif EEPROM_M24M01
      
#endif
        flag = true;//��ɴ洢
        break;
      }
      else            //����Ҫ��ҳ
      {
        //�洢����һҳ  ��ַΪ addr ���� tem
#if  EEPROM_M24C16 
        M24C16_Page_Write(addr,&*(buff+offset),tem);
#elif EEPROM_M24C02
        M24C16_Page_Write(addr,&*(buff+offset),tem);
#elif EEPROM_M24M01
 
#endif
        offset = offset + tem;
        addr = addr + tem;
        lengh= lengh - tem;
      }
    }
  }
  return(flag);
}



bool EEPROM_Array_Read(uint16_t addr,uint8_t *buff,uint16_t lengh)
{

#if   EEPROM_M24C02 
        return(M24C02_Array_Read(addr,&*buff,lengh));
#elif EEPROM_M24C16 
        return(M24C16_Array_Read(addr,&*buff,lengh));
#elif EEPROM_M24M01
 
#endif
 
}


/*******************************************************************************
** Function name:           M24C02_Page_Write  256�ֽ� 
** Descriptions:            M24C02д����ֽ�����  ��ַ��֪
** input parameters:        addr  M24C02 �ڲ���ַ
*buff Ҫд�����������
lengh ����
** output parameters:       true  �ɹ�
false ʧ��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C02_Page_Write(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  Eeprom_Vcc_En();
  Eeprom_Wp_Ds();//���д����
  
  IIC_Start()   ;
  
  if(addr >= 0xff)//02����256�ֽ�  0-255��ַ  
  {
    return(false);
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*����������ַ*/
  }
  if(ack==false)return(false);
  

  if(addr >= 0xff)//����������ַ��Χ
  {
    return(false);            /*���������ӵ�ַ*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*���������ӵ�ַ*/
  }
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh;i++)
  {   
    ack = IIC_SendByte(*buff);                 /*��������*/
    if(ack==false)return(false);
    buff++;
  } 
  IIC_Stop();                           /*��������*/ 
  
  Delay_ms(7);	                        //Ҫ��EEPROM������д���ʱ�� 
  
  Eeprom_Wp_En();  
  Eeprom_Vcc_Ds();  //��д����  	
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24C02_Array_Read
** Descriptions:            M24C02�������ֽ�����  ��ַ��֪
** input parameters:        addr  M24C02 �ڲ���ַ
*buff Ҫд�����������
lengh ����
** output parameters:       true  �ɹ�
false ʧ��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C02_Array_Read(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  
  Eeprom_Vcc_En();

  IIC_Start();          /*��������*/
  if(addr >= 0xff)
  {
    return(false);      /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*����������ַ*/
  }
  if(ack==false)return(false);
  
  if(addr >= 0xff)
  {
    return(false);             /*���������ӵ�ַ*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*���������ӵ�ַ*/
  }
  if(ack==false)return(false);


  IIC_Start();          /*��������*/
  if(addr >= 0xff)
  {
    return(false);       /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(0xA1);      /*����������ַ*/
  }
  if(ack==false)return(false);
  

  for(uint16_t i=0;i<lengh-1;i++)
  {   
    *buff = IIC_RcvByte();       /*��������*/
    IIC_Ack(false);         /*���;ʹ�λ*/  
    buff++;
  } 
  *buff = IIC_RcvByte();
  
  IIC_Ack(true);        /*���ͷ�Ӧλ*/
  
  IIC_Stop();           /*��������*/ 
  
//  Delay_ms(10);
  
  Eeprom_Vcc_Ds();
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24C16_Page_Write
** Descriptions:            M24C16д����ֽ�����  ��ַ��֪
** input parameters:        addr  M24C16�ڲ���ַ
*buff Ҫд�����������
lengh ����
** output parameters:       true  �ɹ�
false ʧ��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C16_Page_Write(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  uint8_t tem;
  
  __disable_interrupt();
  Eeprom_Vcc_En();
  Eeprom_Wp_Ds();//���д����
  
  IIC_Start()   ;
  
  if(addr > 0xff)
  {
    tem = addr >> 8;
    tem = tem  << 1;
    tem = 0xA0 | tem;
    ack = IIC_SendByte(tem);      /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*����������ַ*/
  }
  if(ack==false)return(false);
  

  if(addr > 0xff)
  {
    tem = addr & 0xff;
    ack = IIC_SendByte(tem);             /*���������ӵ�ַ*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*���������ӵ�ַ*/
  }
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh;i++)
  {   
    ack = IIC_SendByte(*buff);                 /*��������*/
    if(ack==false)return(false);
    buff++;
  } 
  IIC_Stop();                           /*��������*/ 
  
  Delay_ms(7);	                        //Ҫ��EEPROM������д���ʱ�� 
  
  Eeprom_Wp_En();  
  Eeprom_Vcc_Ds();  //��д����  	
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24M01_Array_Read
** Descriptions:            M24M01�������ֽ�����  ��ַ��֪
** input parameters:        addr  M24M01�ڲ���ַ
*buff Ҫд�����������
lengh ����
** output parameters:       true  �ɹ�
false ʧ��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C16_Array_Read(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  uint8_t tem=0;
  
  __disable_interrupt();
  
  Eeprom_Vcc_En();

  IIC_Start();          /*��������*/
  if(addr > 0xff)
  {
    tem = addr >> 8;
    tem = tem  << 1;
    tem = 0xA0 | tem;
    ack = IIC_SendByte(tem);      /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*����������ַ*/
  }
  if(ack==false)return(false);
  
  if(addr > 0xff)
  {
    tem = addr & 0xff;
    ack = IIC_SendByte(tem);             /*���������ӵ�ַ*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*���������ӵ�ַ*/
  }
  if(ack==false)return(false);


  IIC_Start();          /*��������*/
  if(addr > 0xff)
  {
    tem = addr >> 8;
    tem = tem  << 1;
    tem = 0xA0 | tem;
    ack = IIC_SendByte(tem | 0x01);      /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(0xA0 | 0x01);      /*����������ַ*/
  }
  if(ack==false)return(false);
  

  for(uint16_t i=0;i<lengh-1;i++)
  {   
    *buff = IIC_RcvByte();       /*��������*/
    IIC_Ack(false);         /*���;ʹ�λ*/  
    buff++;
  } 
  *buff = IIC_RcvByte();
  
  IIC_Ack(true);        /*���ͷ�Ӧλ*/
  
  IIC_Stop();           /*��������*/ 
  
  Delay_ms(5);
  
  Eeprom_Vcc_Ds();
  __enable_interrupt();
  return(true);
}


/*******************************************************************************
** Function name:           M24M01_Array_Write
** Descriptions:            M24M01д����ֽ�����  ��ַ��֪
** input parameters:        addr  M24M01�ڲ���ַ
*buff Ҫд�����������
lengh ����
** output parameters:       true  �ɹ�
false ʧ��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24M01_Page_Write(uint32_t addr,uint8_t *buff,uint16_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  Eeprom_Vcc_En();
  Eeprom_Wp_Ds();//���д����
  Delay_us(500)   ;//��ʱ���ȶ�IO״̬
  
  IIC_Start()   ;
  
  if(addr>=0xffff)
  {
    ack = IIC_SendByte(DeviceAddrHigh);     /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(DeviceAddrLow);      /*����������ַ*/
  }
  if(ack==false)
    return(false);
  
  ack = IIC_SendByte(addr/256);             /*���������ӵ�ַ*/
  if(ack==false)return(false);
  ack = IIC_SendByte(addr%256);             /*���������ӵ�ַ*/
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh;i++)
  {   
    ack = IIC_SendByte(*buff);                 /*��������*/
    if(ack==false)return(false);
    buff++;
  } 
  IIC_Stop();                           /*��������*/ 
  
  Delay_ms(5);	                        //Ҫ��EEPROM������д���ʱ�� 
  
  Eeprom_Wp_En();  
  Eeprom_Vcc_Ds();  //��д����  	
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24M01_Array_Read
** Descriptions:            M24M01�������ֽ�����  ��ַ��֪
** input parameters:        addr  M24M01�ڲ���ַ
*buff Ҫд�����������
lengh ����
** output parameters:       true  �ɹ�
false ʧ��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24M01_Array_Read(uint32_t addr,uint8_t *buff,uint16_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  
  Eeprom_Vcc_En();
  Delay_us(500) ;

  IIC_Start();          /*��������*/
  if(addr>=0xffff)
  {
    ack = IIC_SendByte(DeviceAddrHigh);     /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(DeviceAddrLow);      /*����������ַ*/
  }
  if(ack==false)return(false);
  
  ack = IIC_SendByte(addr/256);             /*���������ӵ�ַ*/
  if(ack==false)return(false);
  ack = IIC_SendByte(addr%256);             /*���������ӵ�ַ*/
  if(ack==false)return(false);
  
  IIC_Start();	
  if(addr>=0xffff)
  {
    ack = IIC_SendByte(DeviceAddrHigh|0x01);     /*����������ַ*/
  }
  else
  {
    ack = IIC_SendByte(DeviceAddrLow|0x01);      /*����������ַ*/
  }
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh-1;i++)
  {   
    *buff = IIC_RcvByte();       /*��������*/
    IIC_Ack(false);         /*���;ʹ�λ*/  
    buff++;
  } 
  *buff = IIC_RcvByte();
  
  IIC_Ack(true);        /*���ͷ�Ӧλ*/
  
  IIC_Stop();           /*��������*/ 
  
  Delay_ms(5);
  
  Eeprom_Vcc_Ds();
  __enable_interrupt();
  return(true);
}























