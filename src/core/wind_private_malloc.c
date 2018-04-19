/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**文   件   名: wind_private_malloc.h / wind_private_malloc.c
**创   建   人: 周江村
**最后修改日期: 2014/10/24 0:00:00
**描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2014/10/24 0:00:00
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 线程的私有栈的定义和相关的操作
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/


/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_heap.h"
#include "wind_thread.h"
#include "wind_private_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (WIND_HEAP_SUPPORT  && WIND_PRIVATE_HEAP_SUPPORT)

/********************************************内部变量定义**********************************************/



/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
w_err_t wind_privateheap_create(w_uint32_t size)
{
    w_err_t err;
    w_addr_t start;
    thread_s *thread;
    heap_s* mhp;
    thread = wind_thread_current();
    WIND_ASSERT_RETURN((thread != NULL),ERR_NULL_POINTER);
    mhp = (heap_s*)wind_malloc(size);
    WIND_ASSERT_RETURN(mhp != NULL,ERR_NULL_POINTER);
    //mhp = (heap_s*)
    thread->private_heap = mhp;
    start = (w_addr_t)((w_addr_t)mhp + sizeof(heap_s));
    err = wind_heap_create("pri_heap",start,size);
    if(err != ERR_OK)
    {
        wind_free(mhp);
        thread->private_heap = NULL;
        return ERR_COMMAN;
    }
    return ERR_OK;
}

void *wind_privateheap_alloc(w_uint32_t size)
{
    void *p;
    w_err_t err;
    thread_s *thread;
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(thread != NULL,NULL);
    WIND_ASSERT_RETURN(thread->private_heap != NULL,NULL);
    if(thread->private_heap == NULL)
    {
        err = wind_privateheap_create(PRIVATE_HEAP_SIZE);
        WIND_ASSERT_RETURN(err == ERR_OK,NULL);
    }
    p = wind_heap_malloc(thread->private_heap,size);
    return p;
}

w_err_t wind_privateheap_free(void *ptr)
{
    w_err_t err;
    WIND_ASSERT_RETURN(ptr != NULL,ERR_NULL_POINTER);
    err = wind_free((void *) ptr);
    return err;    
}


#endif
#ifdef __cplusplus
}
#endif

