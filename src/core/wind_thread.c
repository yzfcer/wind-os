/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_thread.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 线程相关的代码
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

#include "wind_config.h"
#include "wind_type.h"
#include "wind_core.h"
#include "wind_thread.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_pool.h"
#include "wind_heap.h"
#include "wind_os_hwif.h"
//用来表示

#define WIND_THREAD_PRIO_MIN_LIM 100//优先级的最小值
#define WIND_THREAD_PRIO_MAX_LIM 30000//优先级的最大值
static w_dlist_s threadlist;
static w_dlist_s sleeplist;

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

w_err_t _wind_thread_mod_init(void)
{
    w_err_t err;
    
    DLIST_INIT(threadlist);
    DLIST_INIT(sleeplist);
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
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&threadlist)
    {
        thread = DLIST_OBJ(dnode,w_thread_s,validnode);
        if(thread->name && (wind_strcmp(name,thread->name) == 0))
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

w_err_t wind_thread_init(w_thread_s *thread,
                    const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv,
                    w_prio_e priolevel,
                    w_stack_t *pstk,
                    w_uint16_t stksize)
{
    w_uint16_t i;
    w_stack_t *tmpstk;
    wind_notice("init thread:%s",name);
    //WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread_func != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pstk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(stksize > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(priolevel < PRIO_SYS_LOW && priolevel > PRIO_ZERO,W_ERR_INVALID);
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
    thread->flag = 0;
    CLR_F_THREAD_STKPOOL(thread);
    CLR_F_THREAD_THRPOOL(thread);
    
    thread->name = (char*)name;
    thread->prio = get_prio(priolevel);
    
    tmpstk = _wind_thread_stack_init(thread_entry,0,pstk + stksize -1);
    thread->stack = tmpstk;
    thread->run_times = 0;
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COMMON;
    thread->sleep_ticks = 0;
    wind_disable_interrupt();
    dlist_insert_prio(&threadlist,&thread->validnode,thread->prio);
    wind_enable_interrupt();
    return W_ERR_OK;
}

//创建一个线程
w_thread_s *wind_thread_create(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv,
                    w_prio_e priolevel,
                    w_stack_t *pstk,
                    w_uint16_t stksize)
{
    w_err_t err;
    w_thread_s *thread;
    thread = thread_malloc();
    WIND_ASSERT_RETURN(thread != W_NULL,W_NULL);
    err = wind_thread_init(thread,name,thread_func,argc,argv,priolevel,pstk,stksize);
    if(err == W_ERR_OK)
    {
        SET_F_THREAD_THRPOOL(thread);
        return thread;
    }
    thread_free(thread);
    return W_NULL;
}

#if WIND_STKPOOL_SUPPORT
w_thread_s *wind_thread_create_default(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv)
{
    w_prio_e priol;
    w_stack_t *pstk;
    int stksize;
    w_thread_s *thread;
    priol = PRIO_MID;
    stksize = WIND_STK_SIZE;
    pstk = wind_pool_malloc(stkbufpool);
    WIND_ASSERT_RETURN(pstk != W_NULL,W_NULL);
    thread = wind_thread_create(name,thread_func,argc,argv,priol,pstk,stksize);
    if(thread == W_NULL)
    {
        SET_F_THREAD_STKPOOL(thread);
        return thread;
    }
    wind_pool_free(stkbufpool,pstk);
    return W_NULL;
}
#endif

w_err_t wind_thread_destroy(w_thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    wind_notice("distroy thread:%s",thread->name);
    wind_disable_interrupt();
    dlist_remove(&threadlist,&thread->validnode.dnode);
    //这里需要先释放一些与这个线程相关的一些东西后才能释放这个thread
#if WIND_STKPOOL_SUPPORT
    if(IS_F_THREAD_STKPOOL(thread))
        wind_pool_free(stkbufpool,thread->stack_top);
#endif
    if(IS_F_THREAD_THRPOOL(thread))
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
    dlist_remove(&threadlist,&thread->validnode.dnode);
    thread->prio = prio;
    dlist_insert_prio(&threadlist,&thread->validnode,prio);
    wind_enable_interrupt();
    return W_ERR_OK;
}



w_err_t wind_thread_start(w_thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(thread->magic == WIND_THREAD_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();   
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
    dlist_insert_prio(&sleeplist,&thread->sleepnode,thread->prio);

    wind_enable_interrupt();
#if 0
    foreach_node(dnode,&sleeplist)
    {
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,sleepnode);
        if(thread->prio < 0)
        {
            wind_thread_print(&sleeplist);
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
    WIND_ASSERT_TODO_RETURN(RUN_FLAG,wind_enable_interrupt(),W_ERR_OK);
    foreach_node(dnode,&sleeplist)
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
                dlist_remove(&sleeplist,&thread->sleepnode.dnode);
            }
        }
    }
    wind_enable_interrupt();
    return W_ERR_OK;
}
w_dlist_s *_wind_thread_list(void)
{
    return &threadlist;
}

w_dlist_s *_wind_thread_sleep_list(void)
{
    return &sleeplist;
}


//调试时用到的函数，打印当前的系统中的线程的信息
w_err_t wind_thread_print(void)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    char *stat;
    w_dlist_s *list = &threadlist;
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
