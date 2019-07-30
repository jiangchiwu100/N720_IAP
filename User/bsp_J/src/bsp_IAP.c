#include "includes.h"



iapfun jump2app; 
//跳转到应用程序段
//appxaddr:用户代码起始地址.

void iap_load_app()
{
    TIM_Cmd(TIM6, DISABLE); 
    TIM_Cmd(TIM7, DISABLE);
    
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, DISABLE); 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,DISABLE);
    
    SPI_Cmd(SPI1, DISABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, DISABLE );
    USART_Cmd(USART1, DISABLE);		/* 关闭串口 */
    USART_Cmd(USART2, DISABLE);		/* 关闭串口 */
    __disable_irq(); //关中断 如IAP关中断 APP如果没用UCOS系统，APP
//	__set_PRIMASK(1);
    __disable_fault_irq();

    NVIC_SystemReset();
    
//    if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	
//	{ 
//		jump2app=(iapfun)*(vu32*)(appxaddr+4);
//        
//        __set_CONTROL(0);
//        //进入用户级线程模式 进入软中断后才可以回到特权级线程模式
//        //APP如使用系统如ucos必须要有此过程否则跳到IAP后，无法再次跳到APP
//		__set_MSP(*(vu32*)appxaddr);					
//		jump2app();									
//	}
}	