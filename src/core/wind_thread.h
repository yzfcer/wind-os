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
#include "wind_types.h"
#include "wind_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCB_NUM_LIMIT 32767 //线程总数的上限值
#define PROCESS_NAME_LEN 20 //线程名的最大长度，包括 '\0'

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
    PROC_STATUS_READY = 1,//就绪状态
    PROC_STATUS_SUSPEND,//阻塞状态，可以通过定时器唤醒
    PROC_STATUS_SLEEP,//休眠状态，程序将不能通过定时器唤醒，而需要手动唤醒
    PROC_STATUS_DEAD,//死亡状态，将永远不会被唤醒
    PROC_STATUS_UNKNOWN//未知状态
}proc_status_e;

typedef enum __suscause
{
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


struct _pcb_s;
typedef struct __procCB_s
{
    //void (*proc_created)(struct _pcb_s *pthread);
    void (*start)(struct _pcb_s *pthread);
    void (*suspend)(struct _pcb_s *pthread);
    void (*resume)(struct _pcb_s *pthread);
    void (*dead)(struct _pcb_s *pthread);
}procCB_s;
#endif 


//线程控制PCB
typedef struct _pcb_s
{
    pstack_t pstk;//注意pstk这个参数要放在第一个位置上，不能挪动
    pstack_t pstktop;
    err_t (*procfunc)(s32_t argc,s8_t **argv);
    s16_t argc;
    s8_t **argv;
    struct _pcb_s *parent;
    char name[PROCESS_NAME_LEN];//;//
    bool_t used;
    s16_t prio;
    s8_t proc_status;
    u16_t stksize;
    suscause_e cause;//导致状态变化的原因
#if WIND_THREAD_CALLBACK_SUPPORT > 0
    procCB_s cb;
#endif

#if WIND_HEAP_SUPPORT > 0 && WIND_PRIVATE_HEAP_SUPPORT > 0
    void *private_heap;
#endif
}pcb_s,*pthread_s;


//获取当前的线程PCB
pthread_s wind_get_cur_proc(void);
//打开外部任务创建
bool_t wind_thread_isopen(void);

err_t wind_thread_getname(pthread_s pthread,s8_t *name);
pthread_s wind_get_proc_byname(s8_t *name);
//err_t wind_thread_get_super_permission(void);
pthread_s get_pcb_byname(s8_t *name);
//这个函数将会支持消息驱动模式，但暂时还能不能支持，因为一些支撑代码还没有建立
pthread_s wind_thread_create(const s8_t *name,
                   prio_e priolevel,
                   err_t (*procfunc)(s32_t argc,s8_t **argv),
                   s16_t argc,
                   s8_t **argv,
                   pstack_t psck,
                   u16_t stksize);
err_t wind_thread_changeprio(pthread_s pthread,s16_t prio);
err_t wind_thread_start(pthread_s pthread);
err_t wind_thread_suspend(pthread_s pthread);
err_t wind_thread_resume(pthread_s pthread);
err_t wind_thread_kill(pthread_s pthread);
err_t wind_thread_killN(s8_t *name);
s8_t *wind_thread_get_curname(void);
err_t wind_thread_exit(err_t exitcode);

#if WIND_THREAD_CALLBACK_SUPPORT > 0
err_t wind_thread_callback_register(pthread_s pthread,procevt_e id,void(*cb)(pthread_s));
#endif
err_t wind_thread_showlist(pnode_s nodes);

#if WIND_THREAD_MAX_NUM > PCB_NUM_LIMIT
#error "PCB_MAX_NUM should not be a number larger than PCB_NUM_LIMIT!"
#endif //PCB_MAX_NUM > PCB_NUM_LIMIT

#if WIND_THREAD_MAX_NUM < 8
#error "WIND_THREAD_MAX_NUM should not be a number smaller than 8,as operation system reserves some processes!"
#endif


#ifdef __cplusplus
}
#endif

#endif //WIND_THREADESS_H_
