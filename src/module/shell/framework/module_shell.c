/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: module_shell.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: shellģ�����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2015/1/24 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"
#include "wind_daemon.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_SHELL_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/

static w_stack_t ctrlstk[THREAD_SHELL_STKSIZE];//�������ջ
extern w_err_t thread_shell(w_int32_t argc,char **argv);

w_err_t _create_thread_shell(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("shell",thread_shell,
               0,W_NULL,ctrlstk,THREAD_SHELL_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread, F_THREAD_DAEMON | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,32760);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("shell") == W_NULL)
        wind_daemon_create("shell",_create_thread_shell);
#endif
    return W_ERR_OK;
}


MODULE_INIT(shell)
{
    wind_cmd_init();
    return _create_thread_shell();
}

MODULE_EXIT(shell)
{
    return W_ERR_OK;
}

MODULE_DEF(shell, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
