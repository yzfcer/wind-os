/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_queue.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os����Ϣ��صĴ���
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
    queue_s *q;
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > sizeof(queue_s),ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(itemsize > 0,ERR_INVALID_PARAM);

    q = (queue_s *)mem;
    q->magic = WIND_QUEUE_MAGIC;
    q->rd = q->buf;
    q->wr = q->buf;
    q->itemsize = itemsize;
    q->count = 0;
    
    q->capacity = (size - MBR_OFFSET(queue_s,buf)) / q->itemsize;
    q->end = q->buf + q->capacity *q->itemsize;               
    return ERR_OK;
}



w_int32_t wind_queue_read(void *queue,void *buf,w_uint32_t len)
{
    w_uint32_t i;
    queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;
    
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len % q->itemsize == 0,ERR_INVALID_PARAM);
    
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
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
    queue_s *q;
    w_uint8_t *buff;
    w_uint32_t lenth;

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len % q->itemsize == 0,ERR_INVALID_PARAM);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
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
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    return q->count;
}




w_int32_t wind_queue_max_count(void *queue)
{
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);
    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    return q->capacity;
}



w_err_t wind_queue_clean(void *queue)
{
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    q->rd = q->buf;
    q->wr = q->buf;
    q->count = 0;                                           /* ������ĿΪ0 */
    return ERR_OK;
}

w_err_t wind_queue_destory(void *queue)
{
    queue_s *q;
    WIND_ASSERT_RETURN(queue != NULL,ERR_NULL_POINTER);

    q = (queue_s *)queue;
    WIND_ASSERT_RETURN(q->magic == WIND_QUEUE_MAGIC,ERR_INVALID_PARAM);
    q->magic = 0;
    return ERR_OK;
}




