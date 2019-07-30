#include "includes.h"


static uint16_t ADC_ConvertedValue;

void bsp_InitTemperature_J(void)
{
  Temp_DMA_Config();
  Temp_ADC_Config();
}

/****************************************************************************** 
* ��������       : DMA_Configuraiton 
* ����           : DMA ģ������ 
* ����           : �� 
* ���           : �� 
* ����           : �� 
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
* ��������       : ADC_Configuraiton 
* ����           : ADC ģ������ 
* ����           : �� 
* ���           : �� 
* ����           : �� 
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
  
  //����ADC1_DMA1_1�ж�    
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; //ͨ������Ϊ����1�ж�      
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //�ж���Ӧ���ȼ�0    
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //���ж�    
  NVIC_Init(&NVIC_InitStructure);  
  
}  







/******************************************************************************* 
 * ��������: DMAChannel1_IRQHandler 
 * ��������: DMA�жϴ����� 
 * �������: �� 
 * �������: �� 
 * ����˵������ 
******************************************************************************/  
void DMA1_Channel1_IRQHandler(void)  
{  
  static uint8_t TemNum=0;
  static int32_t TemData[8] = {0};//��ֹ�ϵ��ֵ����Ԥ���ѹ
  static int32_t TemDataSum = 0;
  static int32_t Current_Temp;
         int32_t tem  =  0;
  
  
  if(DMA_GetITStatus(DMA1_IT_TC1))  //��DMAͨ��1��������ж��н��м��  
  {  
    tem = (0x6E2-ADC_ConvertedValue)/0x05+25;
    Current_Temp = (Glide_Average_Filter(&TemData[0],tem,TemNum,&TemDataSum))>>3 ;
    if(++TemNum>=8)
    {
      TemNum = 0;
    }

    ADC_Cmd(ADC1, DISABLE);//ʹ��ָ����ADC1  
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_GL1);//��DMAͨ��1��  
    /* ���������뷢�͵����� */
    OSMboxPost(MboxGateWayTemp, &Current_Temp);
  }  
}

