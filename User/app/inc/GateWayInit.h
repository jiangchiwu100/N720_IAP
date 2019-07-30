#ifndef __GateWayInit_H
#define __GateWayInit_H

#ifdef __cplusplus
extern "C" {
#endif 
  
 
  
extern OS_STK AppTaskGateWayInitStk[ ];	
  
void AppTaskGateWayInit(void *p_arg);
void GateWayInit(void);


  
#ifdef __cplusplus
}
#endif
#endif