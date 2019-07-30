#ifndef __Sx1278_Spi_H
#define __Sx1278_Spi_H


#ifdef __cplusplus
extern "C" {
#endif 


#include "stdbool.h"
#include "stdint.h"


//  RF_RESET    PORTa8

#define         Rf_Reset_Port         GPIOC 
#define         Rf_Reset_Pin          GPIO_Pin_5 
#define         Rf_Reset_En()         GPIO_SetBits(Rf_Reset_Port, Rf_Reset_Pin)
#define         Rf_Reset_Ds()         GPIO_ResetBits(Rf_Reset_Port, Rf_Reset_Pin)
  
//  RF_NSS      PORTB.1
#define         SSN_Port              GPIOB
#define         SSN_Pin               GPIO_Pin_15
#define         SSN_HIGH()            GPIO_SetBits(SSN_Port, SSN_Pin)
#define         SSN_LOW()             GPIO_ResetBits(SSN_Port, SSN_Pin)

//  RF_MOSI     PORTB.2
#define         MOSI_Port             GPIOB
#define         MOSI_Pin              GPIO_Pin_12
#define         MOSI_HIGH()           GPIO_SetBits(MOSI_Port, MOSI_Pin)
#define         MOSI_LOW()            GPIO_ResetBits(MOSI_Port, MOSI_Pin)
  
//RF_MISO       PORTB.10
#define         MISO_Port             GPIOB
#define         MISO_Pin              GPIO_Pin_13
#define         MISO_IN()             GPIO_ReadInputDataBit(MISO_Port,MISO_Pin)  

//RF_SCK        PORTB.13
#define         SCK_Port              GPIOF
#define         SCK_Pin               GPIO_Pin_11
#define         SCK_HIGH()            GPIO_SetBits(SCK_Port, SCK_Pin)
#define         SCK_LOW()             GPIO_ResetBits(SCK_Port, SCK_Pin)

//VC1           PORTF.11
#define         VC1_Port              GPIOB
#define         VC1_Pin               GPIO_Pin_14
#define         LoraTx_Enable()       GPIO_ResetBits(VC1_Port, VC1_Pin)
#define         LoraRx_Enable()       GPIO_SetBits(VC1_Port, VC1_Pin)

//  RF_IRQ0       Pb.15
#define         IRQ0_Port               GPIOC
#define         IRQ0_Pin                GPIO_Pin_4
 
/*******************************************************************************
** Function name:           SPI_Init
** Descriptions:            SPI��ʼ��
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void Sx1278_GPIO_Init(void);
/*******************************************************************************
** Function name:           SPI_Send_Dat
** Descriptions:            SPI����һ��ֱ��  ģʽ0
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SPI_Send_Dat(unsigned char dat);

/*******************************************************************************
** Function name:           SPI_Receiver_Dat
** Descriptions:            SPI����һ��ֱ��  ģʽ0
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
unsigned char SPI_Receiver_Dat(void);

/*******************************************************************************
** Function name:           SX1276Reset
** Descriptions:            ��λLORAоƬ
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void Sx1278_Hardware_Reset(void);

/*******************************************************************************
** Function name:           SX1276WriteBuffer
** Descriptions:            ��Ĵ�����ַ������������
** input parameters:        uint8_t addr,�Ĵ�����ַ
                            uint8_t *buffer,��������ָ��
                            uint8_t sizeָ�볤��
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );

/*******************************************************************************
** Function name:           SX1276ReadBuffer
** Descriptions:            ��Ĵ�����ַ����������
** input parameters:        uint8_t addr,�Ĵ�����ַ
                            uint8_t *buffer,�洢����ָ��
                            uint8_t sizeҪ���ĳ���
** output parameters:       ���ݷ��ص�*buffer��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );

/*******************************************************************************
** Function name:           SX1276Write
** Descriptions:            ��Ĵ�����ַд1�ֽ�����
** input parameters:        uint8_t addr,�Ĵ�����ַ
                            uint8_t data����
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276Write( uint8_t addr, uint8_t data );

/*******************************************************************************
** Function name:           SX1276Read
** Descriptions:            ��Ĵ�����ַ��1�ֽ�����
** input parameters:        uint8_t addr,�Ĵ�����ַ
                            uint8_t *buffer,�洢����ָ��
** output parameters:       ���ݷ��ص�*buffer��
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276Read( uint8_t addr, uint8_t *data );

/*******************************************************************************
** Function name:           SX1276WriteFifo
** Descriptions:            ��FIFOд����
** input parameters:        uint8_t size����
                            uint8_t *buffer,�洢����ָ��
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276WriteFifo( uint8_t *buffer, uint8_t size );

/*******************************************************************************
** Function name:           SX1276ReadFifo
** Descriptions:            ��FIFO������
** input parameters:        uint8_t size����
                            uint8_t *buffer,�洢����ָ��
** output parameters:       uint8_t *buffer �洢��ȡ����
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276ReadFifo( uint8_t *buffer, uint8_t size );

/*******************************************************************************
** Function name:           SX1276WriteRxTx
** Descriptions:            TX/RX��PA�л�  �棺��ΪTX���٣���ΪRX   ΪӲ������PA����IO��
** input parameters:        bool txEnable  �л��߼�
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void SX1276WriteRxTx( bool txEnable );

/*******************************************************************************
** Function name:           SpiTest
** Descriptions:            ����RF����Standbyģʽ���ж�RF SPIͨ���Ƿ�ɹ�
** input parameters:        bool txEnable  �л��߼�
** output parameters:       true �ɹ���false ʧ�ܡ�
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
bool SpiTest(void);




  
#ifdef __cplusplus
}
#endif
#endif
