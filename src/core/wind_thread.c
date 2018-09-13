/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_thread.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 线程相关的代码
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
#include "wind_core.h"
#include "wind_thread.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_pool.h"
#include "wind_var.h"
#include "wind_heap.h"
#include "wind_os_hwif.h"
//用来表示

#define WIND_THREAD_PRIO_MIN_LIM 100//优先级的最小值
#define WIND_THREAD_PRIO_MAX_LIM 30000//优先级的最大值

static w_uint16_t get_prio(w_prio_e priolevel)
{
    static w_uint16_t threadcnt = 0;
    w_uint16_t prio;
    threadcnt ++;
    if(threadcnt >= 10000)
        threadcnt = 0;
    prio = (priolevel - 1) *10000 + threadcnt;
    return prio;
}

//********************************************internal functions******************************
static WIND_POOL(threadpool,WIND_THREAD_MAX_NUM,sizeof(w_thread_s));
#if WIND_STKPOOL_SUPPORT
WIND_POOL(stkbufpool,WIND_STK_MAX_NUM,WIND_STK_SIZE *sizeof(w_stack_t));
#endif

static __INLINE__ w_thread_s *thread_malloc(void)
{
    return (w_thread_s*)wind_pool_malloc(threadpool);
}

static __INLINE__ w_err_t thread_free(w_thread_s *thread)
{
    return wind_pool_free(threadpool,thread);
}


static void thread_entry(void *args)
{
    w_err_t err;
    w_thread_s *thread;
    thread = wind_thread_current();
    wind_notice("run thread:%s.",thread->name);
    if(thread != W_NULL)
    {
        err = thread->thread_func(thread->argc,thread->argv);
        wind_thread_exit(err);
    }
}

static char *wind_thread_status(w_thread_stat_e stat)
{
    switch(stat)
    {
        case THREAD_STATUS_READY:
            return "ready";
        case THREAD_STATUS_SLEEP:
            return "sleep";
        case THREAD_STATUS_SUSPEND:
            return "suspend";
        case THREAD_STATUS_DEAD:
            return "dead";
        default:
            return "unkown";
    }
}

//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t _wind_thread_mod_init(void)
{
    w_err_t err;
#if WIND_STKPOOL_SUPPORT
    err = wind_pool_create("stkbuf",stkbufpool,sizeof(stkbufpool),WIND_STK_SIZE *sizeof(w_stack_t));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
#endif    
    err = wind_pool_create("thread",threadpool,sizeof(threadpool),sizeof(w_thread_s));
    return err;
}

