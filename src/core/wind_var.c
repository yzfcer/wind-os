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
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_pipe.h"
#include "wind_msgbox.h"
#include "wind_sem.h"
#include "wind_mutex.h"
#include "wind_timer.h"
#include "wind_pool.h"
#include "wind_debug.h"
//wind-os�Ļ���ȫ�ֲ����͸����ں���Դ������ͷ
w_core_var_s g_core;
//wind-os��ʼ�����̵߳��ȵı��
volatile w_bool_t gwind_start_flag = W_FALSE;
//�����ȼ��߳�ջָ��
w_pstack_t *gwind_high_stack;
//��ǰ�߳�ջָ��
w_pstack_t *gwind_cur_stack;

void _wind_corevar_init(void)
{
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.usrthren = W_FALSE;
    //g_core.run_falg = W_FALSE;
    g_core.ticks_cnt = 0;
    
    DLIST_INIT(g_core.threadlist);
    DLIST_INIT(g_core.sleeplist);
    DLIST_INIT(g_core.poollist);
#if WIND_SEM_SUPPORT
    DLIST_INIT(g_core.semlist);
#endif
#if WIND_MUTEX_SUPPORT
    DLIST_INIT(g_core.mutexlist);
#endif
#if WIND_EVENT_SUPPORT
    DLIST_INIT(g_core.eventlist);
#endif
#if WIND_MSGBOX_SUPPORT
    DLIST_INIT(g_core.msgboxlist);
#endif
#if WIND_TIMER_SUPPORT
    DLIST_INIT(g_core.timerlist);
#endif
#if WIND_HEAP_SUPPORT
    DLIST_INIT(g_core.heaplist);
#endif
#if WIND_PIPE_SUPPORT
    DLIST_INIT(g_core.pipelist);
#endif
#if WIND_DRVFRAME_SUPPORT
    DLIST_INIT(g_core.devlist);
#endif
#if WIND_BLK_DRVFRAME_SUPPORT
    DLIST_INIT(g_core.blkdevlist);
#endif
#if WIND_FS_SUPPORT
    DLIST_INIT(g_core.fslist);
    DLIST_INIT(g_core.filelist);
#endif
    
}

