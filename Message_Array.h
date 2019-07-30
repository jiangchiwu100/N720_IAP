#ifndef __Message_Array_H
#define __Message_Array_H


#ifdef __cplusplus
extern "C" {
#endif 


#include "stdbool.h"
#include "stdint.h"

#define MegArr_Data_Type     uint8_t
#define MegArr_Line_Max      5
#define MegArr_List_Max      128  
typedef struct 
{
  MegArr_Data_Type In;     //指向数据输入位置
  MegArr_Data_Type Out;    //指向数据输出位置
  MegArr_Data_Type MsgNum; //数据个数   
  uint8_t Len[MegArr_Line_Max];//数组中的长度
  uint8_t Msg[MegArr_Line_Max][MegArr_List_Max];
}Str_Message;

  

void W_Message_Array_List(Str_Message *msgpoint, uint8_t data);
bool W_Message_Array_Line(Str_Message *msgpoint);
uint8_t R_Message_Array_Line(Str_Message *msgpoint, uint8_t *data);
bool Message_Empty(Str_Message *msgpoint);
uint8_t Message_Array_Num(Str_Message *msgpoint);



#ifdef __cplusplus
}
#endif
#endif