
#ifndef __State_H
#define __State_H

#ifdef __cplusplus
extern "C" {
#endif 

  
#include "stdint.h"
#include "stdbool.h"
  
#define SET_1(a,b)      a=(a|(1<<b))  //a字节的第b位置1
#define SET_0(a,b)      a=(a&~(1<<b)) //a字节的第b位置0
#define SET_COM(a,b)    a=(a^(1<<b))//a字节的第b位置取反
#define GET(a,b)        ((a&(1<<b))>>b)    //获取a字节的b位置



void W_State(uint32_t *status,uint32_t sta,bool enable);
bool R_State(uint32_t *status,uint32_t sta);
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif
#endif
