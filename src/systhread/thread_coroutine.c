/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: thread_coroutine.c
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: Э�̵����߳�
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
#include "wind_thread.h"
#include "wind_daemon.h"
#include "wind_coroutine.h"
#include "wind_debug.h"

#if WIND_COROUTINE_SUPPORT
static w_stack_t corout_stack[THREAD_COROUT_STKSIZE];
static w_err_t coroute_main(w_int32_t argc,char **argv)
{
    w_err_t err;
    while(W_TRUE)
    {
        err= _wind_coroutine_dispatch();
        if(err != W_ERR_OK)
            wind_thread_sleep(1);
    }
}

w_err_t _create_coroutine_thread(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("coroutine",coroute_main,0,(char**)W_NULL,corout_stack,THREAD_COROUT_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("coroutine") == W_NULL)
        wind_daemon_create("coroutine",_create_coroutine_thread);
#endif
    return W_ERR_OK;
}
#endif

