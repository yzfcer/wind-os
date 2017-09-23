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
#include "wind_list.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_err.h"
#include "wind_stati.h"
#include "wind_mpool.h"
#include "wind_var.h"
#include "wind_assert.h"
#include "wind_heap.h"
//用来表示
//w_bool_t wind_tick_init_flag = false;

#define WIND_THREAD_PRIO_MIN_LIM 100//优先级的最小值
#define WIND_THREAD_PRIO_MAX_LIM 30000//优先级的最大值


//********************************************internal functions******************************
static pthread_s pcb_malloc(prio_e priolevel)
{
    pthread_s pthread;
    //w_int16_t cnt,priolim = 0;

    WIND_ASSERT_RETURN(priolevel < PRIO_SYS_LOW && priolevel > PRIO_ZERO,NULL);
    pthread = wind_core_alloc(STAT_PROC);
    WIND_ASSERT_RETURN(pthread != NULL,NULL);
    //if(wind_thread_isopen())
    //    priolim = WIND_THREAD_PRIO_MIN_LIM;
    pthread->used = B_TRUE;
    pthread->prio = (priolevel - 1) * 10000 + g_core.pcbcnt;
    g_core.pcbcnt ++;
    WIND_DEBUG("alloc pthread->prio:%d\r\n",pthread->prio);
    return pthread;
}

w_err_t pcb_free(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    pthread->used = B_FALSE;
    pthread->parent = NULL;
    wind_memset(pthread->name,0,THREAD_NAME_LEN);
    //pthread->name = NULL;
    pthread->prio = -1;
    pthread->runstat = THREAD_STATUS_UNKNOWN;
    pthread->cause = CAUSE_COM;
    pthread->pstk = NULL;
    pthread->stksize = 0;
    //pthread->superpermflag = false;
    WIND_DEBUG("minus:%d,%d\r\n",G_STAT[STAT_PROC].used,G_STAT[STAT_PROC].max);
    return wind_core_free(STAT_PROC,pthread);
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
    pcb_free(pthread);
    wind_open_interrupt();
    return ERR_OK;
}

static void thread_entry(void *args)
{
    w_err_t err;
    pthread_s pthread;
    //这里添加了tick的初始化时是因为在调试过程中遇到了暂时不能解决的问题，
    //以后tick的初始化还是应该放在框架代码内

    pthread = wind_thread_current();
    WIND_INFO("begin to run procesess:%s\r\n",pthread->name);
    if(pthread != NULL)
    {
        err = pthread->procfunc(pthread->argc,pthread->argv);
        wind_thread_exit(err);
    }
}
//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t wind_thread_getname(pthread_s pthread,w_int8_t *name)
{
    //pthread_s pthread;
    //pthread = get_pcb_byhandler(handler);

    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    wind_strcpy(name,pthread->name);
    //pthread->name = name;
    return ERR_OK;
}

pthread_s wind_thread_current()
{
    return gwind_cur_pcb;
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
    pnode_s pnode;
    WIND_ASSERT_RETURN(name != NULL,NULL);
    wind_close_interrupt();
    pnode = g_core.threadlist.head;
    while(pnode)
    {
        pthread = (pthread_s)pnode->obj;
        if(wind_strcmp(pthread->name,name) == 0)
        {
            wind_open_interrupt();
            return pthread;
        }
        pnode = pnode->next;
    }
    WIND_ASSERT_TODO(pthread != NULL,wind_open_interrupt(),NULL);
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
                   w_err_t (*procfunc)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv,
                   pstack_t pstk,
                   w_uint16_t stksize)
{
    w_uint16_t i;
    pthread_s pthread;
    pstack_t tmpstk;
    pnode_s pnode;

    WIND_INFO("Creating process:%s\r\n",name);
    WIND_ASSERT_RETURN(name != NULL,NULL);
    WIND_ASSERT_RETURN(procfunc != NULL,NULL);
    WIND_ASSERT_RETURN(pstk != NULL,NULL);
    WIND_ASSERT_RETURN(stksize > 0,NULL);
    
    pthread = pcb_malloc(priolevel);
    WIND_ASSERT_RETURN(pthread != NULL,NULL);
    //WIND_DEBUG("pcb addr:0x%x\r\n",pthread);
    wind_strcpy(pthread->name,name);
    
    pthread->parent = wind_thread_current();
    pthread->pstktop = pstk;
    pthread->pstk = pstk;
    pthread->stksize = stksize;
    for(i = 0;i < stksize;i ++)
        pthread->pstk[i] = 0;
    tmpstk = wind_stk_init(thread_entry,0,pstk + stksize -1);
    pthread->argc = argc;
    pthread->argv = argv;
    pthread->procfunc = procfunc;
    pthread->pstk = tmpstk;
    pthread->runstat = THREAD_STATUS_READY;
    pthread->cause = CAUSE_COM;
    pthread->sleep_ticks = 0;
#if WIND_HEAP_SUPPORT > 0 && WIND_PRIVATE_HEAP_SUPPORT > 0
    pthread->private_heap = NULL;//wind_heap_alloc_default(PRIVATE_HEAP_SIZE);
#endif
    
    pnode = wind_node_malloc(CORE_TYPE_PCB);
    //WIND_ASSERT_RETURN(pnode != NULL,NULL);
    if(pnode == NULL)
    {
        WIND_WARN("warn:node is not enough\r\n");
        pthread->runstat = THREAD_STATUS_DEAD;
        pthread->cause = CAUSE_COM;
        pcb_free(pthread);
        return NULL;
    }
    wind_node_bindobj(pnode,CORE_TYPE_PCB,pthread->prio,pthread);
    wind_list_insert(&g_core.threadlist,pnode);
    WIND_DEBUG("pthread->name:%s\r\n",pthread->name);
    WIND_DEBUG("pthread->pstk:0x%x\r\n",pthread->pstk);
    WIND_DEBUG("pthread->runstat:%d\r\n",pthread->runstat);
    WIND_DEBUG("pthread->prio:%d\r\n",pthread->prio);
    WIND_DEBUG("pthread->stksize:%d\r\n\r\n",pthread->stksize);
    //wind_printf("thread:%p,%p,%p\r\n",pthread,&pthread->pstk,pthread->pstk);

    //wind_open_interrupt();
    return pthread;
    
}

