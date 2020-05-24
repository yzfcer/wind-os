/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_thread.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : 线程相关的代码头文件
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_THREADESS_H_
#define WIND_THREADESS_H_
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define WIND_THREAD_MAGIC 0x35BA6857
#define WIND_THREAD_STK_MARK 0x24681357
#define THREAD_NAME_LEN 20 //Maximum length of thread name, including '\ 0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_addr_t)&(((type*)0)->mbr)))
#define SLEEP_TIMEOUT_MAX 0x7fffffff


#define F_THREAD_POOL (0x01 << 0) //Mark whether the thread object is allocated through the memory pool
#define IS_F_THREAD_POOL(thread) ((thread->flag & F_THREAD_POOL) == F_THREAD_POOL)
#define SET_F_THREAD_POOL(thread) (thread->flag |= F_THREAD_POOL)
#define CLR_F_THREAD_POOL(thread) (thread->flag &= (~F_THREAD_POOL))

#define F_THREAD_STKPOOL (0x01 << 1) //Mark whether the stack of thread object is allocated through memory pool
#define IS_F_THREAD_STKPOOL(thread) ((thread->flag & F_THREAD_STKPOOL) == F_THREAD_STKPOOL)
#define SET_F_THREAD_STKPOOL(thread) (thread->flag |= F_THREAD_STKPOOL)
#define CLR_F_THREAD_STKPOOL(thread) (thread->flag &= (~F_THREAD_STKPOOL))

#define F_THREAD_NO_KILL (0x01 << 2) //Mark whether the thread object can be killed
#define IS_F_THREAD_NO_KILL(thread) ((thread->flag & F_THREAD_NO_KILL) == F_THREAD_NO_KILL)
#define SET_F_THREAD_NO_KILL(thread) (thread->flag |= F_THREAD_NO_KILL)
#define CLR_F_THREAD_NO_KILL(thread) (thread->flag &= (~F_THREAD_NO_KILL))

#define F_THREAD_DAEMON (0x01 << 3) //Mark whether the thread object is added to the background guard
#define IS_F_THREAD_DAEMON(thread) ((thread->flag & F_THREAD_DAEMON) == F_THREAD_DAEMON)
#define SET_F_THREAD_DAEMON(thread) (thread->flag |= F_THREAD_DAEMON)
#define CLR_F_THREAD_DAEMON(thread) (thread->flag &= (~F_THREAD_DAEMON))

#define F_THREAD_SYSTEM (0x01 << 4) //Mark whether thread is a system thread
#define IS_F_THREAD_SYSTEM(thread) ((thread->flag & F_THREAD_SYSTEM) == F_THREAD_SYSTEM)
#define SET_F_THREAD_SYSTEM(thread) (thread->flag |= F_THREAD_SYSTEM)
#define CLR_F_THREAD_SYSTEM(thread) (thread->flag &= (~F_THREAD_SYSTEM))

//Thread status enumeration value
typedef enum __w_thread_stat_e
{
    THREAD_STATUS_INIT = 0, //Initialization status
    THREAD_STATUS_READY = 1,//Ready to run
    THREAD_STATUS_SUSPEND,  //Blocking state, need to be waked up by blocking source
    THREAD_STATUS_SLEEP,    //Sleep state, can be awakened by timeout
    THREAD_STATUS_DEAD,     //Dead state, will never be awakened and dispatched
}w_thread_stat_e;


//Reasons for thread state changes
typedef enum __suscause
{
    CAUSE_NONE = 0, //Caused by nothing
    CAUSE_COMMON,   //Caused by common reason
    CAUSE_SLEEP,    //caused by sleep
    CAUSE_SEM,      //Caused by semaphore
    CAUSE_MSG,      //Caused by message
    CAUSE_LOCK,     //Caused by mutex
    CAUSE_CNT       //The reason count
}w_suscause_e;

typedef struct _w_thread_s w_thread_s;

#if 0
typedef enum __thr_evt_e
{
    THR_EVT_CREATE,
    THR_EVT_START,
    THR_EVT_SUSPEND,
    THR_EVT_RESUME,
    THR_EVT_DEAD
}w_thr_evt_e;
typedef struct __w_thrparam_s
{
    const char *name;
    w_err_t (*thread_func)(w_int32_t argc,char **argv);
    w_int16_t argc;
    char **argv;
    w_int16_t prio;
    w_stack_t *pstk;
    w_uint16_t stksize;
}w_thrparam_s;
#endif


//Thread object structure
struct _w_thread_s
{
    w_uint32_t magic;       //magic code,for checking thread valid
    w_prinode_s validnode;  //Node for adding thread to all thread list 
    w_prinode_s suspendnode;//Node for adding thread to suspend list 
    w_prinode_s sleepnode;  //Node for adding thread to sleep list 
    w_stack_t *stack_cur;   //Current stack pointer
    w_stack_t *stack_start; //Stack top pointer (decrement stack)
    w_uint32_t stksize;     //Stack size (in stack width)
    
    w_int16_t argc;         //Number of thread parameters
    w_int16_t flag;         //Thread attribute tag
    
    char **argv;            //Thread parameter vector
    w_err_t (*thread_func)(w_int32_t argc,char **argv);//Thread function
    
    char *name;             //Thread name
    w_int16_t prio;         //thread priority 
    w_uint16_t tid;         //Thread ID
    w_uint32_t run_times;   //Number of thread schedules
    w_thread_stat_e runstat;//Thread running status
    w_int32_t sleep_ticks;  //Tick value when thread sleep
    w_suscause_e cause;     //Causes of state changes
    w_dlist_s coroutlist;   //Coroutine list
    w_obj_s *corout;        //Current Coroutine

};

#define PRIDNODE_TO_THREAD(dnode,mbrnode) (w_thread_s*)(((char*)(dnode))-((w_addr_t)&(((w_thread_s*)0)->mbrnode.dnode)))


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
                    w_stack_t *psck,
                    w_uint32_t stksize);

w_thread_s *wind_thread_create(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,char **argv),
                    w_int16_t argc,
                    char **argv,
                    w_stack_t *psck,
                    w_uint32_t stksize);

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

w_err_t wind_thread_setflag(w_thread_s *thread,w_int16_t flag);
w_err_t wind_thread_clrflag(w_thread_s *thread,w_int16_t flag);

w_err_t wind_thread_sleep(w_int32_t ms);
w_err_t wind_thread_exit(w_err_t exitcode);

w_err_t wind_thread_print_list(void);
w_err_t wind_thread_print_detail(void);

w_err_t wind_thread_print_stack(w_thread_s *thread);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //WIND_THREADESS_H_
