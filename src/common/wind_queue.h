/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_queue.h
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
#ifndef WIND_QUEUE_H_
#define WIND_QUEUE_H_
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#define WIND_QUEUE_MAGIC 0x25a5c629

typedef struct __queue_s
{
    w_uint32_t magic;
    w_uint8_t *rd;// Point to data read location         
    w_uint8_t *wr; // Point to data write location      
    w_uint8_t *end;// Point to the end of buf    
    w_uint32_t count;// Number of existing data in the queue          
    w_uint32_t capacity;// Number of elements allowed to be stored in the queue
    w_uint32_t itemsize; // Data width of element
    w_uint8_t buf[4]; // Space for data storage,for station
}w_queue_s;



w_err_t wind_queue_create(void *mem,w_uint32_t size,w_uint16_t itemsize);

w_int32_t wind_queue_read(void *queue,void *buf,w_uint32_t len);

w_int32_t wind_queue_write(void *queue,void *buf,w_uint32_t len);

w_int32_t wind_queue_data_count(void *queue);

w_int32_t wind_queue_max_count(void *queue);

w_err_t wind_queue_clean(void *queue);

w_err_t wind_queue_destory(void *queue);

#ifdef __cplusplus
}
#endif//#ifdef __cplusplus
#endif//WIND_QUEUE_H_

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

