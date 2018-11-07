/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_pipe.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的管道相关的代码,管道是一种数据通信机制，不是线程同步机制
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_pipe.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_queue.h"
#include "wind_string.h"
#include "wind_pool.h"

#if (WIND_PIPE_SUPPORT)
static w_dlist_s pipelist;
static WIND_POOL(pipepool,WIND_PIPE_MAX_NUM,sizeof(w_pipe_s));

static __INLINE__ w_pipe_s *pipe_malloc(void)
{
    return (w_pipe_s*)wind_pool_malloc(pipepool);
}

static __INLINE__ w_err_t pipe_free(void *pipe)
{
    return wind_pool_free(pipepool,pipe);
}

w_err_t _wind_pipe_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(pipelist);
    err = wind_pool_create("pipe",pipepool,sizeof(pipepool),sizeof(w_pipe_s));
    return err;
}

w_pipe_s *wind_pipe_get(const char *name)
{
    w_pipe_s *pipe;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&pipelist)
    {
        pipe = DLIST_OBJ(dnode,w_pipe_s,pipenode);
        if(pipe->name && (wind_strcmp(name,pipe->name) == 0))
        {
            wind_enable_switch();
            return pipe;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_pipe_init(w_pipe_s* pipe,const char *name,void *buff,w_uint32_t buflen)
{
    w_err_t err;
    wind_notice("create pipe:%s",name?name:"null");
    WIND_ASSERT_RETURN((pipe != W_NULL),W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN((buff != W_NULL),W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buflen > 0,W_ERR_INVALID);
    err = wind_queue_create(buff,buflen,1);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    pipe->magic = WIND_PIPE_MAGIC;
    pipe->name = name;
    DNODE_INIT(pipe->pipenode)
    pipe->buff = buff;
    pipe->buflen = buflen;
    pipe->flag_pool = 0;
    wind_disable_interrupt();
    dlist_insert_tail(&pipelist,&pipe->pipenode);
    wind_enable_interrupt();
    return W_ERR_OK;
    
}

w_pipe_s* wind_pipe_create(const char *name,void *buff,w_uint32_t buflen)
{
    w_pipe_s* pipe;
    w_err_t err;
    
    
    pipe = pipe_malloc();
    WIND_ASSERT_RETURN(pipe != W_NULL,W_NULL);
    err = wind_pipe_init(pipe,name,buff,buflen);
    if(err == W_ERR_OK)
    {
        pipe->flag_pool = 1;
        return pipe;
    }
    pipe_free(pipe);
    return W_NULL;
}


w_int32_t wind_pipe_read(w_pipe_s* pipe,w_int8_t *str,w_int16_t len)
{
    w_int32_t count = -1;
    WIND_ASSERT_RETURN(pipe != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(str != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(pipe->magic == WIND_PIPE_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    count = wind_queue_read(pipe->buff,str,len);
    wind_enable_interrupt();
    return count;
}

w_int32_t wind_pipe_write(w_pipe_s* pipe,w_int8_t *str,w_int16_t len)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(pipe != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pipe->magic == WIND_PIPE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(str != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len > 0,W_ERR_INVALID);
    wind_disable_interrupt();
    cnt = wind_queue_write(pipe->buff,str,len);
    wind_enable_interrupt();
    return cnt;
}

w_err_t wind_pipe_destroy(w_pipe_s* pipe)
{
    WIND_ASSERT_RETURN(pipe != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pipe->magic == WIND_PIPE_MAGIC,W_ERR_INVALID);
    wind_notice("destroy pipe:%s",pipe->name?pipe->name:"null");
    wind_disable_interrupt();
    dlist_remove(&pipelist,&pipe->pipenode);
    pipe->magic = 0;
    pipe->name = W_NULL;
    if(pipe->flag_pool)
        pipe_free(pipe);
    wind_enable_interrupt();
    return W_ERR_OK;
}

w_err_t _wind_pipe_print(void)
{
    w_dnode_s *dnode;
    w_pipe_s *pipe;
    w_queue_s *queue;
    w_int32_t size,used;
    w_dlist_s* list = &pipelist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\npipe list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-10s\r\n","pipe","lenth","used");
    wind_print_space(5);
    foreach_node(dnode,list)
    {
        pipe = DLIST_OBJ(dnode,w_pipe_s,pipenode);
        queue = (w_queue_s *)pipe->buff;
        size = wind_queue_max_count(queue);
        used = wind_queue_data_count(queue);
        wind_printf("%-16s %-8d %-10d\r\n",
            pipe->name?pipe->name:"null",size,used);
    }
    wind_print_space(5);
    return W_ERR_OK;
}
#endif //WIND_PIPE_SUPPORT


//**********************************************extern functions******************************

