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
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_NAME_LEN 20 //线程名的最大长度，包括 '\0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))
#define SLEEP_TIMEOUT_MAX 0x7fffffff
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
typedef enum __thread_status
{
    THREAD_STATUS_INIT = 0,//初始化状态
    THREAD_STATUS_READY = 1,//就绪状态,可随时运行
    THREAD_STATUS_SUSPEND,//阻塞状态，需要被阻塞源唤醒
    THREAD_STATUS_SLEEP,//休眠状态，可被超时唤醒
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

typedef struct _thread_s thread_s;
typedef struct __threadcb_s threadcb_s;

//定义与线程相关的一些回调函数，需要配置选项支持
#if WIND_THREAD_CALLBACK_SUPPORT
struct __threadcb_s
{
    void (*create)(thread_s *thread);
    void (*start)(thread_s *thread);
    void (*suspend)(thread_s *thread);
    void (*resume)(thread_s *thread);
    void (*dead)(thread_s *thread);
};
#endif 

typedef enum __thr_evt_e
{
    THR_EVT_CREATE,
    THR_EVT_START,
    THR_EVT_SUSPEND,
    THR_EVT_RESUME,
    THR_EVT_DEAD
}thr_evt_e;


//线程对象结构
struct _thread_s
{
    prinode_s validthr;
    prinode_s suspendthr;
    prinode_s sleepthr;
    w_pstack_t stack;//堆栈指针
    w_pstack_t stack_top;//栈顶指针
    w_uint16_t stksize;//堆栈大小，以栈宽度技术
    
    w_int16_t argc;
    w_int8_t **argv;
    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv);
    

    char name[THREAD_NAME_LEN];
    w_int16_t prio;
    
    thread_stat_e runstat;
    w_int32_t sleep_ticks;
    suscause_e cause;//导致状态变化的原因
#if WIND_THREAD_CALLBACK_SUPPORT
    threadcb_s cb;
#endif

#if WIND_HEAP_SUPPORT && WIND_PRIVATE_HEAP_SUPPORT
    void *private_heap;
#endif
};



w_err_t _wind_thread_init(void);

thread_s *wind_thread_create(const w_int8_t *name,
                   prio_e priolevel,
                   w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv,
                   w_pstack_t psck,
                   w_uint16_t stksize);
#if WIND_STKPOOL_SUPPORT
thread_s *wind_thread_create_default(const w_int8_t *name,
                   w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                   w_int16_t argc,
                   w_int8_t **argv);
#else 
#define wind_thread_create_default(n,f,c,v) ERR_COMMAN
#endif
w_err_t wind_thread_changeprio(thread_s *thread,w_int16_t prio);
w_err_t wind_thread_start(thread_s *thread);
w_err_t wind_thread_suspend(thread_s *thread);
w_err_t wind_thread_resume(thread_s *thread);
w_err_t wind_thread_kill(thread_s *thread);
w_err_t wind_thread_killbyname(w_int8_t *name);


w_bool_t  wind_thread_isopen(void);
thread_s *wind_thread_get(const char *name);
thread_s *wind_thread_get_byname(w_int8_t *name);
w_int8_t *wind_thread_curname(void);
thread_s *wind_thread_current(void);
w_int8_t *wind_thread_status(thread_stat_e stat);

w_err_t wind_thread_sleep(w_uint32_t ms);
w_err_t wind_thread_wakeup(void);
w_err_t wind_thread_exit(w_err_t exitcode);

w_err_t wind_thread_print(dlist_s *list);
#if WIND_THREAD_CALLBACK_SUPPORT
w_err_t wind_thread_callback_register(thread_s *thread,thr_evt_e id,void(*cb)(thread_s *));
#endif

#ifdef __cplusplus
}
#endif

#endif //WIND_THREADESS_H_