w_err_t wind_thread_changeprio(pthread_s pthread,w_int16_t prio)
{
    pnode_s node;
    
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
    node = wind_list_search(&g_core.threadlist,pthread);
    node->key = prio;
    pthread->prio = prio;
    wind_list_remove(&g_core.threadlist,node);
    wind_list_insert(&g_core.threadlist,node);
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
    return ERR_OK;//wind_disable_switch(wind_pro_suspend,(void *)pthread);
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
    pnode_s node;
    extern void wind_thread_dispatch(void);
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    node = wind_list_search(&g_core.threadlist,pthread);
    WIND_ASSERT_TODO(node != NULL,wind_open_interrupt(),ERR_NULL_POINTER);
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    if(pthread->cb.dead != NULL)
        pthread->cb.dead(pthread);
#endif
    wind_list_remove(&g_core.threadlist,node);
    wind_node_free(node);
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
    WIND_INFO("proc %s exit with code %d\r\n",pthread->name,exitcode);
    wind_thread_kill(pthread);
    wind_thread_dispatch();
    return ERR_OK;
}

list_s procsleeplist = {NULL,NULL,0};//sleeping process list

//睡眠一段时间,不能在中断中调用这个函数
w_err_t wind_thread_sleep(w_uint32_t ms)
{
    w_uint16_t stcnt;
    pthread_s pthread = NULL;
    pnode_s pnode = NULL;
    stcnt = ms * WIND_TICK_PER_SEC / 1000;
    if(0 == stcnt)
        stcnt = 1;
    pthread = wind_thread_current();
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_SLEEP;
    pnode = wind_node_malloc(CORE_TYPE_PCB);
    WIND_ASSERT_RETURN(pnode != NULL,ERR_NULL_POINTER);
    wind_node_bindobj(pnode,CORE_TYPE_PCB,stcnt,pthread);
    wind_list_insert(&procsleeplist,pnode);
    pnode = procsleeplist.head;
    while(pnode)
    {
        if(pnode->key < 0)
        {
            wind_thread_print(&procsleeplist);
            WIND_ERROR("sleep err\r\n");
            break;
        }
        pnode = pnode->next;
    }
    wind_thread_dispatch();
    wind_open_interrupt();
    return ERR_OK;
}

void wind_thread_wakeup(void)
{
    pnode_s pnode = NULL;
    pthread_s pthread = NULL;
    pnode = procsleeplist.head;
    while(RUN_FLAG && (pnode != NULL))
    {        
        if(pnode->key > 0)
            pnode->key --;
        if(pnode->key <= 0)
        {
            pthread = (pthread_s)pnode->obj;
            if(pthread->runstat != THREAD_STATUS_READY)
            {
                pthread->runstat = THREAD_STATUS_READY;
                pthread->cause = CAUSE_SLEEP;
                wind_list_remove(&procsleeplist,pnode);
                wind_node_free(pnode);
            }
        }
        pnode = pnode->next;
    }
}

#if WIND_THREAD_CALLBACK_SUPPORT > 0
w_err_t wind_thread_callback_register(pthread_s pthread,procevt_e id,void(*cb)(pthread_s))
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);

    switch(id)
    {
    //case PROCEVT_CREATE:
    //    pthread->cb.proc_created = cb;
    //    break;
    case PROCEVT_START:
        pthread->cb.start = cb;
        break;
    case PROCEVT_SUSPEND:
        pthread->cb.suspend = cb;
        break;
    case PROCEVT_RESUME:
        pthread->cb.resume = cb;
        break;
    case PROCEVT_DEAD:
        pthread->cb.dead = cb;
        break;
    default:
        break;
    }
    return ERR_OK;

}
#endif


//调试时用到的函数，打印当前的系统中的线程的信息
w_err_t wind_thread_print(plist_s list)
{
    pnode_s pnode;
    pthread_s pthread;
    char *stat;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    pnode = list->head;
    wind_printf("\r\n\r\nthread list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-8s %-10s\r\n","thread","prio","state");
    wind_printf("----------------------------------------------\r\n");
    
    while(pnode)
    {
        pthread = (pthread_s)pnode->obj;
        stat = wind_thread_status(pthread->runstat);
        wind_printf("%-16s %-8d %-10s\r\n",
            pthread->name,pthread->prio,stat);
        pnode = pnode->next;
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}


//**********************************************extern functions******************************
