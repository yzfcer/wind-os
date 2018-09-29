/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_thread.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: �߳���صĴ���ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
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
#define THREAD_NAME_LEN 20 //�߳�������󳤶ȣ����� '\0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))
#define SLEEP_TIMEOUT_MAX 0x7fffffff
//�̵߳����ȵȼ���
typedef enum _w_prio_e
{
    PRIO_ZERO,
    PRIO_HIGH,
    PRIO_MID,
    PRIO_LOW,
    PRIO_SYS_LOW
} w_prio_e;

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
typedef struct __w_threadcb_s w_threadcb_s;

//�������߳���ص�һЩ�ص���������Ҫ����ѡ��֧��
#if WIND_THREAD_CALLBACK_SUPPORT
struct __w_threadcb_s
{
    void (*start)(w_thread_s *thread);
    void (*suspend)(w_thread_s *thread);
    void (*resume)(w_thread_s *thread);
    void (*dead)(w_thread_s *thread);
};
#endif 

typedef enum __thr_evt_e
{
    THR_EVT_CREATE,
    THR_EVT_START,
    THR_EVT_SUSPEND,
    THR_EVT_RESUME,
    THR_EVT_DEAD
}w_thr_evt_e;


//�̶߳���ṹ
struct _w_thread_s
{
    w_uint32_t magic;
    w_prinode_s validnode;
    w_prinode_s suspendnode;
    w_prinode_s sleepnode;
    w_pstack_t stack;//��ջָ��
    w_pstack_t stack_top;//ջ��ָ��
    w_uint16_t stksize;//��ջ��С����ջ��ȼ���
    
    w_int16_t argc;
    w_uint16_t stkpool_flag:1;//����߳�ջ�Ƿ���ڴ��ȡ��
    w_uint16_t threadpool_flag:1;//����߳̽ṹ�Ƿ���ڴ�ػ�ȡ
    w_int8_t **argv;
    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv);
    
    char *name;
    w_int16_t prio;
    w_uint16_t tid;
    w_uint32_t run_times;
    w_thread_stat_e runstat;
    w_int32_t sleep_ticks;
    w_suscause_e cause;//����״̬�仯��ԭ��
#if WIND_THREAD_CALLBACK_SUPPORT
    w_threadcb_s cb;
#endif
};



w_err_t _wind_thread_mod_init(void);
w_err_t _wind_thread_wakeup(void);

w_thread_s *wind_thread_get(const char *name);
w_thread_s *wind_thread_current(void);
char *wind_thread_curname(void);

w_thread_s *wind_thread_init(w_thread_s *thread,
                    const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                    w_int16_t argc,
                    w_int8_t **argv,
                    w_prio_e priolevel,
                    w_pstack_t psck,
                    w_uint16_t stksize);

w_thread_s *wind_thread_create(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                    w_int16_t argc,
                    w_int8_t **argv,
                    w_prio_e priolevel,
                    w_pstack_t psck,
                    w_uint16_t stksize);

#if WIND_STKPOOL_SUPPORT
w_thread_s *wind_thread_create_default(const char *name,
                    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv),
                    w_int16_t argc,
                    w_int8_t **argv);
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

w_err_t wind_thread_print(w_dlist_s *list);
#if WIND_THREAD_CALLBACK_SUPPORT
w_err_t wind_thread_callback_register(w_thread_s *thread,w_thr_evt_e id,void(*cb)(w_thread_s *));
#endif

#ifdef __cplusplus
}
#endif

#endif //WIND_THREADESS_H_
