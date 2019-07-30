#include "include.h"

/*
  EEPROM 初始数据均为0xFF  
  实测 读、写128字节 大约需要20----30ms
  Memory array:
– 16384 bit (2 Kbytes) of EEPROM
– Page size: 16 bytes
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
  uint16_t offset = 0;//buff 的偏移量
  uint16_t tem = 0;//数据的剩余长度
  uint16_t LoopMaxNum = 0;
  
  LoopMaxNum = ( addr + lengh ) / Eeprom_Max_Page_Size;//末地址到0地址跨了几次页
  LoopMaxNum = LoopMaxNum - addr/ Eeprom_Max_Page_Size;//首地址到0地址跨了几次，相减得到跨页次数
  
  for(uint16_t loop=0;loop <= (LoopMaxNum) ;loop++)
  {
    if(addr < Eeprom_Max_Page_Size)//首页
    {
      if( lengh <= (Eeprom_Max_Page_Size-addr))//数据在第一页就可以完成存储
      {
        //存储第一页
#if   EEPROM_M24C16 
        M24C16_Page_Write(addr,&*buff,lengh);
#elif EEPROM_M24C02
        M24C02_Page_Write(addr,&*buff,lengh);
#elif EEPROM_M24M01
 
#endif
        flag = true;//完成存储
        break;
      }
      else
      {
        tem = (Eeprom_Max_Page_Size - addr);
        //存储第一页 地址为 addr 长度为tem
#if  EEPROM_M24C16 
        M24C16_Page_Write(addr,&*buff,tem);
#elif EEPROM_M24C02
        M24C02_Page_Write(addr,&*buff,tem);
#elif EEPROM_M24M01
 
#endif
        offset = offset + tem;
        addr = addr + tem;//更新地址
        lengh= lengh - tem;//更新长度
      }
    }
    else                          // 非首页
    {
      tem = Eeprom_Max_Page_Size - (addr % Eeprom_Max_Page_Size);//本页剩余空间
      
      if( lengh <= tem)//本页即可写完
      {
        //存储最后一页 地址为 addr 长度为lengh
#if   EEPROM_M24C16 
        M24C16_Page_Write(addr,&*(buff+offset),lengh);
#elif EEPROM_M24C02
        M24C02_Page_Write(addr,&*(buff+offset),lengh);
#elif EEPROM_M24M01
      
#endif
        flag = true;//完成存储
        break;
      }
      else            //还需要跨页
      {
        //存储完整一页  地址为 addr 长度 tem
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
** Function name:           M24C02_Page_Write  256字节 
** Descriptions:            M24C02写入多字节数据  地址已知
** input parameters:        addr  M24C02 内部地址
*buff 要写入的数据数组
lengh 长度
** output parameters:       true  成功
false 失败
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C02_Page_Write(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  Eeprom_Vcc_En();
  Eeprom_Wp_Ds();//解除写保护
  
  IIC_Start()   ;
  
  if(addr >= 0xff)//02共有256字节  0-255地址  
  {
    return(false);
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  

  if(addr >= 0xff)//超出正常地址范围
  {
    return(false);            /*发送器件子地址*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*发送器件子地址*/
  }
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh;i++)
  {   
    ack = IIC_SendByte(*buff);                 /*发送数据*/
    if(ack==false)return(false);
    buff++;
  } 
  IIC_Stop();                           /*结束总线*/ 
  
  Delay_ms(7);	                        //要给EEPROM留出来写入的时间 
  
  Eeprom_Wp_En();  
  Eeprom_Vcc_Ds();  //打开写保护  	
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24C02_Array_Read
** Descriptions:            M24C02读出多字节数据  地址已知
** input parameters:        addr  M24C02 内部地址
*buff 要写入的数据数组
lengh 长度
** output parameters:       true  成功
false 失败
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C02_Array_Read(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  
  Eeprom_Vcc_En();

  IIC_Start();          /*启动总线*/
  if(addr >= 0xff)
  {
    return(false);      /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  
  if(addr >= 0xff)
  {
    return(false);             /*发送器件子地址*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*发送器件子地址*/
  }
  if(ack==false)return(false);


  IIC_Start();          /*启动总线*/
  if(addr >= 0xff)
  {
    return(false);       /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(0xA1);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  

  for(uint16_t i=0;i<lengh-1;i++)
  {   
    *buff = IIC_RcvByte();       /*发送数据*/
    IIC_Ack(false);         /*发送就答位*/  
    buff++;
  } 
  *buff = IIC_RcvByte();
  
  IIC_Ack(true);        /*发送非应位*/
  
  IIC_Stop();           /*结束总线*/ 
  
//  Delay_ms(10);
  
  Eeprom_Vcc_Ds();
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24C16_Page_Write
** Descriptions:            M24C16写入多字节数据  地址已知
** input parameters:        addr  M24C16内部地址
*buff 要写入的数据数组
lengh 长度
** output parameters:       true  成功
false 失败
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C16_Page_Write(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  uint8_t tem;
  
  __disable_interrupt();
  Eeprom_Vcc_En();
  Eeprom_Wp_Ds();//解除写保护
  
  IIC_Start()   ;
  
  if(addr > 0xff)
  {
    tem = addr >> 8;
    tem = tem  << 1;
    tem = 0xA0 | tem;
    ack = IIC_SendByte(tem);      /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  

  if(addr > 0xff)
  {
    tem = addr & 0xff;
    ack = IIC_SendByte(tem);             /*发送器件子地址*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*发送器件子地址*/
  }
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh;i++)
  {   
    ack = IIC_SendByte(*buff);                 /*发送数据*/
    if(ack==false)return(false);
    buff++;
  } 
  IIC_Stop();                           /*结束总线*/ 
  
  Delay_ms(7);	                        //要给EEPROM留出来写入的时间 
  
  Eeprom_Wp_En();  
  Eeprom_Vcc_Ds();  //打开写保护  	
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24M01_Array_Read
** Descriptions:            M24M01读出多字节数据  地址已知
** input parameters:        addr  M24M01内部地址
*buff 要写入的数据数组
lengh 长度
** output parameters:       true  成功
false 失败
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24C16_Array_Read(uint16_t addr,uint8_t *buff,uint8_t lengh)
{
  bool ack;
  uint8_t tem=0;
  
  __disable_interrupt();
  
  Eeprom_Vcc_En();

  IIC_Start();          /*启动总线*/
  if(addr > 0xff)
  {
    tem = addr >> 8;
    tem = tem  << 1;
    tem = 0xA0 | tem;
    ack = IIC_SendByte(tem);      /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(0xA0);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  
  if(addr > 0xff)
  {
    tem = addr & 0xff;
    ack = IIC_SendByte(tem);             /*发送器件子地址*/
  }
  else
  {
    ack = IIC_SendByte(addr);             /*发送器件子地址*/
  }
  if(ack==false)return(false);


  IIC_Start();          /*启动总线*/
  if(addr > 0xff)
  {
    tem = addr >> 8;
    tem = tem  << 1;
    tem = 0xA0 | tem;
    ack = IIC_SendByte(tem | 0x01);      /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(0xA0 | 0x01);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  

  for(uint16_t i=0;i<lengh-1;i++)
  {   
    *buff = IIC_RcvByte();       /*发送数据*/
    IIC_Ack(false);         /*发送就答位*/  
    buff++;
  } 
  *buff = IIC_RcvByte();
  
  IIC_Ack(true);        /*发送非应位*/
  
  IIC_Stop();           /*结束总线*/ 
  
  Delay_ms(5);
  
  Eeprom_Vcc_Ds();
  __enable_interrupt();
  return(true);
}


/*******************************************************************************
** Function name:           M24M01_Array_Write
** Descriptions:            M24M01写入多字节数据  地址已知
** input parameters:        addr  M24M01内部地址
*buff 要写入的数据数组
lengh 长度
** output parameters:       true  成功
false 失败
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24M01_Page_Write(uint32_t addr,uint8_t *buff,uint16_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  Eeprom_Vcc_En();
  Eeprom_Wp_Ds();//解除写保护
  Delay_us(500)   ;//延时，稳定IO状态
  
  IIC_Start()   ;
  
  if(addr>=0xffff)
  {
    ack = IIC_SendByte(DeviceAddrHigh);     /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(DeviceAddrLow);      /*发送器件地址*/
  }
  if(ack==false)
    return(false);
  
  ack = IIC_SendByte(addr/256);             /*发送器件子地址*/
  if(ack==false)return(false);
  ack = IIC_SendByte(addr%256);             /*发送器件子地址*/
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh;i++)
  {   
    ack = IIC_SendByte(*buff);                 /*发送数据*/
    if(ack==false)return(false);
    buff++;
  } 
  IIC_Stop();                           /*结束总线*/ 
  
  Delay_ms(5);	                        //要给EEPROM留出来写入的时间 
  
  Eeprom_Wp_En();  
  Eeprom_Vcc_Ds();  //打开写保护  	
  __enable_interrupt();
  return(true);
}

/*******************************************************************************
** Function name:           M24M01_Array_Read
** Descriptions:            M24M01读出多字节数据  地址已知
** input parameters:        addr  M24M01内部地址
*buff 要写入的数据数组
lengh 长度
** output parameters:       true  成功
false 失败
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-12     
*******************************************************************************/
bool M24M01_Array_Read(uint32_t addr,uint8_t *buff,uint16_t lengh)
{
  bool ack;
  
  __disable_interrupt();
  
  Eeprom_Vcc_En();
  Delay_us(500) ;

  IIC_Start();          /*启动总线*/
  if(addr>=0xffff)
  {
    ack = IIC_SendByte(DeviceAddrHigh);     /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(DeviceAddrLow);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  
  ack = IIC_SendByte(addr/256);             /*发送器件子地址*/
  if(ack==false)return(false);
  ack = IIC_SendByte(addr%256);             /*发送器件子地址*/
  if(ack==false)return(false);
  
  IIC_Start();	
  if(addr>=0xffff)
  {
    ack = IIC_SendByte(DeviceAddrHigh|0x01);     /*发送器件地址*/
  }
  else
  {
    ack = IIC_SendByte(DeviceAddrLow|0x01);      /*发送器件地址*/
  }
  if(ack==false)return(false);
  
  for(uint16_t i=0;i<lengh-1;i++)
  {   
    *buff = IIC_RcvByte();       /*发送数据*/
    IIC_Ack(false);         /*发送就答位*/  
    buff++;
  } 
  *buff = IIC_RcvByte();
  
  IIC_Ack(true);        /*发送非应位*/
  
  IIC_Stop();           /*结束总线*/ 
  
  Delay_ms(5);
  
  Eeprom_Vcc_Ds();
  __enable_interrupt();
  return(true);
}























