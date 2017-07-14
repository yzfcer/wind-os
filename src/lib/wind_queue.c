/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_queue.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的消息相关的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2014.11.2
** 描　述: 修改了队列的互斥方法，允许系统在操作队列时局部锁定或者不锁定
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_types.h"
#include "wind_queue.h"
#include "wind_os_hwif.h"
#include "wind_assert.h"
#include "wind_err.h"

//#define wind_queue_lock(lock_type) if(lock_type == LOCK_TYPE_GLOBAL) \wind_close_interrupt();
//#define wind_queue_unlock(lock_type) wind_open_interrupt()

static w_uint32_t wind_queue_lock(queue_s *q)
{
    if(q->lock_type == LOCK_TYPE_NONE)
        return 0;
    else if(q->lock_type == LOCK_TYPE_GLOBAL) 
        wind_close_interrupt();
#if WIND_LOCK_SUPPORT > 0
    else if(q->lock_type == LOCK_TYPE_AREA)
        wind_lock_close(q->lock);
#endif
    return 0;
}

static void wind_queue_unlock(queue_s *q)
{
    if(q->lock_type == LOCK_TYPE_NONE)
        return;
    else if(q->lock_type == LOCK_TYPE_GLOBAL) 
        wind_open_interrupt();
#if WIND_LOCK_SUPPORT > 0
    else if(q->lock_type == LOCK_TYPE_AREA)
        wind_lock_open(q->lock);
#endif
}

/*********************************************************************************************************
** 函数名称: wind_queue_create
** 功能描述: 初始化数据队列
** 输　入: buf      ：为队列分配的存储空间地址
**         lenth：为队列分配的存储空间大小（字节）
**         DataWid  :缓冲区数据宽度
**         ReadEmpty：为队列读空时处理程序
**         WriteFull：为队列写满时处理程序
** 输　出: QUEUE_ERR:参数错误
**         ERR_OK:成功
** 全局变量: 无
** 调用模块: wind_queue_lock(),wind_queue_unlock()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

w_err_t wind_queue_create(void *mem,
                          w_uint32_t size,
                          w_uint16_t data_wid,
                          lock_type_e lock_type
                          )
{
    queue_s *q;
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > sizeof(queue_s),ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(data_wid > 0,ERR_INVALID_PARAM);

    q = (queue_s *)mem;
    
    //在需要使用局部锁时，如果互斥锁不支持，则使用全局锁
    if(lock_type == LOCK_TYPE_GLOBAL)
    {
#if WIND_LOCK_SUPPORT > 0
        q->lock_type = (w_int32_t)lock_type;
        q->lock = wind_lock_create("queue");
        WIND_ASSERT_RETURN(q->lock != NULL,ERR_NULL_POINTER);
#else
        q->lock_type = LOCK_TYPE_GLOBAL;
#endif
    }

    q->rd = q->buf;
    q->wr = q->buf;
    q->data_wid = data_wid;
    q->count = 0;
    
    // 计算队列可以存储的数据数目 
    q->capacity = (size - (w_uint32_t)(((queue_s *)0)->buf)) / q->data_wid;
    
    // 计算数据缓冲的结束地址
    q->end = q->buf + q->capacity * q->data_wid;               
    q->magic = WIND_QUEUE_MAGIC;
    return ERR_OK;
}


/*********************************************************************************************************
** 函数名称: wind_queue_read
** 功能描述: 获取队列中的数据
** 输　入: ret:存储返回的消息的地址
**         buf:指向队列的指针
** 输　出: 实际读取的数据长度，以字节长度为单位
**                      读取出错时返回小于0的整数
**         
** 全局变量: 无
** 调用模块: wind_queue_lock(),wind_queue_unlock()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
w_int32_t wind_queue_read(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;
    
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len % q->data_wid == 0,ERR_INVALID_PARAM);
    
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    buff = buf;
    
    wind_queue_lock(q);
    lenth = q->count * q->data_wid;
    lenth = lenth > len?len:lenth;
    for(i = 0;i < lenth;i ++)
    {
        buff[i] = q->rd[0]; /* 数据出队     */
        q->rd++;  /* 调整出队指针 */
        if(q->rd >= q->end)
            q->rd = 0;
    }
    q->count -= lenth / q->data_wid;                                     /* 数据减少      */
    wind_queue_unlock(q);
    return lenth;
}



/*********************************************************************************************************
** 函数名称: wind_queue_write
** 功能描述: FIFO方式发送数据
** 输　入: buf :指向队列的指针
**         Data:消息数据
** 输　出: QUEUE_ERR   :参数错误
**         QUEUE_FULL:队列满
**         ERR_OK  :发送成功
** 全局变量: 无
** 调用模块: wind_queue_lock(),wind_queue_unlock()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


w_int32_t wind_queue_write(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;
    
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len % q->data_wid == 0,ERR_INVALID_PARAM);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    buff = buf;

    wind_queue_lock(q);
    lenth = (q->capacity - q->count) * q->data_wid;
    lenth = lenth > len?len:lenth;
                                                             
    for(i = 0;i < q->data_wid;i ++)
    {
        q->wr[0] = buff[i];                                        
        q->wr++; /* 调整入队指针*/
        if(q->wr >= q->end)
            q->wr = 0;                                         
    }
    q->count += lenth / q->data_wid;   
    wind_queue_unlock(q);

    return lenth;
}




/*********************************************************************************************************
** 函数名称: wind_queue_datalen
** 功能描述: 取得队列中数据数
** 输　入: buf:指向队列的指针
** 输　出: 消息数
** 全局变量: 无
** 调用模块: wind_queue_lock(),wind_queue_unlock()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_NDATA
#define EN_QUEUE_NDATA    0
#endif

#if EN_QUEUE_NDATA > 0

w_uint32_t wind_queue_datalen(void *queue)
{
    w_uint32_t temp;
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    temp = 0;                                                   
    wind_queue_lock(q);
    temp = q->count;
    wind_queue_unlock(q);
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: wind_queue_capacity
** 功能描述: 取得队列总容量
** 输　入: buf:指向队列的指针
** 输　出: 队列总容量
** 全局变量: 无
** 调用模块: wind_queue_lock(),wind_queue_unlock()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_SIZE
#define EN_QUEUE_SIZE    0
#endif

#if EN_QUEUE_SIZE 

w_uint32_t wind_queue_capacity(void *queue)
{
    w_uint16_t temp;
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    temp = 0;                                                   /* 队列无效返回0 */
    wind_queue_lock(q);
    temp = ((queue_s *)queue)->count;
    wind_queue_unlock(q);
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: wind_queue_flush
** 功能描述: 清空队列
** 输　入: buf:指向队列的指针
** 输　出: 无
** 全局变量: 无
** 调用模块: wind_queue_lock(),wind_queue_unlock()
**
** 作　者: 周江村
** 日　期: 2012.09.26
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

w_err_t wind_queue_flush(void *queue)
{
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    wind_queue_lock(q);
    q->rd = q->buf;
    q->wr = q->buf;
    q->count = 0;                                           /* 数据数目为0 */
    wind_queue_unlock(q);
    return ERR_OK;
}

w_err_t wind_queue_destory(void *queue)
{
    queue_s *q;
    w_err_t err;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    wind_queue_lock(q);
    q->magic = 0;
    wind_queue_unlock(q);
    if(q->lock_type == LOCK_TYPE_GLOBAL)
    {
#if WIND_LOCK_SUPPORT > 0
        err = wind_lock_free(q->lock);
        WIND_ASSERT_RETURN(err == ERR_OK,err);
#endif
    }
    return ERR_OK;
}



/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

