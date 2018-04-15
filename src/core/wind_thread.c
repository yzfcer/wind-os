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
#include "wind_os_hwif.h"
#include "wind_core.h"
#include "wind_thread.h"
#include "wind_mem.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_stati.h"
#include "wind_mpool.h"
#include "wind_var.h"
#include "wind_heap.h"
#include "wind_macro.h"
//用来表示

#define WIND_THREAD_PRIO_MIN_LIM 100//优先级的最小值
#define WIND_THREAD_PRIO_MAX_LIM 30000//优先级的最大值

static w_uint16_t get_prio(prio_e priolevel)
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
static WIND_MPOOL(threadpool,WIND_THREAD_MAX_NUM,sizeof(thread_s));
#if WIND_STKPOOL_SUPPORT
WIND_MPOOL(stkbufpool,WIND_STK_MAX_NUM,WIND_STK_SIZE *sizeof(w_stack_t));
#endif

static __INLINE__ thread_s *thread_malloc(void)
{
    return (thread_s*)wind_pool_alloc(threadpool);
}

static __INLINE__ w_err_t thread_free(thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    wind_memset(thread->name,0,THREAD_NAME_LEN);
    thread->prio = -1;
    thread->runstat = THREAD_STATUS_INIT;
    thread->cause = CAUSE_COM;
    thread->stack = NULL;
    thread->stksize = 0;
    return wind_pool_free(threadpool,thread);
}

w_err_t wind_thread_distroy(thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    wind_notice("distroy thread:%s",thread->name);
    wind_close_interrupt();
    //这里需要先释放一些与这个线程相关的一些东西后才能释放这个thread
#if WIND_HEAP_SUPPORT && WIND_PRIVATE_HEAP_SUPPORT
    if(thread->private_heap != NULL)
    {
        wind_heap_free(thread->private_heap);
    }
#endif
    wind_pool_free(stkbufpool,thread->stack_top);
    thread_free(thread);
    wind_open_interrupt();
    return ERR_OK;
}

static void thread_entry(void *args)
{
    w_err_t err;
    thread_s *thread;
    thread = wind_thread_current();
    wind_notice("run thread:%s.",thread->name);
    if(thread != NULL)
    {
        err = thread->thread_func(thread->argc,thread->argv);
        wind_thread_exit(err);
    }
}
//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t _wind_thread_init(void)
{
    w_err_t err;
#if WIND_STKPOOL_SUPPORT
    err = wind_pool_create("stkbuf",stkbufpool,sizeof(stkbufpool),WIND_STK_SIZE *sizeof(w_stack_t));
    WIND_ASSERT_RETURN(err == ERR_OK,err);
#endif    
    err = wind_pool_create("thread",threadpool,sizeof(threadpool),sizeof(thread_s));
    return err;
}

w_err_t wind_thread_getname(thread_s *thread,w_int8_t *name)
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    wind_strcpy(name,thread->name);
    return ERR_OK;
}

thread_s *wind_thread_current()
{
    return THREAD_FROM_MEMBER(gwind_cur_stack,thread_s,stack);
}

w_int8_t* wind_thread_status(thread_stat_e stat)
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

//这个函数不一定能找到实际的线程，因为无法保证所有的线程的线程名都不一样。
thread_s *wind_thread_get_byname(w_int8_t *name)
{
    thread_s *thread = NULL;
    dnode_s *pdnode;
    WIND_ASSERT_RETURN(name != NULL,NULL);
    wind_close_interrupt();
    foreach_node(pdnode,&g_core.threadlist)
    {
        thread = PRI_DLIST_OBJ(pdnode,thread_s,validthr);
        if(wind_strcmp(thread->name,name) == 0)
        {
            wind_open_interrupt();
            return thread;
        }
    }
    wind_open_interrupt();
    wind_warn("can not find thread:%s",name);
    return NULL;
}


w_int8_t *wind_thread_curname(void)
{
    thread_s *thread;
    thread = wind_thread_current();
    return thread->name;
}


