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
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_err.h"
#include "wind_stati.h"
#include "wind_mpool.h"
#include "wind_var.h"
#include "wind_assert.h"
#include "wind_heap.h"
#include "wind_macro.h"
#include "core_obj.h"
//用来表示

#define WIND_THREAD_PRIO_MIN_LIM 100//优先级的最小值
#define WIND_THREAD_PRIO_MAX_LIM 30000//优先级的最大值

static w_uint16_t get_prio(prio_e priolevel)
{
    static w_uint16_t pcbcnt = 0;
    w_uint16_t prio;
    pcbcnt ++;
    if(pcbcnt >= 10000)
        pcbcnt = 0;
    prio = (priolevel - 1) * 10000 + pcbcnt;
    return prio;
}

//********************************************internal functions******************************
static pthread_s thread_malloc(void)
{
    pthread_s pthread;    
    pthread = wind_core_alloc(IDX_THREAD);
    WIND_ASSERT_RETURN(pthread != NULL,NULL);
    pthread->used = B_TRUE;
    WIND_DEBUG("alloc pthread->prio:%d\r\n",pthread->prio);
    return pthread;
}

static w_err_t thread_free(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    pthread->used = B_FALSE;
    pthread->parent = NULL;
    wind_memset(pthread->name,0,THREAD_NAME_LEN);
    pthread->prio = -1;
    pthread->runstat = THREAD_STATUS_INIT;
    pthread->cause = CAUSE_COM;
    pthread->stack = NULL;
    pthread->stksize = 0;
    return wind_core_free(IDX_THREAD,pthread);
}

w_err_t wind_thread_distroy(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    //这里需要先释放一些与这个线程相关的一些东西后才能释放这个pcb
#if WIND_HEAP_SUPPORT > 0 && WIND_PRIVATE_HEAP_SUPPORT > 0
    if(pthread->private_heap != NULL)
    {
        wind_heap_free(pthread->private_heap);
    }
#endif
    thread_free(pthread);
    wind_open_interrupt();
    return ERR_OK;
}

static void thread_entry(void *args)
{
    w_err_t err;
    pthread_s pthread;
    pthread = wind_thread_current();
    WIND_INFO("begin to run thread:%s\r\n",pthread->name);
    if(pthread != NULL)
    {
        err = pthread->thread_func(pthread->argc,pthread->argv);
        wind_thread_exit(err);
    }
}
//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t wind_thread_getname(pthread_s pthread,w_int8_t *name)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    wind_strcpy(name,pthread->name);
    return ERR_OK;
}

pthread_s wind_thread_current()
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
pthread_s wind_thread_get_byname(w_int8_t *name)
{
    pthread_s pthread = NULL;
    pdnode_s pdnode;
    WIND_ASSERT_RETURN(name != NULL,NULL);
    wind_close_interrupt();
    foreach_node(pdnode,&g_core.threadlist)
    {
        pthread = PRI_DLIST_OBJ(pdnode,thread_s,validthr);
        if(wind_strcmp(pthread->name,name) == 0)
        {
            wind_open_interrupt();
            return pthread;
        }
    }
    wind_open_interrupt();
    return NULL;
}


w_int8_t *wind_thread_curname(void)
{
    pthread_s pthread;
    pthread = wind_thread_current();
    return pthread->name;
}


