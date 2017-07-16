/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_stack.c
**创   建   人: 周江村
**最后修改日期: 2013.10.05
**描        述: wind os的堆栈相关的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.10.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.10.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

//#include "wind_core_inc.h"
#include "wind_types.h"
#include "wind_stack.h"
#include "wind_os_hwif.h"


static w_err_t defaultreadempty(pstack_s pstk,void *data)
{
    pstk->emptycnt ++;
    return STACK_EMPTY;
}

static w_err_t defaultwritefull(pstack_s pstk,void *data)
{
    pstk->fullcnt ++;
    return STACK_FULL;
}
/*********************************************************************************************************
** 函数名称: wind_stack_create
** 功能描述: 初始化数据堆栈
** 输　入: Buf      ：为堆栈分配的存储空间地址
**         lenth：为堆栈分配的存储空间大小（字节）
**         DataWid  :缓冲区数据宽度
**         ReadEmpty：为堆栈读空时处理程序
**         WriteFull：为堆栈写满时处理程序
** 输　出: STACK_ERR:参数错误
**         STACK_OK:成功
** 全局变量: 无
** 调用模块: wind_close_interrupt,wind_open_interrupt()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

w_err_t wind_stack_create(void *Buf,
                          w_uint32_t lenth,
                          w_uint16_t DataWid,
                          w_err_t (* ReadEmpty)(pstack_s pstk,void *data),
                          w_err_t (* WriteFull)(pstack_s pstk,void *data)
                          )
{
    pstack_s pstk;
    if ((Buf != NULL) && (lenth >= sizeof(stack_s)))        // && 判断参数是否有效 
    {
        pstk = (pstack_s )Buf;

        wind_close_interrupt();
        if(ReadEmpty)                                                        // 初始化结构体数据 
            pstk->ReadEmpty = ReadEmpty;
        else
            pstk->ReadEmpty = defaultreadempty;
        if(WriteFull)
            pstk->WriteFull = WriteFull;
        else
            pstk->WriteFull = defaultwritefull;
         // 计算堆栈可以存储的数据数目     
        pstk->MaxData = (lenth - (w_uint32_t)(((pstack_s)0)->Buf)) / sizeof(STACK_DATA_TYPE);
                                       
        pstk->Top = pstk->Buf + pstk->MaxData;               // 计算数据缓冲的结束地址 
        pstk->Out = pstk->Buf;
        pstk->In = pstk->Buf;
        pstk->DataWid = DataWid;
        pstk->NData = 0;
        pstk->emptycnt = 0;
        pstk->fullcnt = 0;        
        wind_open_interrupt();
        return STACK_OK;
    }
    else
    {
        return STACK_ERR;
    }
}


/*********************************************************************************************************
** 函数名称: wind_stack_read
** 功能描述: 获取堆栈中的数据
** 输　入: Ret:存储返回的消息的地址
**         Buf:指向堆栈的指针
** 输　出: STACK_ERR     ：参数错误
**         STACK_OK   ：收到消息
**         STACK_EMPTY：无消息
** 全局变量: 无
** 调用模块: wind_close_interrupt,wind_open_interrupt()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
w_err_t wind_stack_read(void *Buf,void *Ret)
{
    w_err_t err;
    w_int8_t i;
    pstack_s pstk;
    //w_uint8_t *pEnd;
    w_uint8_t *ret = (w_uint8_t *)Ret;
    err = STACK_ERR;
    if (Buf != NULL)                                            /* 堆栈是否有效 */
    {                                                           /* 有效 */
        pstk = (pstack_s)Buf;
        
        wind_close_interrupt();
        if (pstk->NData >= pstk->DataWid)                     /* 堆栈是否为空 */
        {                                                       /* 不空         */
            pstk->In = pstk->Out;
            for(i = 0;i < pstk->DataWid;i ++)
            {
                *(ret + i) = pstk->Out[0];                               /* 数据出队     */
                pstk->Out++;                                       /* 调整出队指针 */
                pstk->NData--;                                     /* 数据减少      */
            }
            pstk->Out -= pstk->DataWid;
            err = STACK_OK;
        }
        else
        {                                                       /* 空              */
               err = pstk->ReadEmpty(pstk,Ret);
        }
        wind_open_interrupt();
    }
    return err;
}



