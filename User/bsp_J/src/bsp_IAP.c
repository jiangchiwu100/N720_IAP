#include "includes.h"



iapfun jump2app; 
//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.

void iap_load_app()
{
    TIM_Cmd(TIM6, DISABLE); 
    TIM_Cmd(TIM7, DISABLE);
    
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, DISABLE); 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,DISABLE);
    
    SPI_Cmd(SPI1, DISABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, DISABLE );
    USART_Cmd(USART1, DISABLE);		/* �رմ��� */
    USART_Cmd(USART2, DISABLE);		/* �رմ��� */
    __disable_irq(); //���ж� ��IAP���ж� APP���û��UCOSϵͳ��APP
//	__set_PRIMASK(1);
    __disable_fault_irq();

    NVIC_SystemReset();
    
//    if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	
//	{ 
//		jump2app=(iapfun)*(vu32*)(appxaddr+4);
//        
//        __set_CONTROL(0);
//        //�����û����߳�ģʽ �������жϺ�ſ��Իص���Ȩ���߳�ģʽ
//        //APP��ʹ��ϵͳ��ucos����Ҫ�д˹��̷�������IAP���޷��ٴ�����APP
//		__set_MSP(*(vu32*)appxaddr);					
//		jump2app();									
//	}
}	