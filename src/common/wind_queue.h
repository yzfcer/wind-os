/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_queue.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ����FIFO���е�ʵ�ַ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2014.11.2
** �衡��: �޸��˶��еĻ��ⷽ��������ϵͳ�ڲ�������ʱ�ֲ��������߲�����
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __WIND_QUEUE_H_
#define __WIND_QUEUE_H_
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