//创建一个线程
thread_s *wind_thread_create(const w_int8_t *name,
                   prio_e priolevel,
                   w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv,
                   w_pstack_t pstk,
                   w_uint16_t stksize)
{
    w_uint16_t i;
    thread_s *thread;
    w_pstack_t tmpstk;

    wind_notice("create thread:%s",name);
    WIND_ASSERT_RETURN(name != NULL,NULL);
    WIND_ASSERT_RETURN(thread_func != NULL,NULL);
    WIND_ASSERT_RETURN(pstk != NULL,NULL);
    WIND_ASSERT_RETURN(stksize > 0,NULL);
    WIND_ASSERT_RETURN(priolevel < PRIO_SYS_LOW && priolevel > PRIO_ZERO,NULL);
    thread = thread_malloc();
    WIND_ASSERT_RETURN(thread != NULL,NULL);
    wind_strcpy(thread->name,name);
    thread->prio = get_prio(priolevel);
    PRIO_DNODE_INIT(thread->validthr);
    PRIO_DNODE_INIT(thread->suspendthr);
    PRIO_DNODE_INIT(thread->sleepthr);
    thread->stack_top = pstk;
    thread->stack = pstk;
    thread->stksize = stksize;
    for(i = 0;i < stksize;i ++)
        thread->stack[i] = 0;
    tmpstk = wind_stk_init(thread_entry,0,pstk + stksize -1);
    thread->argc = argc;
    thread->argv = argv;
    thread->thread_func = thread_func;
    thread->stack = tmpstk;
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COM;
    thread->sleep_ticks = 0;
#if WIND_HEAP_SUPPORT && WIND_PRIVATE_HEAP_SUPPORT
    thread->private_heap = NULL;
#endif
    
    dlist_insert_prio(&g_core.threadlist,&thread->validthr,thread->prio);
    wind_debug("create thread info");
    wind_debug("thread->name:%s",thread->name);
    wind_debug("thread->stack:0x%x",thread->stack);
    wind_debug("thread->runstat:%d",thread->runstat);
    wind_debug("thread->prio:%d",thread->prio);
    wind_debug("thread->stksize:%d\r\n",thread->stksize);
    return thread;
}

#if WIND_STKPOOL_SUPPORT
thread_s *wind_thread_create_default(const w_int8_t *name,
                   w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv)
{
    prio_e priol;
    w_pstack_t pstk;
    int stksize;
    priol = PRIO_MID;
    stksize = WIND_STK_SIZE;
    pstk = wind_pool_alloc(stkbufpool);
    WIND_ASSERT_RETURN(pstk != NULL,NULL);
    return wind_thread_create(name,priol,thread_func,argc,argv,pstk,stksize);
}
#endif

w_err_t wind_thread_changeprio(thread_s *thread,w_int16_t prio)
{
    dnode_s *node;
    
    w_int16_t minlim = 0,maxlim = 32767;
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN((prio >= minlim) && (prio <= maxlim),ERR_PARAM_OVERFLOW);
    if(wind_thread_isopen())
    {
        minlim = WIND_THREAD_PRIO_MIN_LIM;
        maxlim = WIND_THREAD_PRIO_MAX_LIM;
    }

    wind_close_interrupt();
    wind_debug("change prio %s:%d\r\n",thread->name,prio);
    node = &thread->validthr.node;
    dlist_remove(&g_core.threadlist,node);
    thread->prio = prio;
    dlist_insert_prio(&g_core.threadlist,&thread->validthr,prio);
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_thread_start(thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();   
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.start != NULL)
        thread->cb.start(thread);
#endif
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COM;
    wind_open_interrupt();

    return ERR_OK;
}


w_err_t wind_thread_suspend(thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.suspend != NULL)
        thread->cb.suspend(thread);
#endif
    thread->runstat = THREAD_STATUS_SUSPEND;
    thread->cause = CAUSE_COM;
    wind_open_interrupt();
    return ERR_OK;
}



w_err_t wind_thread_resume(thread_s *thread)
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.resume != NULL)
        thread->cb.resume(thread);
