#ifndef __bsp_IAP_H
#define __bsp_IAP_H

#ifdef __cplusplus
extern "C" {
#endif 

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.
    

    
void iap_load_app();			//ִ��flash�����app����
    
    
#ifdef __cplusplus
}
#endif
#endif