/*********************************************************************************************************
** 函数名称: wind_stack_write
** 功能描述: FIFO方式发送数据
** 输　入: Buf :指向堆栈的指针
**         Data:消息数据
** 输　出: STACK_ERR   :参数错误
**         STACK_FULL:堆栈满
**         STACK_OK  :发送成功
** 全局变量: 无
** 调用模块: wind_close_interrupt,wind_open_interrupt()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_STACK_WRITE
#define EN_STACK_WRITE      0
#endif

#if EN_STACK_WRITE > 0

w_err_t wind_stack_write(void *Buf, void *Data)
{
    w_err_t err;
    w_int8_t i;
    pstack_s pstk;
    //w_uint8_t *pEnd;
    w_uint8_t *data = (w_uint8_t *)Data;
    err = STACK_ERR;
    if (Buf != NULL)                                                    /* 堆栈是否有效 */
    {
        pstk = (pstack_s )Buf;
        wind_close_interrupt();
        if (pstk->NData <= pstk->MaxData - pstk->DataWid)                              /* 堆栈是否满  */
        {                                                               /* 不满        */
            for(i = 0;i < pstk->DataWid;i ++)
            {
                pstk->In[0] = *(data + i);                                        /* 数据入队    */
                pstk->In++;
                pstk->NData++;                                             /* 数据增加    */
            }
            pstk->Out = pstk->In - pstk->DataWid;
            err = STACK_OK;
        }
        else
        {                                                               /* 满           */
            err = STACK_FULL;
            if (pstk->WriteFull != NULL)                               /* 调用用户处理函数 */
            {
                err = pstk->WriteFull(pstk, Data);
            }
        }
        wind_open_interrupt();
    }
    return err;
}
#endif



/*********************************************************************************************************
** 函数名称: wind_stack_datalen
** 功能描述: 取得堆栈中数据数
** 输　入: Buf:指向堆栈的指针
** 输　出: 消息数
** 全局变量: 无
** 调用模块: wind_close_interrupt,wind_open_interrupt()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_STACK_NDATA
#define EN_STACK_NDATA    0
#endif

#if EN_STACK_NDATA > 0

w_uint16_t wind_stack_datalen(void *Buf)
{
    w_uint16_t temp;
    temp = 0;                                                   /* 堆栈无效返回0 */
    if (Buf != NULL)
    {
        wind_close_interrupt();
        temp = ((pstack_s )Buf)->NData / ((pstack_s )Buf)->DataWid;
        wind_open_interrupt();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: wind_stack_size
** 功能描述: 取得堆栈总容量
** 输　入: Buf:指向堆栈的指针
** 输　出: 堆栈总容量
** 全局变量: 无
** 调用模块: wind_close_interrupt,wind_open_interrupt()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_stack_sIZE
#define EN_stack_sIZE    0
#endif

#if EN_stack_sIZE 

w_uint16_t wind_stack_size(void *Buf)
{
    w_uint16_t temp;
    temp = 0;                                                   /* 堆栈无效返回0 */
    if (Buf != NULL)
    {
        wind_close_interrupt();
        temp = ((pstack_s )Buf)->MaxData / ((pstack_s )Buf)->DataWid;
        wind_open_interrupt();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: wind_stack_flush
** 功能描述: 清空堆栈
** 输　入: Buf:指向堆栈的指针
** 输　出: 无
** 全局变量: 无
** 调用模块: wind_close_interrupt,wind_open_interrupt()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef EN_STACK_FLUSH
#define EN_STACK_FLUSH    0
#endif

#if EN_STACK_FLUSH 

void wind_stack_flush(void *Buf)
{
    pstack_s pstk;
    if (Buf != NULL)                                                /* 堆栈是否有效 */
    {                                                               /* 有效         */
        pstk = (pstack_s)Buf;
        wind_close_interrupt();
        pstk->NData = 0;                                           /* 数据数目为0 */
        pstk->Top = pstk->Buf + pstk->MaxData;               // 计算数据缓冲的结束地址 
        pstk->Out = pstk->Buf;
        pstk->In = pstk->Buf;
        wind_open_interrupt();
    }
}
#endif



/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

