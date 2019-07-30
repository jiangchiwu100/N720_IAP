
#ifndef __MeterLibrary_H
#define __MeterLibrary_H


#ifdef __cplusplus
extern "C" {
#endif 
  
  //表库最大数量 128
  //每个表占据3个page 128个表需要384页  前0--114页存放其他数据 
  
  //0页---7页  存放表库  每页存放256字节 32个表 8页一共可以存放 8*32 = 256个表

  //8页开始存放其他参数配置 如网关号 等等 
  
/*

  115页
页地址 x*256

  */
#define MeterLibAddr(NodeNum)     ( NodeNum  * 8  ) 
  
#define MeterLibraryMaxNum      128
#define MeterLibraryPageOffset  114    
  
#define PageAddr( x )   ( (MeterLibraryPageOffset + x * 3 ) * 256 )//第x个表的页地址
#define MeterDataItem( x , y ) ( PageAddr( x ) + (y-1) * 128 )//第x个表的第y个数据帧
  
//数据分布说明 ： 每块表分配3个数据页  第一个128字节
  
  
  /*表号索引数组存放在eeprom中的位置地址   0-41 
  NodeNum  PageNum
  0  - 41     0
  42 - 83     1  
  84 - 125    2  
  126-        3  
  */
  

  // 第n个地址              MeterDataItem(n)
  // 第n个地址 第x个数据项  MeterDataItem(n) + MeterDataItem(x)
  // 第n个地址 日冻结       MeterDataItem(n) + MeterDayFreeze()
  
  /*
  实例 对地址0开始的数据分配
  MeterAddr(0)       0  ----127  表号   数据帧数量  唤醒时间点  通信失败次数  
  MeterDataItem(0)   128----255  存放主站下发的数据帧 0 
  MeterDataItem(1)   256----383  存放主站下发的数据帧 1 
  MeterDataItem(2)   384----511  存放主站下发的数据帧 2 
  MeterDayFreeze     512----639  存放每天的日冻结
  reserve            640----767  备用
  MeterAddr(1)       768   
  */
  
  /*
  MeterAddr(0)       0  ----127  具体地址划分
  通信失败次数---超过3次就删除整个表库 
  
  0--3 id
  4    数据帧数量
  5--6 唤醒时间点
  7    通信失败次数
  */
  
  typedef union 
  {
    uint8_t Meter[128];
    struct          
    {
      uint8_t  Id[4];            //节点表号
      uint8_t  Data[124];
    }Str_MeterLib;
  }Union_Meter_Lib;
  

  
void    Meter_Lib_Init(void);
uint8_t Count_Meter_Lib_Num(void);

uint8_t Insert_Meter_Lib(uint8_t *id);
void    Reset_Meter_Lib(uint8_t num);

int8_t  Get_Meter_Lib_NodeNum(uint8_t *id);
void    Get_Meter_Lib_MeterNum(uint8_t *id,uint8_t nodenum);

void    Insert_Meter_Lib_DataItem(uint8_t *buf,uint8_t len);
uint8_t Get_Meter_Lib_DataItem(uint8_t *buf,uint8_t node);
uint8_t Get_Meter_Lib_DataItem_Num(void);


#ifdef __cplusplus
}
#endif
#endif





