#include "includes.h"


static uint16_t ADC_ConvertedValue;

void bsp_InitTemperature_J(void)
{
  Temp_DMA_Config();
  Temp_ADC_Config();
}

/****************************************************************************** 
* 函数名称       : DMA_Configuraiton 
* 描述           : DMA 模块设置 
* 输入           : 无 
* 输出           : 无 
* 返回           : 无 
******************************************************************************/  
void Temp_DMA_Config(void)  
{  
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  DMA_InitTypeDef DMA_InitStructure;
  
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);  
  DMA_Cmd(DMA1_Channel1, ENABLE);
}  


/****************************************************************************** 
* 函数名称       : ADC_Configuraiton 
* 描述           : ADC 模块设置 
* 输入           : 无 
* 输出           : 无 
* 返回           : 无 
******************************************************************************/  
void Temp_ADC_Config(void)  
{  
  ADC_InitTypeDef ADC_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); 
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);
  ADC_TempSensorVrefintCmd(ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  
  ADC_Cmd(ADC1, ENABLE); 
  ADC_ResetCalibration(ADC1); 
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  
  //配置ADC1_DMA1_1中断    
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; //通道设置为串口1中断      
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //中断响应优先级0    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //打开中断    
  NVIC_Init(&NVIC_InitStructure);  
  
}  







/******************************************************************************* 
 * 函数名称: DMAChannel1_IRQHandler 
 * 函数功能: DMA中断处理函数 
 * 输入参数: 无 
 * 输出参数: 无 
 * 函数说明：无 
******************************************************************************/  
void DMA1_Channel1_IRQHandler(void)  
{  
  static uint8_t TemNum=0;
  static int32_t TemData[8] = {0};//防止上电初值低于预设电压
  static int32_t TemDataSum = 0;
  static int32_t Current_Temp;
         int32_t tem  =  0;
  
  
  if(DMA_GetITStatus(DMA1_IT_TC1))  //在DMA通道1传输完成中断中进行检测  
  {  
    tem = (0x6E2-ADC_ConvertedValue)/0x05+25;
    Current_Temp = (Glide_Average_Filter(&TemData[0],tem,TemNum,&TemDataSum))>>3 ;
    if(++TemNum>=8)
    {
      TemNum = 0;
    }

    ADC_Cmd(ADC1, DISABLE);//使能指定的ADC1  
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_GL1);//清DMA通道1的  
    /* 将按键代码发送到邮箱 */
    OSMboxPost(MboxGateWayTemp, &Current_Temp);
  }  
}

