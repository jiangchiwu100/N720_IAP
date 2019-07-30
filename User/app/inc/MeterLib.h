
#ifndef __MeterLib_H
#define __MeterLib_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
//    typedef struct
//    { 
//      uint16_t  TaskID        ;//正在执行的任务序列号，0表示没有任务在执行，发送成功时=发送任务的序列号，接收成功时根据该序列号读取任务并组帧发送给前置机
//      uint8_t   TaskLoop      ;//对某一个任务的循环执行次数  最多3次 如果发现大约3次 就要清除这个任务
//      uint8_t   len           ;
//      uint8_t   buf[64]       ;//多少轮后超时，任务发送成功后设置为改任务的超时次数，每轮检测-1，如果==0则给前置机回复失败
//    }Str_PingTask        ;// 表库要素
  
  
  
 
   typedef  struct
    { 
      uint8_t    MeterID[6]    ;//表地址
      uint8_t    CommNum       ;//通信成功次数 如果CommNum等于0 就是说明此处为空
      uint16_t   TaskID        ;//任务号 
      uint8_t    TaskLoop      ;//任务循环执行次数  不能大于3次 如果超过3次就把它删除掉 
      uint8_t    DatLen        ;//下发数据的长度  如果等于0 就是没有数据
      uint8_t    DatBuf[64]    ;//下发数据
    }Str_MeterLib_Parm         ;// 表库要素
 
 
  
 
  

void ReadTest(void);
void WriteTest(uint8_t _Value);

void Maintenance_MeterLib(void);
uint8_t Seek_Lib_ID(uint8_t *id,uint16_t *stn);
uint8_t Empty_Meter_Lib_ID(uint16_t stn);
uint16_t Count_Meter_Lib_Num(void);
uint8_t Insert_Meter_Lib(uint8_t *id,uint16_t *stn);
uint8_t Insert_Meter_Lib_Data(uint8_t *id,uint8_t *buf,uint8_t len);
uint8_t Get_Meter_Lib_Data(uint16_t stn,uint8_t *buf);
uint8_t Get_Meter_Lib_ID(uint16_t stn,uint8_t *id,uint8_t num);

uint8_t Get_Meter_Lib_Data_State(uint16_t stn);
uint8_t Empty_Meter_Lib_Data(uint16_t stn);

#ifdef __cplusplus
}
#endif
#endif
