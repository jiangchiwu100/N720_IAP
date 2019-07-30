#ifndef __bsp_IAP_H
#define __bsp_IAP_H

#ifdef __cplusplus
extern "C" {
#endif 

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
    

    
void iap_load_app();			//执行flash里面的app程序
    
    
#ifdef __cplusplus
}
#endif
#endif
