/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_var.h / wind_var.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os���ں���صı����ļ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.11.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2013.11.27
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_VAR_H__
#define WIND_VAR_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_dlist.h"
#include "wind_pipe.h"
#include "wind_msgbox.h"
#include "wind_sem.h"
#include "wind_mutex.h"
#include "wind_timer.h"
#include "wind_stati.h"
#include "wind_mpool.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct __core_var_s
{
    //�ں˶����б�
    dlist_s threadlist;
    dlist_s sleeplist;
    dlist_s semlist;
    dlist_s mutexlist;
    dlist_s msgboxlist;
    dlist_s ttmerlist;
    dlist_s poollist;
    dlist_s heaplist;
    dlist_s pipelist;
    dlist_s devlist;
    dlist_s blkdevlist;
    
    //volatile w_bool_t run_falg;//���̵߳��ȿ�ʼ�ı�־
    w_bool_t usrthren;/*�û��߳��������ı�־ */
    
    volatile w_uint32_t idle_cnt;//���м�����
    w_uint32_t ticks_cnt;//tick��ʱ
    w_uint32_t idle_cnt_max;//��һ��ʱ���ڵ�idle����ļ���ֵ
    w_uint32_t cpu_usage;

    //�߳����ö���
    thread_s *pmain;
    thread_s *pidle;
    thread_s *pstat;
    thread_s *pctrl;
    thread_s *pdaemon;
    thread_s *ptimer;
}core_var_s;
extern core_var_s g_core;//�ں���صĲ�����
extern volatile w_bool_t gwind_start_flag;//��ʼ���ȵı�־

extern w_pstack_t *gwind_high_stack;
extern w_pstack_t *gwind_cur_stack;

void _wind_corevar_init(void);

//CPU������ռ����
#define WIND_CPU_USAGE (g_core.cpu_usage)

#define IDLE_CNT_PER_SEC (g_core.idle_cnt_max)

#define RUN_FLAG (gwind_start_flag)

#define TICKS_CNT (g_core.ticks_cnt)


#ifdef __cplusplus
}
#endif


#endif

