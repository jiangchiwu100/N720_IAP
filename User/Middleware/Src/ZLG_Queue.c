#include "ZLG_Queue.h"
#include "os_cpu.h"

/*********************************************************************************************************
** Function name:       queueCreate
** Descriptions:        ��ʼ�����ݶ���
** input parameters:   *vBuf:       Ϊ���з���Ĵ洢�ռ��ַ������Ϊ�ֶ���
**                      sizeOfBuf:  Ϊ���з���Ĵ洢�ռ��С���ֽڣ����������24
**                     *readEmpty:  Ϊ���ж���ʱ�������
**                     *writeFull:  Ϊ����д��ʱ�������
** output parameters:   None
** Returned value:      1(TRUE)���ɹ���0(FALSE)����������
*********************************************************************************************************/
uint8_t queueCreate(void *vBuf,uint32_t sizeOfBuf,void (*readEmpty)(void),void (*writeFull)(void))
{
  tDATAQUEUE  *Queue;

  OS_CPU_SR  cpu_sr = 0;
 
    
    
    if ((vBuf      == (void *)0) ||
        (sizeOfBuf <= sizeof(tDATAQUEUE))) {                            /*  �жϲ����Ƿ���Ч            */
        return (0);                                                     /*  ���ز�������                */
    }

    Queue = (tDATAQUEUE *)vBuf;

    /*
     *  ��ʼ����������
     */
    OS_ENTER_CRITICAL();

    Queue->MaxData   = (sizeOfBuf - (uint32_t)(((tDATAQUEUE *)0)->Buf)) / 
                       sizeof(QUEUE_DATA_TYPE);
    Queue->End       = Queue->Buf + Queue->MaxData;
    Queue->Out       = Queue->Buf;
    Queue->In        = Queue->Buf;
    Queue->NData     = 0;
    
#if Q_FLOW_CTRL_ENABLE
    Queue->readEmpty = readEmpty;
    Queue->writeFull = writeFull;
#endif

    OS_EXIT_CRITICAL();

    return (1);
}

/*********************************************************************************************************
** Function name:       queueRead
** Descriptions:        ��ȡ�����е�����
** input parameters:   *vBuf:   ָ����е�ָ��
** output parameters:  *Ret:    ���ص�����
** Returned value:      1(TRUE)���ɹ���0(FALSE)����������

float b=3.14,*a=&b;
int *p=(int *)a;              //������������ģ�ǰ����int *�������зֺš���ʾ��ָ��a������ת��Ϊ����ָ���ٸ���p��
*********************************************************************************************************/
uint8_t queueRead(void *vBuf, QUEUE_DATA_TYPE *Ret)
{
    uint8_t      err = 0;
    tDATAQUEUE  *Queue;
OS_CPU_SR  cpu_sr = 0;
    if (vBuf == (void *)0) return (0);                                  /*  ���ز�������                */

    Queue = (tDATAQUEUE *)vBuf;

    if (Queue->NData > 0) {                                             /*  �����Ƿ�Ϊ��                */
        OS_ENTER_CRITICAL();

        *Ret = Queue->Out[0];                                           /*  ���ݳ���                    */
        Queue->Out++;                                                   /*  ��������ָ��                */
        Queue->NData--;                                                 /*  ���ݼ���                    */
       
        if (Queue->Out >= Queue->End) {
            Queue->Out  = Queue->Buf;
        }

        OS_EXIT_CRITICAL();

        err = 1;
    }
    
#if Q_FLOW_CTRL_ENABLE
    if (Queue->NData <= Q_FLOW_CTRL_LOWER_THRESHOLD) {                  /*  ��������������              */
        if (Queue->readEmpty != 0) {                                    /*  �����û�������            */
            Queue->readEmpty();
        }
    }
#endif

    return (err);
}

/*********************************************************************************************************
** Function name:       queueWrite
** Descriptions:        FIFO��ʽ��������
** input parameters:   *vBuf:   ָ����е�ָ��
**                      data:   ��������
** output parameters:   None
** Returned value:      1(TRUE)���ɹ���0(FALSE)����������

#define NULL ((void *)0)
����������Ч��ָ��
(void *)0 ���ǽ�0ǿ��ת��Ϊ(void *)���͵�ָ��
char *ch = (void *)0;//chָ���ַ0
*********************************************************************************************************/
uint8_t queueWrite(void *vBuf, QUEUE_DATA_TYPE data)
{
    uint8_t      err = 0;
    tDATAQUEUE  *Queue;
OS_CPU_SR  cpu_sr = 0;
    if (vBuf == (void *)0) return (0);                                  /*  ���ز�������                */

    Queue = (tDATAQUEUE *)vBuf;

    if (Queue->NData < Queue->MaxData) {                                /*  �����Ƿ���                  */
        OS_ENTER_CRITICAL();

        Queue->In[0] = data;                                            /*  �������                    */
        Queue->In++;                                                    /*  �������ָ��                */
        Queue->NData++;                                                 /*  ��������                    */

        if (Queue->In >= Queue->End) {
            Queue->In = Queue->Buf;
        }
        
        OS_EXIT_CRITICAL();
        
        err = 1;
    }

#if Q_FLOW_CTRL_ENABLE
    if (Queue->NData >= (uint16_t)(Queue->MaxData - 
                        Q_FLOW_CTRL_UPPER_THRESHOLD)) {                 /*  ��������������              */
        if (Queue->writeFull != 0) {                                    /*  �����û�������            */
            Queue->writeFull();
        }
    }
#endif

    return (err);
}

/*********************************************************************************************************
** Function name:       queueNData
** Descriptions:        ȡ�ö��������ݸ���
** input parameters:   *vBuf:   ָ����е�ָ��
** output parameters:   None
** Returned value:      ������
*********************************************************************************************************/
uint16_t queueNData(void *vBuf)
{
    if (vBuf == (void *)0) return (0);                                  /*  ���ز�������                */
    return (((tDATAQUEUE *)vBuf)->NData);/*ָ��vBuf������ת��ΪtDATAQUEUE*/
}
/*********************************************************************************************************
** Function name:       queueSize
** Descriptions:        ȡ�ö���������
** input parameters:   *vBuf:   ָ����е�ָ��
** output parameters:   None
** Returned value:      ����������
*********************************************************************************************************/
uint16_t queueSize(void *vBuf)
{
    if (vBuf == (void *)0) return (0);                                  /*  ���ز�������                */
    return (((tDATAQUEUE *)vBuf)->MaxData);
}
/*********************************************************************************************************
** Function name:       queueFlush
** Descriptions:        ��ն���
** input parameters:   *vBuf:   ָ����е�ָ��
** output parameters:   None
** Returned value:      1(TRUE)���ɹ���0(FALSE)����������
*********************************************************************************************************/
void queueFlush(void *vBuf)
{
    tDATAQUEUE  *Queue;
    OS_CPU_SR  cpu_sr = 0;
    if (vBuf == (void *)0) return;                                      /*  ���ز�������                */

    Queue = (tDATAQUEUE *)vBuf;
    
    OS_ENTER_CRITICAL();

    Queue->Out   = Queue->Buf;
    Queue->In    = Queue->Buf;
    Queue->NData = 0;                                                   /*  ������ĿΪ0                 */

    OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
    End Of File
********************************************************************************************************/