//创建一个线程
pthread_s wind_thread_create(const w_int8_t *name,
                   prio_e priolevel,
                   w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv,
                   w_pstack_t pstk,
                   w_uint16_t stksize)
{
    w_uint16_t i;
    pthread_s pthread;
    w_pstack_t tmpstk;

    WIND_INFO("creating thread:%s\r\n",name);
    WIND_ASSERT_RETURN(name != NULL,NULL);
    WIND_ASSERT_RETURN(thread_func != NULL,NULL);
    WIND_ASSERT_RETURN(pstk != NULL,NULL);
    WIND_ASSERT_RETURN(stksize > 0,NULL);
    WIND_ASSERT_RETURN(priolevel < PRIO_SYS_LOW && priolevel > PRIO_ZERO,NULL);
    pthread = thread_malloc();
    WIND_ASSERT_RETURN(pthread != NULL,NULL);
    wind_strcpy(pthread->name,name);
    pthread->prio = get_prio(priolevel);
    PRIO_DNODE_INIT(pthread->validthr);
    PRIO_DNODE_INIT(pthread->suspendthr);
    PRIO_DNODE_INIT(pthread->sleepthr);
    pthread->parent = wind_thread_current();
    pthread->stack_top = pstk;
    pthread->stack = pstk;
    pthread->stksize = stksize;
    for(i = 0;i < stksize;i ++)
        pthread->stack[i] = 0;
    tmpstk = wind_stk_init(thread_entry,0,pstk + stksize -1);
    pthread->argc = argc;
    pthread->argv = argv;
    pthread->thread_func = thread_func;
    pthread->stack = tmpstk;
    pthread->runstat = THREAD_STATUS_READY;
    pthread->cause = CAUSE_COM;
    pthread->sleep_ticks = 0;
#if WIND_HEAP_SUPPORT > 0 && WIND_PRIVATE_HEAP_SUPPORT > 0
    pthread->private_heap = NULL;
#endif
    
    dlist_insert_prio(&g_core.threadlist,&pthread->validthr,pthread->prio);
    WIND_DEBUG("pthread->name:%s\r\n",pthread->name);
    WIND_DEBUG("pthread->stack:0x%x\r\n",pthread->stack);
    WIND_DEBUG("pthread->runstat:%d\r\n",pthread->runstat);
    WIND_DEBUG("pthread->prio:%d\r\n",pthread->prio);
    WIND_DEBUG("pthread->stksize:%d\r\n\r\n",pthread->stksize);
    return pthread;
}


pthread_s wind_thread_create_default(const w_int8_t *name,
                   w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv)
{
    prio_e priol = PRIO_MID;
    w_pstack_t pstk = wind_core_alloc(IDX_STACK);
    int stksize = WIND_STK_SIZE;
    return wind_thread_create(name,priol,thread_func,argc,argv,pstk,stksize);
}

