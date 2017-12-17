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

#include "wind_type.h"
#include "wind_queue.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
static w_err_t wind_queue_create_lock(queue_s *q)
{
    return ERR_OK;
}
static void wind_queue_free_lock(queue_s *q)
{
    wind_error("no lock for queue");
}

static void wind_queue_lock(queue_s *q)
{
    wind_error("no lock for queue");
}

static void wind_queue_unlock(queue_s *q)
{
    wind_error("no lock for queue");
}


w_err_t wind_queue_create(void *mem,
                          w_uint32_t size,
                          w_uint16_t itemsize
                          )
{
    w_err_t err;
    queue_s *q;
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > sizeof(queue_s),ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(itemsize > 0,ERR_INVALID_PARAM);

    q = (queue_s *)mem;
    err = wind_queue_create_lock(q);
    WIND_ASSERT_RETURN(err == ERR_OK,ERR_NULL_POINTER);
    q->rd = q->buf;
    q->wr = q->buf;
    q->itemsize = itemsize;
    q->count = 0;
    
    // 计算队列可以存储的数据数目 
    q->capacity = (size - (w_uint32_t)(((queue_s *)0)->buf)) / q->itemsize;
    
    // 计算数据缓冲的结束地址
    q->end = q->buf + q->capacity *q->itemsize;               
    q->magic = WIND_QUEUE_MAGIC;
    return ERR_OK;
}



w_int32_t wind_queue_read(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;
    
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len % q->itemsize == 0,ERR_INVALID_PARAM);
    
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    buff = buf;
    
    wind_queue_lock(q);
    lenth = q->count *q->itemsize;
    lenth = lenth > len?len:lenth;
    for(i = 0;i < lenth;i ++)
    {
        buff[i] = q->rd[0]; /* 数据出队     */
        q->rd++;  /* 调整出队指针 */
        if(q->rd >= q->end)
            q->rd = 0;
    }
    q->count -= lenth / q->itemsize;                                     /* 数据减少      */
    wind_queue_unlock(q);
    return lenth;
}




w_int32_t wind_queue_write(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;
    
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len % q->itemsize == 0,ERR_INVALID_PARAM);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    buff = buf;

    wind_queue_lock(q);
    lenth = (q->capacity - q->count) *q->itemsize;
    lenth = lenth > len?len:lenth;
                                                             
    for(i = 0;i < q->itemsize;i ++)
    {
        q->wr[0] = buff[i];                                        
        q->wr++; /* 调整入队指针*/
        if(q->wr >= q->end)
            q->wr = 0;                                         
    }
    q->count += lenth / q->itemsize;   
    wind_queue_unlock(q);

    return lenth;
}


w_int32_t wind_queue_datalen(void *queue)
{
    w_int32_t temp;
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




w_int32_t wind_queue_capacity(void *queue)
{
    w_int32_t temp;
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
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    wind_queue_lock(q);
    q->magic = 0;
    wind_queue_unlock(q);
    wind_queue_free_lock(q);
    return ERR_OK;
}




