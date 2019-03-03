/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_pipe.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�Ĺܵ���صĴ���,�ܵ���һ������ͨ�Ż��ƣ������߳�ͬ������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_PIPE_H__
#define WIND_PIPE_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (WIND_PIPE_SUPPORT)

#define WIND_PIPE_MAGIC 0x56384c6a
#define PIPE_NAME_LEN 12

#define F_PIPE_POOL (0x01 << 0) //���pipe�����Ƿ�ͨ���ڴ�ط���
#define IS_F_PIPE_POOL(pipe) ((pipe->obj.flag & F_PIPE_POOL) == F_PIPE_POOL)
#define SET_F_PIPE_POOL(pipe) (pipe->obj.flag |= F_PIPE_POOL)
#define CLR_F_PIPE_POOL(pipe) (pipe->obj.flag &= (~F_PIPE_POOL))

typedef struct __w_pipe_s
{
    w_obj_s obj;
    void *buff;
    w_uint16_t buflen;
}w_pipe_s;

w_err_t _wind_pipe_mod_init(void);
w_pipe_s* wind_pipe_get(const char *name);
w_err_t wind_pipe_init(w_pipe_s* pipe,const char *name,void *buff,w_uint32_t buflen);
w_pipe_s* wind_pipe_create(const char *name,void *buff,w_uint32_t buflen);
w_int32_t wind_pipe_read(w_pipe_s* pipe,w_int8_t *str,w_int16_t len);
w_int32_t wind_pipe_write(w_pipe_s* pipe,w_int8_t *str,w_int16_t len);
w_err_t wind_pipe_destroy(w_pipe_s* pipe);
w_err_t wind_pipe_print(void);


#endif //WIND_PIPE_SUPPORT
#ifdef __cplusplus
}
#endif


#endif