w_err_t wind_thread_changeprio(pthread_s pthread,w_int16_t prio)
{
    pdnode_s node;
    
    w_int16_t minlim = 0,maxlim = 32767;
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN((prio >= minlim) && (prio <= maxlim),ERR_PARAM_OVERFLOW);
    if(wind_thread_isopen())
    {
        minlim = WIND_THREAD_PRIO_MIN_LIM;
        maxlim = WIND_THREAD_PRIO_MAX_LIM;
    }

    wind_close_interrupt();
    WIND_DEBUG("change prio %s:%d\r\n",pthread->name,prio);
    node = &pthread->validthr.node;
    dlist_remove(&g_core.threadlist,node);
    pthread->prio = prio;
    dlist_insert_prio(&g_core.threadlist,&pthread->validthr,prio);
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_thread_start(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();   
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    if(pthread->cb.start != NULL)
        pthread->cb.start(pthread);
#endif
    pthread->runstat = THREAD_STATUS_READY;
    pthread->cause = CAUSE_COM;
    wind_open_interrupt();

    return ERR_OK;
}


w_err_t wind_thread_suspend(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    if(pthread->cb.suspend != NULL)
        pthread->cb.suspend(pthread);
#endif
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_COM;
    wind_open_interrupt();
    return ERR_OK;
}



w_err_t wind_thread_resume(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    
    wind_close_interrupt();
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    if(pthread->cb.resume != NULL)
        pthread->cb.resume(pthread);
#endif
    pthread->runstat = THREAD_STATUS_READY;
    pthread->cause = CAUSE_COM;
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_thread_kill(pthread_s pthread)
{
    pdnode_s node;
    extern void wind_thread_dispatch(void);
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    node = &pthread->validthr.node;
    dlist_remove(&g_core.threadlist,node);
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    if(pthread->cb.dead != NULL)
        pthread->cb.dead(pthread);
#endif
    wind_thread_distroy(pthread);
    wind_open_interrupt();
    wind_thread_dispatch();
    return ERR_OK;
}



//根据线程的名称来销毁线程
w_err_t wind_thread_killbyname(w_int8_t *name)
{
    w_err_t err;
    pthread_s pthread;
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    pthread = wind_thread_get_byname(name);
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    err = wind_thread_kill(pthread);
    return err;
}


//退出线程，在对应的线程中调用
w_err_t wind_thread_exit(w_err_t exitcode)
{
    pthread_s pthread;
    pthread = wind_thread_current();
    WIND_INFO("thread %s exit with code %d\r\n",pthread->name,exitcode);
    wind_thread_kill(pthread);
    wind_thread_dispatch();
    return ERR_OK;
}


//睡眠一段时间,不能在中断中调用这个函数
w_err_t wind_thread_sleep(w_uint32_t ms)
{
    w_uint16_t stcnt;
    //pdnode_s pnode;
    pthread_s pthread = NULL;
    stcnt = ms * WIND_TICK_PER_SEC / 1000;
    if(0 == stcnt)
        stcnt = 1;
    wind_close_interrupt();
    pthread = wind_thread_current();
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_SLEEP;
    pthread->sleep_ticks = stcnt;
    dlist_insert_prio(&g_core.sleeplist,&pthread->sleepthr,pthread->prio);

    wind_open_interrupt();
#if 0
    foreach_node(pnode,&g_core.sleeplist)
    {
        pthread = PRI_DLIST_OBJ(pnode,thread_s,sleepthr);
        if(pthread->prio < 0)
        {
            wind_thread_print(&g_core.sleeplist);
            WIND_ERROR("sleep err\r\n");
            break;
        }
    }
#endif
    wind_thread_dispatch();
    return ERR_OK;
}

w_err_t wind_thread_wakeup(void)
{
    pdnode_s pnode;
    pthread_s pthread;
    wind_close_interrupt();
    WIND_ASSERT_TODO(RUN_FLAG,wind_open_interrupt(),ERR_OK);
    foreach_node(pnode,&g_core.sleeplist)
    {
        pthread = PRI_DLIST_OBJ(pnode,thread_s,sleepthr);
        if(pthread->sleep_ticks > 0)
            pthread->sleep_ticks --;
        if(pthread->sleep_ticks <= 0)
        {
            if(pthread->runstat != THREAD_STATUS_READY)
            {
                pthread->runstat = THREAD_STATUS_READY;
                pthread->cause = CAUSE_SLEEP;
                dlist_remove(&g_core.sleeplist,&pthread->sleepthr.node);
            }
        }
    }
    wind_open_interrupt();
    return ERR_OK;
}

#if WIND_THREAD_CALLBACK_SUPPORT > 0
w_err_t wind_thread_callback_register(pthread_s pthread,thr_evt_e id,void(*cb)(pthread_s))
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);

    switch(id)
    {
    case THR_EVT_CREATE:
        pthread->cb.create = cb;
        break;
    case THR_EVT_START:
        pthread->cb.start = cb;
        break;
    case THR_EVT_SUSPEND:
        pthread->cb.suspend = cb;
        break;
    case THR_EVT_RESUME:
        pthread->cb.resume = cb;
        break;
    case THR_EVT_DEAD:
        pthread->cb.dead = cb;
        break;
    default:
        break;
    }
    return ERR_OK;

}
#endif


//调试时用到的函数，打印当前的系统中的线程的信息
w_err_t wind_thread_print(pdlist_s list)
{
    pdnode_s pnode;
    pthread_s pthread;
    char *stat;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    //pnode = list->head;
    wind_printf("\r\n\r\nthread list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-8s %-10s\r\n","thread","prio","state");
    wind_printf("----------------------------------------------\r\n");
    foreach_node(pnode,list)
    {
        pthread = PRI_DLIST_OBJ(pnode,thread_s,validthr);
        stat = wind_thread_status(pthread->runstat);
        wind_printf("%-16s %-8d %-10s\r\n",
            pthread->name,pthread->prio,stat);
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}


//**********************************************extern functions******************************
