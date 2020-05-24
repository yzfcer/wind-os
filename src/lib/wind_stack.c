/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_stack.c
**Author: Jason Zhou
**Last Date: 2013.10.05
**Description: wind os的FILO栈结构相关功能
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2013.10.05
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2013.10.05
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_stack.h"
#include "wind_core.h"

#if WIND_STACK_SUPPORT

static w_err_t defaultreadempty(w_stack_s* pstk,void *data)
{
    pstk->emptycnt ++;
    return STACK_EMPTY;
}

static w_err_t defaultwritefull(w_stack_s* pstk,void *data)
{
    pstk->fullcnt ++;
    return STACK_FULL;
}
/*********************************************************************************************************
** 函数名称: wind_stack_create
** 功能描述: 初始化数据堆栈
** 输　入: mem      ：为堆栈分配的存储空间地址
**         lenth：为堆栈分配的存储空间大小（字节）
**         item_size  :缓冲区数据宽度
**         ReadEmpty：为堆栈读空时处理程序
**         WriteFull：为堆栈写满时处理程序
** 输　出: STACK_ERR:参数错误
**         STACK_OK:成功
** 全局变量: 无
** 调用模块: wind_disable_interrupt,wind_enable_interrupt()
**
** 作　者: Jason Zhou
** Date: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** Modify:
** Date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

w_err_t wind_stack_create(void *mem,
                          w_uint32_t size,
                          w_uint16_t item_size,
                          w_err_t (* ReadEmpty)(w_stack_s* pstk,void *data),
                          w_err_t (* WriteFull)(w_stack_s* pstk,void *data)
                          )
{
    w_stack_s* pstk;
    if ((mem != W_NULL) && (size >= sizeof(w_stack_s)))        // && 判断参数是否有效 
    {
        pstk = (w_stack_s* )mem;

        wind_disable_interrupt();
        if(ReadEmpty)                                                        // 初始化结构体数据 
            pstk->read_empty = ReadEmpty;
        else
            pstk->read_empty = defaultreadempty;
        if(WriteFull)
            pstk->write_full = WriteFull;
        else
            pstk->write_full = defaultwritefull;
         // 计算堆栈可以存储的数据数目     
        pstk->item_max = (size - (w_uint32_t)(((w_stack_s*)0)->buff)) / sizeof(STACK_DATA_TYPE);
                                       
        pstk->top = pstk->buff + pstk->item_max;               // 计算数据缓冲的结束地址 
        pstk->out = pstk->buff;
        pstk->in = pstk->buff;
        pstk->item_size = item_size;
        pstk->item_cnt = 0;
        pstk->emptycnt = 0;
        pstk->fullcnt = 0;
        wind_enable_interrupt();
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
**         mem:指向堆栈的指针
** 输　出: STACK_ERR     ：参数错误
**         STACK_OK   ：收到消息
**         STACK_EMPTY：无消息
** 全局变量: 无
** 调用模块: wind_disable_interrupt,wind_enable_interrupt()
**
** 作　者: Jason Zhou
** Date: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** Modify:
** Date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
w_err_t wind_stack_read(void *mem,void *Ret)
{
    w_err_t err;
    w_int8_t i;
    w_stack_s* pstk;
    //w_uint8_t *pEnd;
    w_uint8_t *ret = (w_uint8_t *)Ret;
    err = STACK_ERR;
    if (mem != W_NULL)                                            /* 堆栈是否有效 */
    {                                                           /* 有效 */
        pstk = (w_stack_s*)mem;
        
        wind_disable_interrupt();
        if (pstk->item_cnt >= pstk->item_size)                     /* 堆栈是否为空 */
        {                                                       /* 不空         */
            pstk->in = pstk->out;
            for(i = 0;i < pstk->item_size;i ++)
            {
                *(ret + i) = pstk->out[0];                               /* 数据出队     */
                pstk->out++;                                       /* 调整出队指针 */
                pstk->item_cnt--;                                     /* 数据减少      */
            }
            pstk->out -= pstk->item_size;
            err = STACK_OK;
        }
        else
        {                                                       /* 空              */
               err = pstk->read_empty(pstk,Ret);
        }
        wind_enable_interrupt();
    }
    return err;
}



