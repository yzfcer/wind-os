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

#define THREAD_NAME_LEN 20 //�߳�������󳤶ȣ����� '\0'
#define THREAD_FROM_MEMBER(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))
#define SLEEP_TIMEOUT_MAX 0x7fffffff
//�̵߳����ȵȼ���
typedef enum _PRIOLEVEL
{
    PRIO_ZERO,
    PRIO_HIGH,
    PRIO_MID,
    PRIO_LOW,
    PRIO_SYS_LOW
} prio_e;

//�߳�״̬�б�
typedef enum __thread_status
{
    THREAD_STATUS_INIT = 0,//��ʼ��״̬
    THREAD_STATUS_READY = 1,//����״̬,����ʱ����
    THREAD_STATUS_SUSPEND,//����״̬����Ҫ������Դ����
    THREAD_STATUS_SLEEP,//����״̬���ɱ���ʱ����
    THREAD_STATUS_DEAD,//����״̬������Զ���ᱻ���Ѻ͵���
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

//�������߳���ص�һЩ�ص���������Ҫ����ѡ��֧��
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


//�̶߳���ṹ
struct _thread_s
{
    prinode_s validthr;
    prinode_s suspendthr;
    prinode_s sleepthr;
    w_pstack_t stack;//��ջָ��
    w_pstack_t stack_top;//ջ��ָ��
    w_uint16_t stksize;//��ջ��С����ջ��ȼ���
    
    w_int16_t argc;
    w_int8_t **argv;
    w_err_t (*thread_func)(w_int32_t argc,w_int8_t **argv);
    

    char name[THREAD_NAME_LEN];
    w_int16_t prio;
    
    thread_stat_e runstat;
    w_int32_t sleep_ticks;
    suscause_e cause;//����״̬�仯��ԭ��
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
