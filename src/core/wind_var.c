/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_var.h / wind_var.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os���ں���صı����ļ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.11.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
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
w_stack_t **gwind_high_stack;
//��ǰ�߳�ջָ��
w_stack_t **gwind_cur_stack;

void _wind_corevar_init(void)
{
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.usrthren = W_FALSE;
    g_core.ticks_cnt = 0;
    







#if WIND_FS_SUPPORT
    DLIST_INIT(g_core.fslist);
    DLIST_INIT(g_core.filelist);
#endif
    
}

