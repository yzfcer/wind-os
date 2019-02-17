/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_thread.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 线程相关的代码头文件
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
#ifndef WIND_THREADESS_H_
#define WIND_THREADESS_H_


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif
#define WIND_THREAD_MAGIC 0x35BA6857
#define THREAD_NAME_LEN 20 //线程名的最大长度，包括 '\0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))
#define SLEEP_TIMEOUT_MAX 0x7fffffff

#define F_THREAD_THRPOOL 0x01
#define IS_F_THREAD_THRPOOL(thread) ((thread->flag & F_THREAD_THRPOOL) == F_THREAD_THRPOOL)
#define SET_F_THREAD_THRPOOL(thread) (thread->flag |= F_THREAD_THRPOOL)
#define CLR_F_THREAD_THRPOOL(thread) (thread->flag &= (~F_THREAD_THRPOOL))

#define F_THREAD_STKPOOL 0x02
#define IS_F_THREAD_STKPOOL(thread) ((thread->flag & F_THREAD_STKPOOL) == F_THREAD_STKPOOL)
#define SET_F_THREAD_STKPOOL(thread) (thread->flag |= F_THREAD_STKPOOL)
#define CLR_F_THREAD_STKPOOL(thread) (thread->flag &= (~F_THREAD_STKPOOL))

//线程的优先等级，
typedef enum _w_prio_e
{
    PRIO_ZERO,
    PRIO_HIGH,
    PRIO_MID,
    PRIO_LOW,
    PRIO_SYS_LOW
} w_prio_e;

//线程状态列表
typedef enum __w_thread_stat_e
{
    THREAD_STATUS_INIT = 0,//初始化状态
    THREAD_STATUS_READY = 1,//就绪状态,可随时运行
    THREAD_STATUS_SUSPEND,//阻塞状态，需要被阻塞源唤醒
    THREAD_STATUS_SLEEP,//休眠状态，可被超时唤醒
    THREAD_STATUS_DEAD,//死亡状态，将永远不会被唤醒和调度
}w_thread_stat_e;

typedef enum __suscause
{
    CAUSE_NONE = 0,
    CAUSE_COMMON,
    CAUSE_SLEEP,
    CAUSE_SEM,
    CAUSE_MSG,
    CAUSE_LOCK,
    CAUSE_CNT
}w_suscause_e;

typedef struct _w_thread_s w_thread_s;

typedef enum __thr_evt_e
{
    THR_EVT_CREATE,
    THR_EVT_START,
    THR_EVT_SUSPEND,
    THR_EVT_RESUME,
    THR_EVT_DEAD
}w_thr_evt_e;


//线程对象结构
struct _w_thread_s
{
    w_uint32_t magic;
    w_prinode_s validnode;
    w_prinode_s suspendnode;
    w_prinode_s sleepnode;
    w_stack_t *stack;//堆栈指针
    w_stack_t *stack_top;//栈顶指针
    w_uint16_t stksize;//堆栈大小，以栈宽度技术
    
    w_int16_t argc;
    w_int16_t flag;
    
    char **argv;
    w_err_t (*thread_func)(w_int32_t argc,char **argv);
    
    char *name;
    w_int16_t prio;
    w_uint16_t tid;
    w_uint32_t run_times;
    w_thread_stat_e runstat;
    w_int32_t sleep_ticks;
    w_suscause_e cause;//导致状态变化的原因

};



w_err_t _wind_thread_mod_init(void);
w_err_t _wind_thread_wakeup(void);
w_dlist_s *_wind_thread_list(void);
w_dlist_s *_wind_thread_sleep_list(void);

w_thread_s *wind_thread_get(const char *name);
w_thread_s *wind_thread_current(void);
char *wind_thread_curname(void);

w_err_t wind_thread_init(w_thread_s *thread,
                    const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv,
                    w_prio_e priolevel,
                    w_stack_t *psck,
                    w_uint16_t stksize);

w_thread_s *wind_thread_create(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv,
                    w_prio_e priolevel,
                    w_stack_t *psck,
                    w_uint16_t stksize);

#if WIND_STKPOOL_SUPPORT
w_thread_s *wind_thread_create_default(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv);
#else 
#define wind_thread_create_default(n,f,c,v) W_ERR_FAIL
#endif

w_err_t wind_thread_set_priority(w_thread_s *thread,w_int16_t prio);
w_err_t wind_thread_start(w_thread_s *thread);
w_err_t wind_thread_suspend(w_thread_s *thread);
w_err_t wind_thread_resume(w_thread_s *thread);
w_err_t wind_thread_destroy(w_thread_s *thread);

w_err_t wind_thread_sleep(w_uint32_t ms);
w_err_t wind_thread_exit(w_err_t exitcode);

w_err_t wind_thread_print(void);


#ifdef __cplusplus
}
#endif

#endif //WIND_THREADESS_H_