#endif
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_COM;
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_thread_kill(thread_s *thread)
{
    dnode_s *node;
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    node = &thread->validthr.node;
    dlist_remove(&g_core.threadlist,node);
#if WIND_THREAD_CALLBACK_SUPPORT
    if(thread->cb.dead != NULL)
        thread->cb.dead(thread);
#endif
    wind_thread_distroy(thread);
    wind_open_interrupt();
    _wind_thread_dispatch();
    return ERR_OK;
}



//根据线程的名称来销毁线程
w_err_t wind_thread_killbyname(w_int8_t *name)
{
    w_err_t err;
    thread_s *thread;
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    thread = wind_thread_get_byname(name);
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);
    err = wind_thread_kill(thread);
    return err;
}


//退出线程，在对应的线程中调用
w_err_t wind_thread_exit(w_err_t exitcode)
{
    thread_s *thread;
    thread = wind_thread_current();
    wind_notice("thread \"%s\" exit:%d",thread->name,exitcode);
    wind_thread_kill(thread);
    _wind_thread_dispatch();
    return ERR_OK;
}


//睡眠一段时间,不能在中断中调用这个函数
w_err_t wind_thread_sleep(w_uint32_t ms)
{
    w_uint16_t stcnt;
    thread_s *thread = NULL;
    stcnt = ms *WIND_TICK_PER_SEC / 1000;
    if(0 == stcnt)
        stcnt = 1;
    wind_close_interrupt();
    thread = wind_thread_current();
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_SLEEP;
    thread->sleep_ticks = stcnt;
    dlist_insert_prio(&g_core.sleeplist,&thread->sleepthr,thread->prio);

    wind_open_interrupt();
#if 0
    foreach_node(pnode,&g_core.sleeplist)
    {
        thread = PRI_DLIST_OBJ(pnode,thread_s,sleepthr);
        if(thread->prio < 0)
        {
            wind_thread_print(&g_core.sleeplist);
            wind_error("sleep err");
            break;
        }
    }
#endif
    _wind_thread_dispatch();
    return ERR_OK;
}

w_err_t wind_thread_wakeup(void)
{
    dnode_s *pnode;
    thread_s *thread;
    wind_close_interrupt();
    WIND_ASSERT_TODO(RUN_FLAG,wind_open_interrupt(),ERR_OK);
    foreach_node(pnode,&g_core.sleeplist)
    {
        thread = PRI_DLIST_OBJ(pnode,thread_s,sleepthr);
        if(thread->sleep_ticks > 0)
            thread->sleep_ticks --;
        if(thread->sleep_ticks <= 0)
        {
            if(thread->runstat == THREAD_STATUS_SLEEP)
            {
                thread->runstat = THREAD_STATUS_READY;
                thread->cause = CAUSE_SLEEP;
                dlist_remove(&g_core.sleeplist,&thread->sleepthr.node);
            }
        }
    }
    wind_open_interrupt();
    return ERR_OK;
}

#if WIND_THREAD_CALLBACK_SUPPORT
w_err_t wind_thread_callback_register(thread_s *thread,thr_evt_e id,void(*cb)(thread_s *))
{
    WIND_ASSERT_RETURN(thread != NULL,ERR_NULL_POINTER);

    switch(id)
    {
    case THR_EVT_CREATE:
        thread->cb.create = cb;
        break;
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
    return ERR_OK;

}
#endif


//调试时用到的函数，打印当前的系统中的线程的信息
w_err_t wind_thread_print(dlist_s *list)
{
    dnode_s *pnode;
    thread_s *thread;
    char *stat;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    //pnode = list->head;
    wind_printf("\r\n\r\nthread list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-8s %-10s %-10s\r\n","thread","prio","state","stacksize");
    wind_printf("----------------------------------------------\r\n");
    foreach_node(pnode,list)
    {
        thread = PRI_DLIST_OBJ(pnode,thread_s,validthr);
        stat = wind_thread_status(thread->runstat);
        wind_printf("%-16s %-8d %-10s %-10d\r\n",
            thread->name,thread->prio,stat,thread->stksize);
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}


//**********************************************extern functions******************************
