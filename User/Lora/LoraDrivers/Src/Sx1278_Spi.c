
#include "includes.h"


 

void Sx1278_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure ; 
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  
  // SCK         	
  GPIO_InitStructure.GPIO_Pin = SCK_Pin ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(SCK_Port,&GPIO_InitStructure);  
  SCK_HIGH();
  //MISO     
  GPIO_InitStructure.GPIO_Pin = MISO_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(MISO_Port,&GPIO_InitStructure); 
  
  //MOSI
  GPIO_InitStructure.GPIO_Pin = MOSI_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(MOSI_Port,&GPIO_InitStructure); 
  MOSI_HIGH();
  //CS
  GPIO_InitStructure.GPIO_Pin = SSN_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(SSN_Port,&GPIO_InitStructure); 
  SSN_HIGH();
  //RESET
  GPIO_InitStructure.GPIO_Pin = Rf_Reset_Pin;			
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
  GPIO_Init(Rf_Reset_Port, &GPIO_InitStructure);
 
  //VC1
  GPIO_InitStructure.GPIO_Pin   = VC1_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
  GPIO_Init(VC1_Port,&GPIO_InitStructure); 
  
  //DI0
  
  /* 第1步：打开GPIO时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* 使能 AFIO 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  /* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	/* 输入浮空模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
   /* Connect EXTI0 Line to pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
  
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  	/* 上升沿触发 */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
}
/*******************************************************************************
** Function name:           SX1276Reset
** Descriptions:            复位LORA芯片
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void Sx1278_Hardware_Reset(void)
{
  Rf_Reset_Ds();           
  SysCtlDelay(1000*(72000000/3000000));          
  Rf_Reset_En();       
  SysCtlDelay(1000*(72000000/3000000));  
}

void SPI_Send_Dat(unsigned char dat)
{
  unsigned char n;
  
  for(n=0;n<8;n++)
  {
    SCK_LOW();
    
    if(dat&0x80)
      MOSI_HIGH();
    else
      MOSI_LOW();
    dat<<=1;
    
    SCK_HIGH(); 
  }  
  SCK_LOW();
}

unsigned char SPI_Receiver_Dat(void)
{
  unsigned char n ,dat; 
  for(n=0;n<8;n++)
  {
    SCK_LOW();
    
    dat<<=1;
    if(MISO_IN())
      dat|=0x01;
    else dat&=0xfe;
    
    SCK_HIGH();
  }
  SCK_LOW();
  return dat;
}

/*******************************************************************************
** Function name:           SX1276WriteBuffer
** Descriptions:            向寄存器地址连续发送数据
** input parameters:        uint8_t addr,寄存器地址
                            uint8_t *buffer,发送数组指针
                            uint8_t size指针长度
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
  uint8_t i;  
  
  SSN_LOW();  
  
  SPI_Send_Dat(addr| 0x80);
  for( i = 0; i < size; i++ )
  {   
    SPI_Send_Dat(buffer[i]);
  }  
  
  SSN_HIGH();
}

/*******************************************************************************
** Function name:           SX1276ReadBuffer
** Descriptions:            向寄存器地址连续读数据
** input parameters:        uint8_t addr,寄存器地址
                            uint8_t *buffer,存储数组指针
                            uint8_t size要读的长度
** output parameters:       数据返回到*buffer中
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
 
void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
  uint8_t i;
  
  SSN_LOW();  

  SPI_Send_Dat(addr & 0x7F);
  for( i = 0; i < size; i++ )
  {   
    buffer[i] = SPI_Receiver_Dat();                 // Store data from last data RX
  }  

  SSN_HIGH();
} 

 /*******************************************************************************
** Function name:           SX1276Write
** Descriptions:            向寄存器地址写1字节数据
** input parameters:        uint8_t addr,寄存器地址
                            uint8_t data数据
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276Write( uint8_t addr, uint8_t data )
{
  OS_CPU_SR  cpu_sr = 0;
  
  cpu_sr = OS_CPU_SR_Save();
  
  SX1276WriteBuffer( addr, &data, 1 );
  
  OS_CPU_SR_Restore(cpu_sr); 
}

/*******************************************************************************
** Function name:           SX1276Read
** Descriptions:            向寄存器地址读1字节数据
** input parameters:        uint8_t addr,寄存器地址
                            uint8_t *buffer,存储数组指针
** output parameters:       数据返回到*buffer中
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276Read( uint8_t addr, uint8_t *data )
{
  OS_CPU_SR  cpu_sr = 0;
  
  cpu_sr = OS_CPU_SR_Save();
  
  SX1276ReadBuffer( addr, data, 1 );
  
  OS_CPU_SR_Restore(cpu_sr);
}

/*******************************************************************************
** Function name:           SX1276WriteFifo
** Descriptions:            向FIFO写数据
** input parameters:        uint8_t size长度
                            uint8_t *buffer,存储数组指针
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
  OS_CPU_SR  cpu_sr = 0;
  
  cpu_sr = OS_CPU_SR_Save();
  
  SX1276WriteBuffer( 0, buffer, size );
    
  OS_CPU_SR_Restore(cpu_sr);
}

/*******************************************************************************
** Function name:           SX1276ReadFifo
** Descriptions:            向FIFO读数据
** input parameters:        uint8_t size长度
                            uint8_t *buffer,存储数组指针
** output parameters:       uint8_t *buffer 存储读取内容
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
  OS_CPU_SR  cpu_sr = 0;
  
  cpu_sr = OS_CPU_SR_Save();
  
  SX1276ReadBuffer( 0, buffer, size );
      
  OS_CPU_SR_Restore(cpu_sr);
}





