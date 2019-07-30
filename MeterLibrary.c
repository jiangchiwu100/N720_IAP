
#include "includes.h"
//0-3   id 
//4-4   入网标识  0 未入网  0xff 已经入网
//5-5   长度     
//6-127 数据帧
static uint8_t MeterLib[MeterLibraryMaxNum][128] ; //128个每个表6字节表号
static uint8_t MeterLibDataNum = 0;
static uint8_t MeterLibEquipmentNum=0;

/*******************************************************************************
** Function name:           Meter_Lib_Init
** Descriptions:            表库初始化  需要40ms完成
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void Meter_Lib_Init(void)
{
  memset(&MeterLib[0][0] ,0x00,(MeterLibraryMaxNum*128));//清零
  MeterLibDataNum=0;
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
uint8_t Insert_Meter_Lib(uint8_t *id)
{
  uint8_t num = 0;
  uint8_t  tem[4]={0};
  
  for(num=0;num<MeterLibraryMaxNum;num++)
  {
    if((memcmp(&tem[0],&MeterLib[num][0],4)==0))//找一个空位置 
    {
      memset(&MeterLib[num][0],0x00,128);//清零
      memcpy(&MeterLib[num][0],&*id,4);//处理内存
      MeterLibEquipmentNum++;
      break;
    }
  }

  return(num);
}

/*******************************************************************************
** Function name:           Reset_Meter_Lib
** Descriptions:            重置表库中某一个位置  清除掉   
** input parameters:        void
** output parameters:       void
** Returned value:          表号插入进去 并返回位置号
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void Reset_Meter_Lib(uint8_t num)
{
  memset(&MeterLib[num][0],0x00,128);//对应表库清零
  if(MeterLibEquipmentNum>0)
  {
    MeterLibEquipmentNum--;
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
uint8_t Count_Meter_Lib_Num(void)
{
  return(MeterLibEquipmentNum);
}

/*
测试用645数据帧 68 17 00 10 52 00 00 68 01 02 34 23 A3 16

插入数据项
*/
void Insert_Meter_Lib_DataItem(uint8_t *buf,uint8_t len)
{
  uint8_t recvbuf[128]={0};
  
  memcpy(&recvbuf[0],&*buf,len);
  
  for(uint8_t num=0;num<MeterLibraryMaxNum;num++)
  {
    if((memcmp(&recvbuf[1],&MeterLib[num][0],4)==0))//找到对应表号
    {
      MeterLibDataNum++;
      MeterLib[num][5] = len;
      memset(&MeterLib[num][6],0x00,122);        //清零
      memcpy(&MeterLib[num][6],&recvbuf[0],len); //存入内存
      break;
    }
  }
}


uint8_t Get_Meter_Lib_DataItem(uint8_t *buf,uint8_t node)
{ 
  uint8_t len=0;
  
  len = MeterLib[node][5];
  if(len>0)
  {
    memcpy(&*buf,&MeterLib[node][6],len); //拷贝到buf中
    memset(&MeterLib[node][5],0x00,123);  //清零
    if(MeterLibDataNum > 0)
    {
      MeterLibDataNum--;
    }
  }
  return(len);
}

uint8_t Get_Meter_Lib_DataItem_Num(void)
{
  return( MeterLibDataNum );
}






  
/*******************************************************************************
** Function name:           Get_Meter_Lib_NodeNum
** Descriptions:            根据表号取得节点号  例如主站需要某一个表的冻结数据  可以通过此方法取得
** input parameters:        void
** output parameters:       void
** Returned value:          表号插入进去 并返回位置号
** Created by:              程卫玺
** Created Date:            2016-2-22    STM32  72M主频  遍历128个点 需要125u   
*******************************************************************************/
int8_t Get_Meter_Lib_NodeNum(uint8_t *id)
{
  uint8_t num=0;
  
  for(num=0;num<MeterLibraryMaxNum;num++)
  {
    if((memcmp(&id[0],&MeterLib[num][0],4)==0))
    {
      break;
    }
  }
  
  return(num);
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_MeterNum
** Descriptions:            根据节点编号 取得对应表号
** input parameters:        void
** output parameters:       void
** Returned value:          表号插入进去 并返回位置号
** Created by:              程卫玺
** Created Date:            2016-2-22    STM32  72M主频  遍历128个点 需要125u   
*******************************************************************************/
void Get_Meter_Lib_MeterNum(uint8_t *id,uint8_t nodenum)
{
  memcpy(&*id,&MeterLib[nodenum][0],4);
}


















