/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32F1XX)
*	�ļ����� : bsp.c
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ���������ģ������ļ�����Ҫ�ṩ bsp_Init()��������������á��������ÿ��c�ļ������ڿ�
*			  ͷ	��� #include "bsp.h" ���������е���������ģ�顣
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-03-01 armfly   ��ʽ����
*		V1.1    2015-08-02 Eric2013 ����uCOS-II���躯��
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*	�� �� ��: BSP_CPU_ClkFreq
*	����˵��: ��ȡϵͳʱ�ӣ�uCOS-II��Ҫʹ��
*	��    ��: ��
*	�� �� ֵ: ϵͳʱ��
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);
#ifdef PrintfEnable
    
    printf("----------ϵͳʱ��%d----------\n",(CPU_INT32U)rcc_clocks.HCLK_Frequency);
#endif
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);

}

void TestGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure ; 
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  
  
  
  //DI0
  
  /* ��1������GPIOʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* ʹ�� AFIO ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  /* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;	/* ���븡��ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
   /* Connect EXTI0 Line to pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
  
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  	/* �����ش��� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ��Ӳ���豸��ֻ��Ҫ����һ�Ρ��ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����
*			 ȫ�ֱ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{

 /*
  ����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
  �����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����
  
  ϵͳʱ��ȱʡ����Ϊ168MHz�������Ҫ���ģ������޸� system_stm32f4xx.c �ļ�
  */
  
  /* ���ȼ���������Ϊ4 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  bsp_InitUart();       /* ��ʼ������ */
  
  
  bsp_SetTIMforInt(TIM7, 100, 0, 0);   //����������ʱ��10ms ����debug�����жϽ��ղ���������֡ 
  bsp_SetTIMforInt(TIM6, 100, 0, 0);   //����������ʱ��10ms ����N720�����жϽ��ղ���������֡
  
  TIM_Cmd(TIM6, DISABLE);
  TIM_Cmd(TIM7, DISABLE);       //ʧ�ܶ�ʱ������Ҫʱ�ٴ�
  
  bsp_InitLed();
 
  Sx1278_Init();                //��������ģ��Ӳ����
  bsp_InitRS8025T_J();
  bsp_InitTemperature_J();
  bsp_InitN720_GPIO();
  
  
//  TestGPIO();
  GateWay_Variable_Init(); 
  Lora_Network_Init();
  N720_Mess_Q_Init();
//  ConfigInfo.LoraChannel = 0;
}



/*
*********************************************************************************************************
*	�� �� ��: BSP_Tick_Init
*	����˵��: ��ʼ��ϵͳ�δ�ʱ����ΪuCOS-II��ϵͳʱ�ӽ��ģ�1msһ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BSP_Tick_Init (void)
{
  CPU_INT32U  cpu_clk_freq;
  CPU_INT32U  cnts;
  
  cpu_clk_freq = BSP_CPU_ClkFreq();                           /* ��ȡϵͳʱ��  */
  
#if (OS_VERSION >= 30000u)
  cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;     
#else
  cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* ��õδ�ʱ���Ĳ���  */
#endif
  
  OS_CPU_SysTickInit(cnts);                                   /* ����Ĭ�ϵ���������ȼ�            */
}




void bsp_RunPer1ms(void)
{
  
}

void bsp_RunPer10ms(void)
{
  
}










/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
