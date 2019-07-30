#include "includes.h"




 
 
/*******************************************************************************
** Function name:           Meter_Lib_Init
** Descriptions:            表库初始化   
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void Meter_Lib_Init(void)
{
  for( uint16_t loop = 0;loop<MeterLibMaxNum;loop++)
  {
    memset(&MeterLib[loop].MeterID[0],0,6);//清空ID
    MeterLib[loop].CommNum  = 0 ;//清空
    MeterLib[loop].DatLen   = 0 ;
    MeterLib[loop].TaskLoop = 0 ;
  }
}
 
uint8_t Seek_Lib_ID(uint8_t *id,uint16_t *stn)//根据表号 查找时隙号 插入表库钱需要查找一下有没有重复的表号
{
  for( uint16_t tem = 0;tem < MeterLibMaxNum;tem++)
  {
    if((memcmp(&MeterLib[tem].MeterID[0],&*id,6)==0))//找到对应表号
    {
      *stn = tem;
      return(true);//返回当前的时隙号
    }
  }
  
  return(false);
}





/*******************************************************************************
** Function name:           Insert_Meter_Lib
** Descriptions:            表库中插入一个表号  遍历表库 直到找到一个空的位置  
** input parameters:        void
** output parameters:       void
** Returned value:          表号插入进去 并返回位置号 如果位置号等于最大表库数量 
                            则为错误
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/ 
uint8_t Insert_Meter_Lib(uint8_t *id,uint16_t *stn)
{
  uint16_t loop = 0;
  uint16_t num  = 0;
  uint16_t tem  = 0;
  
  
  for(loop=0;loop<16;loop++)
  {
    for(num=0;num<29;num++)
    {
      tem = loop + num * 16;
      
      if(MeterLib[tem].CommNum == 0)
      {
        MeterLib[tem].CommNum  = 3 ;
        MeterLib[tem].DatLen   = 0 ;
        MeterLib[tem].TaskLoop = 0;
        memcpy(&MeterLib[tem].MeterID[0],&*id,6);//处理内存
        *stn = tem ;
        return(true);
      }
    }
  }
  
  return(false);
}

/*******************************************************************************
** Function name:           Insert_Meter_Lib_DownData
** Descriptions:            根据表号 把645数据插入  测试用645数据帧 68 17 00 10 52 00 00 68 01 02 34 23 A3 16
** input parameters:        id ：表号 buf ：前置机下发的数据帧 len：数据帧长度
** output parameters:       是否插入成功
** Returned value:          true 插入成功 false 未插入成功 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Insert_Meter_Lib_Data(uint8_t *id,uint8_t *buf,uint8_t len)
{
 
  for(uint16_t num=0;num<MeterLibMaxNum;num++)
  {
    if((memcmp(&MeterLib[num].MeterID[0],&*id,6)==0))//找到对应表号
    {
      memcpy(&MeterLib[num].DatBuf[0],&*buf,len);
      MeterLib[num].DatLen = len;
      return(true);
    }
  }
  
  return(false);
}
 
/*******************************************************************************
** Function name:           Maintenance_MeterLib
** Descriptions:            表库系统维护  遇到抄读次数为0  则清除  每天晚上复位前调用
** input parameters:        void
** output parameters:       void
** Returned value:          表号插入进去 并返回位置号 如果位置号等于最大表库数量 
                            则为错误
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void Maintenance_MeterLib(void)
{
  for(uint16_t num=0;num<MeterLibMaxNum;num++)
  {
    if(MeterLib[num].CommNum == 0)
    {
      MeterLib[num].CommNum  = 0 ;//通信次数 
      MeterLib[num].DatLen   = 0 ;//
      MeterLib[num].TaskLoop = 0 ;
    }
  }
}


/*******************************************************************************
** Function name:           Count_Meter_Lib_Num
** Descriptions:            统计表库中表的数量
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
uint16_t Count_Meter_Lib_Num(void)
{
  uint16_t num=0;
  
  for(uint16_t num=0;num<MeterLibMaxNum;num++)
  {
    if(MeterLib[num].CommNum > 0)
    {
      num++;//清零
    }
  }

  return(num);
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_DownData
** Descriptions:            根据时隙编号 读取对应的下发数据帧  但是并不删除数据
** input parameters:        stn ： 时隙号 buf ：时隙号对应的数据帧  
** output parameters:       数据帧长度 
** Returned value:           如返回0 则表示没有取到数据
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Get_Meter_Lib_Data(uint16_t stn,uint8_t *buf)
{ 
  uint8_t len = 0;
  
  if(MeterLib[stn].DatLen > 0)
  {
    len = MeterLib[stn].DatLen;//获得数据帧长度 
    memcpy(&*buf,&MeterLib[stn].DatBuf[0],len); //拷贝到buf中
  }
  
  return(len);
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_DownData
** Descriptions:            根据时隙编号 读取对应的下发数据帧  但是并不删除数据
** input parameters:        stn ： 时隙号 id ：时隙号对应表ID  num ：需要拷贝的数量长度 
** output parameters:       数据帧长度 
** Returned value:           如返回0 则表示没有取到数据
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Get_Meter_Lib_ID(uint16_t stn,uint8_t *id,uint8_t num)
{ 
 
  if(MeterLib[stn].CommNum == 0)//此处有节点 
  {
    return(false);
  }
  else
  {
    memcpy(&*id,&MeterLib[stn].MeterID[0],num); //拷贝到buf中
    return(true);
  }
}


uint8_t Empty_Meter_Lib_ID(uint16_t stn)
{ 
  if( stn < MeterLibMaxNum )
  {
    memset(&MeterLib[stn].MeterID[0],0,6);//清空ID
    MeterLib[stn].CommNum  = 0 ;//清空
    MeterLib[stn].DatLen   = 0 ;
    MeterLib[stn].TaskLoop = 0 ;
    return(true );
  }
  else
  {
    return(false);
  }
}

/*******************************************************************************
** Function name:           Empty_Meter_Lib_Data
** Descriptions:            根据时隙编号 清空对应的下发数据帧  
** input parameters:        stn ： 时隙号 
** output parameters:       数据帧长度 
** Returned value:           如返回0 则表示没有取到数据
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Empty_Meter_Lib_Data(uint16_t stn)
{ 
  if( stn < MeterLibMaxNum )
  {
    MeterLib[stn].DatLen = 0;//获得数据帧长度 
    return(true );
  }
  else
  {
    return(false);
  }
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_DownData_State
** Descriptions:            根据时隙号 查找是否有下发数据
** input parameters:        stn ： 时隙号  
** output parameters:        
** Returned value:           如返回0 则表示没有取到数据
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Get_Meter_Lib_Data_State(uint16_t stn)
{ 
  if(MeterLib[stn].DatLen == 0)
  {
    return(false) ;
  }
  else
  {
    return(true) ;
  }
}