w_thread_s *wind_thread_get(const char *name)
{
    w_thread_s *thread;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.threadlist)
    {
        thread = DLIST_OBJ(dnode,w_thread_s,validnode);
        if(wind_strcmp(name,thread->name) == 0)
        {
            wind_enable_switch();
            return thread;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_thread_s *wind_thread_current(void)
{
    return THREAD_FROM_MEMBER(gwind_cur_stack,w_thread_s,stack);
}

char *wind_thread_curname(void)
{
    w_thread_s *thread;
    thread = wind_thread_current();
    return thread->name;
}

//创建一个线程
w_thread_s *wind_thread_create(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                    w_int16_t argc,
                    w_int8_t **argv,
                    w_prio_e priolevel,
                    w_pstack_t pstk,
                    w_uint16_t stksize)
{
    w_uint16_t i;
    w_thread_s *thread;
    w_pstack_t tmpstk;

    wind_notice("create thread:%s",name);
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(thread_func != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(pstk != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(stksize > 0,W_NULL);
    WIND_ASSERT_RETURN(priolevel < PRIO_SYS_LOW && priolevel > PRIO_ZERO,W_NULL);
    thread = thread_malloc();
    WIND_ASSERT_RETURN(thread != W_NULL,W_NULL);
    thread->magic = WIND_THREAD_MAGIC;
    PRIO_DNODE_INIT(thread->validnode);
    PRIO_DNODE_INIT(thread->suspendnode);
    PRIO_DNODE_INIT(thread->sleepnode);
    thread->stack = pstk;
    thread->stack_top = pstk;
    thread->stksize = stksize;
    for(i = 0;i < stksize;i ++)
        thread->stack[i] = 0;
    thread->argc = argc;
    thread->argv = argv;
    thread->thread_func = thread_func;
    thread->stkpool_flag = 0;
    
    //wind_strcpy(thread->name,name);
    thread->name = (char*)name;
    thread->prio = get_prio(priolevel);
    
    tmpstk = _wind_thread_stack_init(thread_entry,0,pstk + stksize -1);
    thread->stack = tmpstk;
    thread->run_times = 0;
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COMMON;
    thread->sleep_ticks = 0;
#if WIND_THREAD_CALLBACK_SUPPORT
    wind_memset(&thread->cb,0,sizeof(thread->cb));
#endif
    wind_disable_interrupt();
    dlist_insert_prio(&g_core.threadlist,&thread->validnode,thread->prio);
    wind_enable_interrupt();
    //wind_thread_print(&g_core.threadlist);
    return thread;
}

#if WIND_STKPOOL_SUPPORT
w_thread_s *wind_thread_create_default(const w_int8_t *name,
                    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                    w_int16_t argc,
                    w_int8_t **argv)
{
    w_prio_e priol;
    w_pstack_t pstk;
    int stksize;
    w_thread_s *thread;
    priol = PRIO_MID;
    stksize = WIND_STK_SIZE;
    pstk = wind_pool_malloc(stkbufpool);
    WIND_ASSERT_RETURN(pstk != W_NULL,W_NULL);
    thread = wind_thread_create(name,thread_func,argc,argv,priol,pstk,stksize);
    WIND_ASSERT_RETURN(thread != W_NULL,W_NULL);
    thread->stkpool_flag = 1;
    return thread;
}
#endif

w_err_t wind_thread_destroy(w_thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    wind_notice("distroy thread:%s",thread->name);
#if WIND_THREAD_CALLBACK_SUPPORT
        if(thread->cb.dead != W_NULL)
            thread->cb.dead(thread);
#endif
    wind_disable_interrupt();
    dlist_remove(&g_core.threadlist,&thread->validnode.dnode);
    //这里需要先释放一些与这个线程相关的一些东西后才能释放这个thread
    if(thread->stkpool_flag)
        wind_pool_free(stkbufpool,thread->stack_top);
    
    thread_free(thread);
    wind_enable_interrupt();
    _wind_thread_dispatch();
    return W_ERR_OK;
}

w_err_t wind_thread_set_priority(w_thread_s *thread,w_int16_t prio)
{
    w_int16_t minlim = 0,maxlim = 32767;
    extern w_bool_t  wind_thread_isopen(void);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN((prio >= minlim) && (prio <= maxlim),W_ERR_OVERFLOW);
    //如何防止用户强制修改为禁用的优先级?
    if(wind_thread_isopen())
    {
        minlim = WIND_THREAD_PRIO_MIN_LIM;
        maxlim = WIND_THREAD_PRIO_MAX_LIM;
    }

    wind_debug("change prio %s:%d\r\n",thread->name,prio);
    wind_disable_interrupt();
    dlist_remove(&g_core.threadlist,&thread->validnode.dnode);
    thread->prio = prio;
    dlist_insert_prio(&g_core.threadlist,&thread->validnode,prio);
    wind_enable_interrupt();
    return W_ERR_OK;
}



w_err_t wind_thread_start(w_thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();   
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.start != W_NULL)
        thread->cb.start(thread);
#endif
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COMMON;
    wind_enable_interrupt();

    return W_ERR_OK;
}


w_err_t wind_thread_suspend(w_thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.suspend != W_NULL)
        thread->cb.suspend(thread);
#endif
    thread->runstat = THREAD_STATUS_SUSPEND;
    thread->cause = CAUSE_COMMON;
    wind_enable_interrupt();
    return W_ERR_OK;
}



w_err_t wind_thread_resume(w_thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.resume != W_NULL)
        thread->cb.resume(thread);
#endif
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COMMON;
    wind_enable_interrupt();
    return W_ERR_OK;
}


//退出线程，在对应的线程中调用
w_err_t wind_thread_exit(w_err_t exitcode)
{
    w_thread_s *thread;
    thread = wind_thread_current();
    wind_notice("exit thread:%s,exitcode:%d",thread->name,exitcode);
    wind_thread_destroy(thread);
    _wind_thread_dispatch();
    return W_ERR_OK;
}


//睡眠一段时间,不能在中断中调用这个函数
w_err_t wind_thread_sleep(w_uint32_t ms)
{
    w_uint16_t stcnt;
    w_thread_s *thread = W_NULL;
    stcnt = ms *WIND_TICK_PER_SEC / 1000;
    if(0 == stcnt)
        stcnt = 1;
    wind_disable_interrupt();
    thread = wind_thread_current();
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_SLEEP;
    thread->sleep_ticks = stcnt;
    dlist_insert_prio(&g_core.sleeplist,&thread->sleepnode,thread->prio);

    wind_enable_interrupt();
#if 0
    foreach_node(dnode,&g_core.sleeplist)
    {
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,sleepnode);
        if(thread->prio < 0)
        {
            wind_thread_print(&g_core.sleeplist);
            wind_error("sleep err");
            break;
        }
    }
#endif
    _wind_thread_dispatch();
    return W_ERR_OK;
}

