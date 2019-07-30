#ifndef __SoftwareTimer_H
#define __SoftwareTimer_H

#ifdef __cplusplus
extern "C" {
#endif 
 
 
/*
	�ڴ˶������ɸ������ʱ��ȫ�ֱ���
	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TMR_COUNT	2		/* �����ʱ���ĸ��� ����ʱ��ID��Χ 0 - 3) */

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* һ�ι���ģʽ */
	TMR_AUTO_MODE = 1		/* �Զ���ʱ����ģʽ */
}TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint8_t Mode;		/* ������ģʽ��1���� */
	volatile uint8_t Flag;		/* ��ʱ�����־  */
	volatile uint32_t Count;	/* ������ */
	volatile uint32_t PreLoad;	/* ������Ԥװֵ */
}SOFT_TMR;
  


void bsp_InitTimer(void);
void Soft_Timer_Tick(void);
void bsp_SoftTimerDec(SOFT_TMR *_tmr);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
uint8_t bsp_CheckTimer(uint8_t _id);
void bsp_StopTimer(uint8_t _id);


#ifdef __cplusplus
}
#endif
#endif
