
#include "Math_Lib.h"

/*
SysCtlDelay(Fire_Num*(TheSysClock/3000000));
SysCtlDelay(1000000*(TheSysClock/3000000));  1s
*/
void SysCtlDelay(unsigned long ulCount)
{
  __asm("    subs    r0, #1\n"
        "    bne.n   SysCtlDelay\n"
          "    bx      lr");
}

void Delay_us(uint32_t dat)
{
  SysCtlDelay(dat*(72000000/3000000));  
}

void Delay_ms(uint16_t dat)
{
  uint16_t i;
  
  for(i=0;i<dat;i++)
  {
    Delay_us(1000);
  }
}
/*******************************************************************************
** Function name:           Glide_Average_Filter
** Descriptions:            递推平均滤波
** input parameters:        
** output parameters:       输出
** Returned value:          none                                                
** Created by:              程卫玺                                              
** Created Date:            2014/3/3                                            
**------------------------------------------------------------------------------
** Modified by:
** Modified date:
*******************************************************************************/
uint32_t Glide_Average_Filter(uint32_t *point,uint32_t data,uint32_t num,uint32_t *sum_point)
{ 
  *sum_point += data;
  *sum_point -= *(point + num);
  *(point + num) = data;
  
  return(*sum_point);
}

//函 数 名：AscToHex()
//功能描述：把ASCII转换为16进制
char Hex_To_Ascii(uint8_t dat)
{
   switch(dat)
  {
  case  0:
    {dat = '0';}
    break;
  case 0x01:
    {dat = '1';}
    break;
  case 0x02:
    {dat = '2';}
    break;
  case 0x03:
    {dat = '3';}
    break;
  case 0x04:
    {dat = '4';}
    break;
  case 0x05:
    {dat = '5';}
    break;
  case 0x06:
    {dat = '6';}
    break;
  case 0x07:
    {dat = '7';}
    break;
  case 0x08:
    {dat = '8';}
    break;
  case 0x09:
    {dat = '9';}
    break;
  case 0x0A:
    {dat = 'A';}
    break;
  case 0x0B:
    {dat = 'B';}
    break;
  case 0x0C:
    {dat = 'C';}
    break;
  case 0x0D:
    {dat = 'D';}
    break;
  case 0x0E:
    {dat = 'E';}
    break;
  case 0x0F:
    {dat = 'F';}
    break;
  default:
    dat = 0xff;
    break;
  }
  return dat;
}

//函 数 名：HexToAsc()
//功能描述：把16进制转换为ASCII
uint8_t Ascii_To_Hex(char uchar)
{
  uint8_t dat = 0;
  
  switch(uchar)
  {
  case '0':
    {dat = 0x00;}
    break;
  case '1':
    {dat = 0x01;}
    break;
  case '2':
    {dat = 0x02;}
    break;
  case '3':
    {dat = 0x03;}
    break;
  case '4':
    {dat = 0x04;}
    break;
  case '5':
    {dat = 0x05;}
    break;
  case '6':
    {dat = 0x06;}
    break;
  case '7':
    {dat = 0x07;}
    break;
  case '8':
    {dat = 0x08;}
    break;
  case '9':
    {dat = 0x09;}
    break;
  case 'a':
  case 'A':
    {dat = 0x0A;}
    break;
  case 'b':
  case 'B':
    {dat = 0x0B;}
    break;
  case 'c':
  case 'C':
    {dat = 0x0C;}
    break;
  case 'd':
  case 'D':
    {dat = 0x0D;}
    break;
  case 'e':
  case 'E':
    {dat = 0x0E;}
    break;
  case 'f':
  case 'F':
    {dat = 0x0F;}
    break;
  default:
    dat = 0xff;
    break;
  }
  return(dat);
  
}

void Random_Seed(void)
{
//  uint32_t ChipUniqueID[3]={0};
//  uint16_t Seed=0;
//  
//  ChipUniqueID[0] = *(unsigned int*)(0x00100E7);
//  ChipUniqueID[1] = *(unsigned int*)(0x00100E8);
//  ChipUniqueID[2] = *(unsigned int*)(0x00100E9);
//  
//  Seed = (uint16_t)((ChipUniqueID[0]>>1)+(ChipUniqueID[1]>>2)+(ChipUniqueID[2]>>3));
//  srand(Seed);
}

uint8_t Count_CheckSum(uint8_t *dat,uint8_t len)
{
  uint8_t num,comsum=0;
  
  for(num=0;num<len;num++)
  {
    comsum+=dat[num];
  }
  
  return(comsum);
}


/*********************************************************************
** Function name:      Decimal_2_Hexadecimal
** Descriptions:       十进制转为十六进制
** input parameters:   dec_data 十进制数据
** output parameters:  返回对应的十六进制  例如 12345678 转为 0x12345678
** Returned value:     none
** Created by:         chengweixi
** Created Date:       2013-07-01
**-------------------------------------------------------------------
** Modified by:
** Modified date:
*********************************************************************/
uint32_t Bin_To_Bcd_Bit32(uint32_t dec_data)
{
  unsigned long int value=0,data=0;
  for(unsigned char i=0;i<8;i++)
  {
    value = dec_data %10;
    dec_data /=10;
    value = value<< i*4 ;
    data  = data + value;
  }
  return(data);
}

/******************************************************************
** Function name:      Hexadecimal_2_Decimal
** Descriptions:       十六进制转为十进制
** input parameters:   dec_data 十进制数据
** output parameters:  返回对应的十进制  例如 0x12345678 转为 12345678
** Returned value:     none
** Created by:         chengweixi
** Created Date:       2013-07-01
**----------------------------------------------------------------
** Modified by:
** Modified date:
***************************************************************/
uint32_t Bcd_To_Bin_Bit32(uint32_t hex_data)
{
  unsigned long int value=0,data=0,temp=1;
  unsigned char i,y;
  for(i=0;i<8;i++)
  {
    value = hex_data % 0x10;
    hex_data /=0x10;
    for(y=0;y<i;y++)
    {
      temp *=10;
    }
    data  = data + value*temp; 
    temp=1;
  }
  return(data);
}

//23    转 0x23 
uint8_t Bin_To_Bcd_Bit8(uint8_t value)
{
  return ( ((value/10)<<4)+(value%10) );
}
//0x23  转  23 
uint8_t Bcd_To_Bin_Bit8(uint8_t value)
{
  return ( ((value&0xF0)>>4)*10+(value&0x0F) );
}

//if(_memcmp(&ID_Lib[IDLibLoop*6] , 0 ,6) == true)//判断表号是否为0
bool _memcmp( void *dest, const uint8_t src, uint8_t count)
{
 char *tmp = dest;
 bool flag=true;
 
 while (count--)
 {  
   if(*tmp++ != src)
    flag = false;
 }
 return flag;
}