w_err_t _wind_thread_wakeup(void)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    wind_disable_interrupt();
    WIND_ASSERT_TODO(RUN_FLAG,wind_enable_interrupt(),W_ERR_OK);
    foreach_node(dnode,&g_core.sleeplist)
    {
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,sleepnode);
        if(thread->sleep_ticks > 0)
            thread->sleep_ticks --;
        if(thread->sleep_ticks <= 0)
        {
            if(thread->runstat == THREAD_STATUS_SLEEP)
            {
                thread->runstat = THREAD_STATUS_READY;
                thread->cause = CAUSE_SLEEP;
                dlist_remove(&g_core.sleeplist,&thread->sleepnode.dnode);
            }
        }
    }
    wind_enable_interrupt();
    return W_ERR_OK;
}

#if WIND_THREAD_CALLBACK_SUPPORT
w_err_t wind_thread_callback_register(w_thread_s *thread,w_thr_evt_e id,void(*cb)(w_thread_s *))
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);

    switch(id)
    {
    case THR_EVT_START:
        thread->cb.start = cb;
        break;
    case THR_EVT_SUSPEND:
        thread->cb.suspend = cb;
        break;
    case THR_EVT_RESUME:
        thread->cb.resume = cb;
        break;
    case THR_EVT_DEAD:
        thread->cb.dead = cb;
        break;
    default:
        break;
    }
    return W_ERR_OK;

}
#endif


//调试时用到的函数，打印当前的系统中的线程的信息
w_err_t wind_thread_print(w_dlist_s *list)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    char *stat;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nthread list as following:\r\n");
    wind_print_space(7);
    wind_printf("%-16s %-8s %-10s %-10s %-10s\r\n","thread","prio","state","stacksize","runtimes");
    wind_print_space(7);
    foreach_node(dnode,list)
    {
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,validnode);
        stat = wind_thread_status(thread->runstat);
        wind_printf("%-16s %-8d %-10s %-10d %-10d\r\n",
            thread->name,thread->prio,stat,thread->stksize,thread->run_times);
    }
    wind_print_space(7);
    return W_ERR_OK;
}


//**********************************************extern functions******************************