/*********************************************************************************************************
** 函数名称: wind_stack_write
** 功能描述: FIFO方式发送数据
** 输　入: mem :指向堆栈的指针
**         Data:消息数据
** 输　出: STACK_ERR   :参数错误
**         STACK_FULL:堆栈满
**         STACK_OK  :发送成功
** 全局变量: 无
** 调用模块: wind_disable_interrupt,wind_enable_interrupt()
**
** 作　者: Jason Zhou
** Date: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** Modify:
** Date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


w_err_t wind_stack_write(void *mem, void *Data)
{
    w_err_t err;
    w_int8_t i;
    w_stack_s* pstk;
    //w_uint8_t *pEnd;
    w_uint8_t *data = (w_uint8_t *)Data;
    err = STACK_ERR;
    if (mem != W_NULL)                                                    /* 堆栈是否有效 */
    {
        pstk = (w_stack_s* )mem;
        wind_disable_interrupt();
        if (pstk->item_cnt <= pstk->item_max - pstk->item_size)                              /* 堆栈是否满  */
        {                                                               /* 不满        */
            for(i = 0;i < pstk->item_size;i ++)
            {
                pstk->in[0] = *(data + i);                                        /* 数据入队    */
                pstk->in++;
                pstk->item_cnt++;                                             /* 数据增加    */
            }
            pstk->out = pstk->in - pstk->item_size;
            err = STACK_OK;
        }
        else
        {                                                               /* 满           */
            err = STACK_FULL;
            if (pstk->write_full != W_NULL)                               /* 调用用户处理函数 */
            {
                err = pstk->write_full(pstk, Data);
            }
        }
        wind_enable_interrupt();
    }
    return err;
}




/*********************************************************************************************************
** 函数名称: wind_stack_datalen
** 功能描述: 取得堆栈中数据数
** 输　入: mem:指向堆栈的指针
** 输　出: 消息数
** 全局变量: 无
** 调用模块: wind_disable_interrupt,wind_enable_interrupt()
**
** 作　者: Jason Zhou
** Date: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** Modify:
** Date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


w_uint16_t wind_stack_datalen(void *mem)
{
    w_uint16_t temp;
    temp = 0;                                                   /* 堆栈无效返回0 */
    if (mem != W_NULL)
    {
        wind_disable_interrupt();
        temp = ((w_stack_s* )mem)->item_cnt / ((w_stack_s* )mem)->item_size;
        wind_enable_interrupt();
    }
    return temp;
}


/*********************************************************************************************************
** 函数名称: wind_stack_size
** 功能描述: 取得堆栈总容量
** 输　入: mem:指向堆栈的指针
** 输　出: 堆栈总容量
** 全局变量: 无
** 调用模块: wind_disable_interrupt,wind_enable_interrupt()
**
** 作　者: Jason Zhou
** Date: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** Modify:
** Date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


w_uint16_t wind_stack_size(void *mem)
{
    w_uint16_t temp;
    temp = 0;                                                   /* 堆栈无效返回0 */
    if (mem != W_NULL)
    {
        wind_disable_interrupt();
        temp = ((w_stack_s* )mem)->item_max / ((w_stack_s* )mem)->item_size;
        wind_enable_interrupt();
    }
    return temp;
}



/*********************************************************************************************************
** 函数名称: wind_stack_flush
** 功能描述: 清空堆栈
** 输　入: mem:指向堆栈的指针
** 输　出: 无
** 全局变量: 无
** 调用模块: wind_disable_interrupt,wind_enable_interrupt()
**
** 作　者: Jason Zhou
** Date: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** Modify:
** Date:
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/


void wind_stack_flush(void *mem)
{
    w_stack_s* pstk;
    if (mem != W_NULL)                                                /* 堆栈是否有效 */
    {                                                               /* 有效         */
        pstk = (w_stack_s*)mem;
        wind_disable_interrupt();
        pstk->item_cnt = 0;                                           /* 数据数目为0 */
        pstk->top = pstk->buff + pstk->item_max;               // 计算数据缓冲的结束地址 
        pstk->out = pstk->buff;
        pstk->in = pstk->buff;
        wind_enable_interrupt();
    }
}



#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

