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
#include "wind_pipe.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_core.h"
#include "wind_queue.h"
#include "wind_string.h"
#include "wind_pool.h"

#if (WIND_PIPE_SUPPORT)

static WIND_POOL(pipepool,WIND_PIPE_MAX_NUM,sizeof(pipe_s));
//********************************************internal functions******************************

static __INLINE__ pipe_s *pipe_malloc(void)
{
    return (pipe_s*)wind_pool_malloc(pipepool);
}

static __INLINE__ w_err_t pipe_free(void *pipe)
{
    return wind_pool_free(pipepool,pipe);
}



//********************************************internal functions******************************




//**********************************************extern functions******************************
w_err_t _wind_pipe_init(void)
{
    w_err_t err;
    err = wind_pool_create("pipe",pipepool,sizeof(pipepool),sizeof(pipe_s));
    return err;
}

pipe_s *wind_pipe_get(const char *name)
{
    pipe_s *pipe;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.pipelist)
    {
        pipe = DLIST_OBJ(dnode,pipe_s,pipenode);
        if(wind_strcmp(name,pipe->name) == 0)
        {
            wind_enable_switch();
            return pipe;
        }
    }
    wind_enable_switch();
    return NULL;
}


pipe_s* wind_pipe_create(const char *name,void *buff,w_uint32_t buflen)
{
    pipe_s* pipe;
    w_err_t err;
    
    wind_notice("create pipe:%s",name);
    WIND_ASSERT_RETURN((buff != NULL),NULL);
    WIND_ASSERT_RETURN(buflen > 0,NULL);
    err = wind_queue_create(buff,buflen,1);
    WIND_ASSERT_RETURN(err == ERR_OK,NULL);
    
    pipe = pipe_malloc();
    WIND_ASSERT_RETURN(pipe != NULL,NULL);
    pipe->magic = WIND_PIPE_MAGIC;
    pipe->name = name;
    DNODE_INIT(pipe->pipenode)
    pipe->used = B_TRUE;
    pipe->buff = buff;
    pipe->buflen = buflen;
    wind_disable_interrupt();
    dlist_insert_tail(&g_core.pipelist,&pipe->pipenode);
    wind_enable_interrupt();
    return pipe;
}


w_int32_t wind_pipe_read(pipe_s* pipe,w_int8_t *str,w_int16_t len)
{
    w_int32_t count = -1;
    WIND_ASSERT_RETURN(pipe != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(str != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(pipe->magic == WIND_PIPE_MAGIC,ERR_INVALID_PARAM);
    wind_disable_interrupt();
    count = wind_queue_read(pipe->buff,str,len);
    wind_enable_interrupt();
    return count;
}

w_int32_t wind_pipe_write(pipe_s* pipe,w_int8_t *str,w_int16_t len)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(pipe != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pipe->magic == WIND_PIPE_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(str != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(len > 0,ERR_INVALID_PARAM);
    wind_disable_interrupt();
    cnt = wind_queue_write(pipe->buff,str,len);
    wind_enable_interrupt();
    return cnt;
}

w_err_t wind_pipe_destroy(pipe_s* pipe)
{
    WIND_ASSERT_RETURN(pipe != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pipe->magic == WIND_PIPE_MAGIC,ERR_INVALID_PARAM);
    wind_notice("destroy pipe:%s",pipe->name);
    wind_disable_interrupt();
    dlist_remove(&g_core.pipelist,&pipe->pipenode);
    pipe->magic = 0;
    pipe->used = B_FALSE;
    pipe->name = NULL;
    pipe_free(pipe);
    wind_enable_interrupt();
    return ERR_OK;
}

w_err_t _wind_pipe_print(dlist_s* list)
{
    dnode_s *dnode;
    pipe_s *pipe;
    queue_s *queue;
    w_int32_t size,used;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\npipe list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-10s\r\n","pipe","lenth","used");
    wind_print_space(5);
    foreach_node(dnode,list)
    {
        pipe = DLIST_OBJ(dnode,pipe_s,pipenode);
        queue = (queue_s *)pipe->buff;
        size = wind_queue_max_count(queue);
        used = wind_queue_data_count(queue);
        wind_printf("%-16s %-8d %-10d\r\n",
            pipe->name,size,used);
    }
    wind_print_space(5);
    return ERR_OK;
}
#endif //WIND_PIPE_SUPPORT


//**********************************************extern functions******************************

