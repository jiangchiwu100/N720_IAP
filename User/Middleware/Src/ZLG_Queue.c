#include "ZLG_Queue.h"
#include "os_cpu.h"

/*********************************************************************************************************
** Function name:       queueCreate
** Descriptions:        初始化数据队列
** input parameters:   *vBuf:       为队列分配的存储空间地址，必须为字对齐
**                      sizeOfBuf:  为队列分配的存储空间大小（字节），必须大于24
**                     *readEmpty:  为队列读空时处理程序
**                     *writeFull:  为队列写满时处理程序
** output parameters:   None
** Returned value:      1(TRUE)：成功，0(FALSE)：参数错误
*********************************************************************************************************/
uint8_t queueCreate(void *vBuf,uint32_t sizeOfBuf,void (*readEmpty)(void),void (*writeFull)(void))
{
  tDATAQUEUE  *Queue;

  OS_CPU_SR  cpu_sr = 0;
 
    
    
    if ((vBuf      == (void *)0) ||
        (sizeOfBuf <= sizeof(tDATAQUEUE))) {                            /*  判断参数是否有效            */
        return (0);                                                     /*  返回参数错误                */
    }

    Queue = (tDATAQUEUE *)vBuf;

    /*
     *  初始化队列数据
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
** Descriptions:        获取队列中的数据
** input parameters:   *vBuf:   指向队列的指针
** output parameters:  *Ret:    返回的数据
** Returned value:      1(TRUE)：成功，0(FALSE)：参数错误

float b=3.14,*a=&b;
int *p=(int *)a;              //这个才是完整的，前面有int *，后面有分号。表示将指针a的类型转换为整型指针再赋给p。
*********************************************************************************************************/
uint8_t queueRead(void *vBuf, QUEUE_DATA_TYPE *Ret)
{
    uint8_t      err = 0;
    tDATAQUEUE  *Queue;
OS_CPU_SR  cpu_sr = 0;
    if (vBuf == (void *)0) return (0);                                  /*  返回参数错误                */

    Queue = (tDATAQUEUE *)vBuf;

    if (Queue->NData > 0) {                                             /*  队列是否为空                */
        OS_ENTER_CRITICAL();

        *Ret = Queue->Out[0];                                           /*  数据出队                    */
        Queue->Out++;                                                   /*  调整出队指针                */
        Queue->NData--;                                                 /*  数据减少                    */
       
        if (Queue->Out >= Queue->End) {
            Queue->Out  = Queue->Buf;
        }

        OS_EXIT_CRITICAL();

        err = 1;
    }
    
#if Q_FLOW_CTRL_ENABLE
    if (Queue->NData <= Q_FLOW_CTRL_LOWER_THRESHOLD) {                  /*  队列数据量过少              */
        if (Queue->readEmpty != 0) {                                    /*  调用用户处理函数            */
            Queue->readEmpty();
        }
    }
#endif

    return (err);
}

/*********************************************************************************************************
** Function name:       queueWrite
** Descriptions:        FIFO方式发送数据
** input parameters:   *vBuf:   指向队列的指针
**                      data:   数据数据
** output parameters:   None
** Returned value:      1(TRUE)：成功，0(FALSE)：参数错误

#define NULL ((void *)0)
用来定义无效的指针
(void *)0 就是将0强制转化为(void *)类型的指针
char *ch = (void *)0;//ch指向地址0
*********************************************************************************************************/
uint8_t queueWrite(void *vBuf, QUEUE_DATA_TYPE data)
{
    uint8_t      err = 0;
    tDATAQUEUE  *Queue;
OS_CPU_SR  cpu_sr = 0;
    if (vBuf == (void *)0) return (0);                                  /*  返回参数错误                */

    Queue = (tDATAQUEUE *)vBuf;

    if (Queue->NData < Queue->MaxData) {                                /*  队列是否满                  */
        OS_ENTER_CRITICAL();

        Queue->In[0] = data;                                            /*  数据入队                    */
        Queue->In++;                                                    /*  调整入队指针                */
        Queue->NData++;                                                 /*  数据增加                    */

        if (Queue->In >= Queue->End) {
            Queue->In = Queue->Buf;
        }
        
        OS_EXIT_CRITICAL();
        
        err = 1;
    }

#if Q_FLOW_CTRL_ENABLE
    if (Queue->NData >= (uint16_t)(Queue->MaxData - 
                        Q_FLOW_CTRL_UPPER_THRESHOLD)) {                 /*  队列数据量过多              */
        if (Queue->writeFull != 0) {                                    /*  调用用户处理函数            */
            Queue->writeFull();
        }
    }
#endif

    return (err);
}

/*********************************************************************************************************
** Function name:       queueNData
** Descriptions:        取得队列中数据个数
** input parameters:   *vBuf:   指向队列的指针
** output parameters:   None
** Returned value:      数据数
*********************************************************************************************************/
uint16_t queueNData(void *vBuf)
{
    if (vBuf == (void *)0) return (0);                                  /*  返回参数错误                */
    return (((tDATAQUEUE *)vBuf)->NData);/*指针vBuf的类型转换为tDATAQUEUE*/
}
/*********************************************************************************************************
** Function name:       queueSize
** Descriptions:        取得队列总容量
** input parameters:   *vBuf:   指向队列的指针
** output parameters:   None
** Returned value:      队列总容量
*********************************************************************************************************/
uint16_t queueSize(void *vBuf)
{
    if (vBuf == (void *)0) return (0);                                  /*  返回参数错误                */
    return (((tDATAQUEUE *)vBuf)->MaxData);
}
/*********************************************************************************************************
** Function name:       queueFlush
** Descriptions:        清空队列
** input parameters:   *vBuf:   指向队列的指针
** output parameters:   None
** Returned value:      1(TRUE)：成功，0(FALSE)：参数错误
*********************************************************************************************************/
void queueFlush(void *vBuf)
{
    tDATAQUEUE  *Queue;
    OS_CPU_SR  cpu_sr = 0;
    if (vBuf == (void *)0) return;                                      /*  返回参数错误                */

    Queue = (tDATAQUEUE *)vBuf;
    
    OS_ENTER_CRITICAL();

    Queue->Out   = Queue->Buf;
    Queue->In    = Queue->Buf;
    Queue->NData = 0;                                                   /*  数据数目为0                 */

    OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
    End Of File
********************************************************************************************************/

