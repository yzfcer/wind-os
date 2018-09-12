/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_queue.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: ����FIFO���е�ʵ�ַ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2014.11.2
** �衡��: �޸��˶��еĻ��ⷽ��������ϵͳ�ڲ�������ʱ�ֲ��������߲�����
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
    WIND_ASSERT_RETURN(mem != NULL,W_ERR_NULL);
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
    WIND_ASSERT_RETURN(buf != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(queue != NULL,W_ERR_NULL);
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
    WIND_ASSERT_RETURN(buf != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(queue != NULL,W_ERR_NULL);
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
    WIND_ASSERT_RETURN(queue != NULL,W_ERR_NULL);
    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    return q->count;
}




w_int32_t wind_queue_max_count(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,W_ERR_NULL);
    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    return q->capacity;
}



w_err_t wind_queue_clean(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,W_ERR_NULL);

    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    q->rd = q->buf;
    q->wr = q->buf;
    q->count = 0;                                           /* ������ĿΪ0 */
    return W_ERR_OK;
}

w_err_t wind_queue_destory(void *queue)
{
    w_queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,W_ERR_NULL);

    q = (w_queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,W_ERR_INVALID);
    q->magic = 0;
    return W_ERR_OK;
}




