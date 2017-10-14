/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_thread.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 线程相关的代码头文件
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
#ifndef WIND_THREADESS_H_
#define WIND_THREADESS_H_


#include "wind_config.h"
#include "wind_type.h"
#include "dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCB_NUM_LIMIT 512 //线程总数的上限值
#define THREAD_NAME_LEN 20 //线程名的最大长度，包括 '\0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))

//线程的优先等级，
typedef enum _PRIOLEVEL
{
    PRIO_ZERO,
    PRIO_HIGH,
    PRIO_MID,
    PRIO_LOW,
    PRIO_SYS_LOW
} prio_e;
//线程状态列表
typedef enum __proc_status
{
    THREAD_STATUS_INIT = 0,//初始化状态
    THREAD_STATUS_READY = 1,//就绪状态
    THREAD_STATUS_SUSPEND,//阻塞状态
    THREAD_STATUS_SLEEP,//休眠状态
    THREAD_STATUS_DEAD,//死亡状态，将永远不会被唤醒和调度
}thread_stat_e;

typedef enum __suscause
{
    CAUSE_NONE = 0,
    CAUSE_COM,
    CAUSE_SLEEP,
    CAUSE_SEM,
    CAUSE_MSG,
    CAUSE_LOCK,
    CAUSE_CNT
}suscause_e;


//定义与线程相关的一些回调函数，需要配置选项支持
#if WIND_THREAD_CALLBACK_SUPPORT > 0
typedef enum __procevt_e
{
    //PROCEVT_CREATE,
    PROCEVT_START,
    PROCEVT_SUSPEND,
    PROCEVT_RESUME,
    PROCEVT_DEAD
}procevt_e;


struct _thread_s;
typedef struct __threadcb_s
{
    //void (*proc_created)(struct _pcb_s *pthread);
    void (*start)(struct _thread_s *pthread);
    void (*suspend)(struct _thread_s *pthread);
    void (*resume)(struct _thread_s *pthread);
    void (*dead)(struct _thread_s *pthread);
}threadcb_s;
#endif 


//线程控制PCB
typedef struct _thread_s
{
    dnode_s validthr;
    dnode_s suspendthr;
    dnode_s sleepthr;
    pstack_t stack;
    pstack_t stack_top;
    w_uint16_t stksize;
    
    w_err_t (*procfunc)(w_int32_t argc,w_int8_t **argv);
    w_int16_t argc;
    w_int8_t **argv;
    
    struct _thread_s *parent;
    char name[THREAD_NAME_LEN];//
    w_bool_t used;
    w_int16_t prio;
    
    thread_stat_e runstat;
    w_int32_t sleep_ticks;
    suscause_e cause;//导致状态变化的原因
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    threadcb_s cb;
#endif

#if WIND_HEAP_SUPPORT > 0 && WIND_PRIVATE_HEAP_SUPPORT > 0
    void *private_heap;
#endif
}thread_s,*pthread_s;




pthread_s wind_thread_create(const w_int8_t *name,
                   prio_e priolevel,
                   w_err_t (*procfunc)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv,
                   pstack_t psck,
                   w_uint16_t stksize);
w_err_t wind_thread_changeprio(pthread_s pthread,w_int16_t prio);
w_err_t wind_thread_start(pthread_s pthread);
w_err_t wind_thread_suspend(pthread_s pthread);
w_err_t wind_thread_resume(pthread_s pthread);
w_err_t wind_thread_kill(pthread_s pthread);
w_err_t wind_thread_killbyname(w_int8_t *name);


w_bool_t  wind_thread_isopen(void);
w_err_t   wind_thread_getname(pthread_s pthread,w_int8_t *name);
pthread_s wind_thread_get_byname(w_int8_t *name);
w_int8_t* wind_thread_curname(void);
pthread_s wind_thread_current(void);
w_int8_t* wind_thread_status(thread_stat_e stat);

w_err_t wind_thread_sleep(w_uint32_t ms);
void    wind_thread_wakeup(void);
w_err_t wind_thread_exit(w_err_t exitcode);

void insert_thread(pdlist_s list,pthread_s thread);
w_err_t wind_thread_print(pdlist_s list);
#if WIND_THREAD_CALLBACK_SUPPORT > 0
w_err_t wind_thread_callback_register(pthread_s pthread,procevt_e id,void(*cb)(pthread_s));
#endif



#if WIND_THREAD_MAX_NUM > PCB_NUM_LIMIT
#error "PCB_MAX_NUM should not be a number larger than PCB_NUM_LIMIT!"
#endif //#if PCB_MAX_NUM > PCB_NUM_LIMIT

#if WIND_THREAD_MAX_NUM < 8
#error "WIND_THREAD_MAX_NUM should not be a number smaller than 8,as operation system reserves some threads!"
#endif //#if WIND_THREAD_MAX_NUM < 8


#ifdef __cplusplus
}
#endif

#endif //WIND_THREADESS_H_
