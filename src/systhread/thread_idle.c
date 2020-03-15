/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: thread_idle.c
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: �����߳�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_thread.h"

static w_stack_t idlestk[THREAD_IDLE_STKSIZE];//���������ջ
static w_err_t thread_idle(w_int32_t argc,char **argv)
{    
    while(1)
    {
        g_core.idle_cnt ++;
    }
    return W_ERR_OK;
}


w_err_t _create_thread_idle(void)	
{
    w_thread_s *thread;
    thread = wind_thread_create("idle",thread_idle,
                    0,W_NULL,idlestk,THREAD_IDLE_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread,F_THREAD_NO_KILL | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,32767);
    return W_ERR_OK;
}

