
#ifndef __State_H
#define __State_H

#ifdef __cplusplus
extern "C" {
#endif 

  
#include "stdint.h"
#include "stdbool.h"
  
#define SET_1(a,b)      a=(a|(1<<b))  //a�ֽڵĵ�bλ��1
#define SET_0(a,b)      a=(a&~(1<<b)) //a�ֽڵĵ�bλ��0
#define SET_COM(a,b)    a=(a^(1<<b))//a�ֽڵĵ�bλ��ȡ��
#define GET(a,b)        ((a&(1<<b))>>b)    //��ȡa�ֽڵ�bλ��



void W_State(uint32_t *status,uint32_t sta,bool enable);
bool R_State(uint32_t *status,uint32_t sta);
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif
#endif
