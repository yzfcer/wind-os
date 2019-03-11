/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_thread.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: �߳���صĴ���ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
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
#define WIND_THREAD_STK_MARK 0x24681357
#define THREAD_NAME_LEN 20 //�߳�������󳤶ȣ����� '\0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))
#define SLEEP_TIMEOUT_MAX 0x7fffffff


#define F_THREAD_POOL (0x01 << 0) //���thread�����Ƿ�ͨ���ڴ�ط���
#define IS_F_THREAD_POOL(thread) ((thread->flag & F_THREAD_POOL) == F_THREAD_POOL)
#define SET_F_THREAD_POOL(thread) (thread->flag |= F_THREAD_POOL)
#define CLR_F_THREAD_POOL(thread) (thread->flag &= (~F_THREAD_POOL))

#define F_THREAD_STKPOOL (0x01 << 1) //���thread����Ķ�ջ�Ƿ�ͨ���ڴ�ط���
#define IS_F_THREAD_STKPOOL(thread) ((thread->flag & F_THREAD_STKPOOL) == F_THREAD_STKPOOL)
#define SET_F_THREAD_STKPOOL(thread) (thread->flag |= F_THREAD_STKPOOL)
#define CLR_F_THREAD_STKPOOL(thread) (thread->flag &= (~F_THREAD_STKPOOL))

#define F_THREAD_NO_KILL (0x01 << 2) //���thread������ܷ�ɱ��
#define IS_F_THREAD_NO_KILL(thread) ((thread->flag & F_THREAD_NO_KILL) == F_THREAD_NO_KILL)
#define SET_F_THREAD_NO_KILL(thread) (thread->flag |= F_THREAD_NO_KILL)
#define CLR_F_THREAD_NO_KILL(thread) (thread->flag &= (~F_THREAD_NO_KILL))

#define F_THREAD_DAEMON (0x01 << 3) //���thread�����Ƿ�����̨�ػ�
#define IS_F_THREAD_DAEMON(thread) ((thread->flag & F_THREAD_DAEMON) == F_THREAD_DAEMON)
#define SET_F_THREAD_DAEMON(thread) (thread->flag |= F_THREAD_DAEMON)
#define CLR_F_THREAD_DAEMON(thread) (thread->flag &= (~F_THREAD_DAEMON))

//#define F_THREAD_VIRTUAL (0x01 << 4) //���thread�����Ƿ����⻯
//#define IS_F_THREAD_VIRTUAL(thread) ((thread->flag & F_THREAD_VIRTUAL) == F_THREAD_VIRTUAL)
//#define SET_F_THREAD_VIRTUAL(thread) (thread->flag |= F_THREAD_VIRTUAL)
//#define CLR_F_THREAD_VIRTUAL(thread) (thread->flag &= (~F_THREAD_VIRTUAL))

//�߳�״̬�б�
typedef enum __w_thread_stat_e
{
    THREAD_STATUS_INIT = 0,//��ʼ��״̬
    THREAD_STATUS_READY = 1,//����״̬,����ʱ����
    THREAD_STATUS_SUSPEND,//����״̬����Ҫ������Դ����
    THREAD_STATUS_SLEEP,//����״̬���ɱ���ʱ����
    THREAD_STATUS_DEAD,//����״̬������Զ���ᱻ���Ѻ͵���
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

//�̶߳���ṹ
struct _w_thread_s
{
    w_uint32_t magic;
    w_prinode_s validnode;
    w_prinode_s suspendnode;
    w_prinode_s sleepnode;
    w_stack_t *stack_cur;//��ջָ��
    w_stack_t *stack_start;//ջ��ָ��
    w_uint32_t stksize;//��ջ��С����ջ��ȼ���
    
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
    w_suscause_e cause;//����״̬�仯��ԭ��

};

#define PRIDNODE_TO_THREAD(dnode,mbrnode) (w_thread_s*)(((char*)(dnode))-((w_uint32_t)&(((w_thread_s*)0)->mbrnode.dnode)))


w_err_t _wind_thread_mod_init(void);
w_err_t _wind_thread_wakeup(void);
void _wind_thread_set_usrmode(void);
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

w_err_t wind_thread_print(void);


#ifdef __cplusplus
}
#endif

#endif //WIND_THREADESS_H_
