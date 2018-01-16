/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_pipe.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的管道相关的代码,管道是一种数据通信机制，不是线程同步机制
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_pipe.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_queue.h"
#if (WIND_PIPE_SUPPORT)

static WIND_MPOOL(pipepool,WIND_PIPE_MAX_NUM,sizeof(pipe_s));
//********************************************internal functions******************************

static __INLINE__ pipe_s *pipe_malloc(void)
{
    return (pipe_s*)wind_pool_alloc(pipepool);
}

static __INLINE__ w_err_t pipe_free(void *pipe)
{
    return wind_pool_free(pipepool,pipe);
}



//********************************************internal functions******************************




//**********************************************extern functions******************************
w_err_t wind_pipe_init(void)
{
    w_err_t err;
    err = wind_pool_create("pipe",pipepool,sizeof(pipepool),sizeof(pipe_s));
    return err;
}

pipe_s* wind_pipe_create(const char *name,void *buff,w_uint32_t buflen)
{
    pipe_s* ppipe;
    w_err_t err;
    WIND_ASSERT_RETURN((buff != NULL),NULL);
    WIND_ASSERT_RETURN(buflen > 0,NULL);
    err = wind_queue_create(buff,buflen,1);
    WIND_ASSERT_RETURN(err == ERR_OK,NULL);
    
    ppipe = pipe_malloc();
    WIND_ASSERT_RETURN(ppipe != NULL,NULL);
    ppipe->magic = WIND_PIPE_MAGIC;
    ppipe->name = name;
    DNODE_INIT(ppipe->pipenode)
    ppipe->used = B_TRUE;
    ppipe->buff = buff;
    ppipe->buflen = buflen;
    wind_close_interrupt();
    dlist_insert_tail(&g_core.pipelist,&ppipe->pipenode);
    wind_open_interrupt();
    return ppipe;
}


w_int32_t wind_pipe_read(pipe_s* ppipe,w_int8_t *str,w_int16_t len)
{
    w_int32_t count = -1;
    WIND_ASSERT_RETURN(ppipe != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(str != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(ppipe->magic == WIND_PIPE_MAGIC,ERR_INVALID_PARAM);
    wind_close_interrupt();
    count = wind_queue_read(ppipe->buff,str,len);
    wind_open_interrupt();
    return count;
}

w_int32_t wind_pipe_write(pipe_s* ppipe,w_int8_t *str,w_int16_t len)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(ppipe != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(ppipe->magic == WIND_PIPE_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(str != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len > 0,ERR_INVALID_PARAM);
    wind_close_interrupt();
    cnt = wind_queue_write(ppipe->buff,str,len);
    wind_open_interrupt();
    return cnt;
}

w_err_t wind_pipe_free(pipe_s* ppipe)
{
    WIND_ASSERT_RETURN(ppipe != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(ppipe->magic == WIND_PIPE_MAGIC,ERR_INVALID_PARAM);
    wind_close_interrupt();
    dlist_remove(&g_core.pipelist,&ppipe->pipenode);
    ppipe->magic = 0;
    ppipe->used = B_FALSE;
    ppipe->name = NULL;
    pipe_free(ppipe);
    wind_open_interrupt();
    return ERR_OK;
}
#endif //WIND_PIPE_SUPPORT


//**********************************************extern functions******************************

