/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_pipe.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的管道相关的代码头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_PIPE_H__
#define WIND_PIPE_H__

#include "wind_queue.h"
#include "wind_config.h"
#include "wind_types.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_PIPE_SUPPORT > 0

#define WIND_PIPE_MAGIC 0x56384c6a
#define PIPE_NAME_LEN 10

typedef struct __WIND_PIPE
{
    u32_t magic;
    const char *name;//[PIPE_NAME_LEN];
    pthread_s client;
    pthread_s owner;
    bool_t used;
    queue_s *in;
    queue_s *out;
    u32_t inlen;
    u32_t outlen;
}pipe_s,*ppipe_s;

err_t wind_pipe_init(void);
ppipe_s wind_pipe_create(const char *name,void *inbuf,u32_t inlen,void *outbuf,u32_t outlen);
err_t wind_pipe_connect(ppipe_s ppipe);
s16_t wind_pipe_read(ppipe_s ppipe,s8_t *str,s16_t len);
s16_t wind_pipe_write(ppipe_s ppipe,s8_t *str,s16_t len);
err_t wind_pipe_free(ppipe_s ppipe);




#endif //WIND_PIPE_SUPPORT > 0
#ifdef __cplusplus
}
#endif


#endif

