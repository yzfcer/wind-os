/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_queue.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 基本FIFO队列的实现方法
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2014.11.2
** 描　述: 修改了队列的互斥方法，允许系统在操作队列时局部锁定或者不锁定
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_queue.h"
#include "wind_debug.h"
#define MBR_OFFSET(type, mbr) ((w_uint32_t)&(((type*)0)->mbr))

w_err_t wind_queue_create(void *mem,w_uint32_t size,w_uint16_t itemsize)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(mem != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(size > sizeof(w_queue_s),W_ERR_INVALID);
    WIND_ASSERT_RETURN(itemsize > 0,W_ERR_INVALID);

    q = (w_queue_s *)mem;
    q->magic = WIND_QUEUE_MAGIC;
    q->rd = q->buf;
    q->wr = q->buf;
    q->itemsize = itemsize;
    q->count = 0;
    
    q->capacity = (size - MBR_OFFSET(w_queue_s,buf)) / q->itemsize;
    q->end = q->buf + q->capacity *q->itemsize;               
    return W_ERR_OK;
}



w_int32_t wind_queue_read(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    w_queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;
    
    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(buf != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(queue != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len % q->itemsize == 0,W_ERR_INVALID);
    
    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    buff = buf;
    
    lenth = q->count * q->itemsize;
    lenth = lenth > len?len:lenth;
    for(i = 0;i < lenth;i ++)
    {
        buff[i] = q->rd[0]; 
        q->rd++;  
        if(q->rd >= q->end)
            q->rd = q->buf;
    }
    q->count -= lenth / q->itemsize; 
    return lenth;
}




w_int32_t wind_queue_write(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    w_queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;

    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(buf != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(queue != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len % q->itemsize == 0,W_ERR_INVALID);

    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    buff = buf;

    lenth = (q->capacity - q->count) *q->itemsize;
    lenth = lenth > len?len:lenth;
                                                             
    for(i = 0;i < lenth;i ++)
    {
        q->wr[0] = buff[i];                                        
        q->wr++; 
        if(q->wr >= q->end)
            q->wr = q->buf;                                         
    }
    q->count += lenth / q->itemsize;   
    return lenth;
}


w_int32_t wind_queue_data_count(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != W_NULL,W_ERR_PTR_NULL);
    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    return q->count;
}




w_int32_t wind_queue_max_count(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != W_NULL,W_ERR_PTR_NULL);
    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    return q->capacity;
}



w_err_t wind_queue_clean(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != W_NULL,W_ERR_PTR_NULL);

    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    q->rd = q->buf;
    q->wr = q->buf;
    q->count = 0;                                           /* 数据数目为0 */
    return W_ERR_OK;
}

w_err_t wind_queue_destory(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != W_NULL,W_ERR_PTR_NULL);

    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    q->magic = 0;
    return W_ERR_OK;
}




