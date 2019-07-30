/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F1XX)
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。主程序的每个c文件可以在开
*			  头	添加 #include "bsp.h" 来包含所有的外设驱动模块。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-03-01 armfly   正式发布
*		V1.1    2015-08-02 Eric2013 增加uCOS-II所需函数
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*	函 数 名: BSP_CPU_ClkFreq
*	功能说明: 获取系统时钟，uCOS-II需要使用
*	形    参: 无
*	返 回 值: 系统时钟
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);
#ifdef PrintfEnable
    
    printf("----------系统时钟%d----------\n",(CPU_INT32U)rcc_clocks.HCLK_Frequency);
#endif
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);

}

void TestGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure ; 
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  
  
  
  //DI0
  
  /* 第1步：打开GPIO时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* 使能 AFIO 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  /* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;	/* 输入浮空模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
   /* Connect EXTI0 Line to pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
  
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  	/* 上升沿触发 */
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
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{

 /*
  由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
  启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。
  
  系统时钟缺省配置为168MHz，如果需要更改，可以修改 system_stm32f4xx.c 文件
  */
  
  /* 优先级分组设置为4 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  bsp_InitUart();       /* 初始化串口 */
  
  
  bsp_SetTIMforInt(TIM7, 100, 0, 0);   //开启基本定时器10ms 用于debug串口中断接收不定长数据帧 
  bsp_SetTIMforInt(TIM6, 100, 0, 0);   //开启基本定时器10ms 用于N720串口中断接收不定长数据帧
  
  TIM_Cmd(TIM6, DISABLE);
  TIM_Cmd(TIM7, DISABLE);       //失能定时器，需要时再打开
  
  bsp_InitLed();
 
  Sx1278_Init();                //配置无线模块硬件层
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
*	函 数 名: BSP_Tick_Init
*	功能说明: 初始化系统滴答时钟做为uCOS-II的系统时钟节拍，1ms一次
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BSP_Tick_Init (void)
{
  CPU_INT32U  cpu_clk_freq;
  CPU_INT32U  cnts;
  
  cpu_clk_freq = BSP_CPU_ClkFreq();                           /* 获取系统时钟  */
  
#if (OS_VERSION >= 30000u)
  cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;     
#else
  cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* 获得滴答定时器的参数  */
#endif
  
  OS_CPU_SysTickInit(cnts);                                   /* 这里默认的是最低优先级            */
}




void bsp_RunPer1ms(void)
{
  
}

void bsp_RunPer10ms(void)
{
  
}










/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
