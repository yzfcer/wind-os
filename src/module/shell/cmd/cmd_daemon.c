/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_daemon.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2019/1/29 21:45:25
** ��        ��: daemon�����������
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019/1/29 21:45:25
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_daemon.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if (CMD_DAEMON_SUPPORT)

/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
COMMAND_DISC(daemon)
{
    wind_printf("to set a daemon object status.\r\n");
}

COMMAND_USAGE(daemon)
{
    wind_printf("daemon list:--to set a daemon status via str.\r\n");
    wind_printf("daemon enable <daemonname>:--to enable a daemon object.\r\n");
    wind_printf("daemon disable <daemonname>:--to disable a daemon object.\r\n");
}

COMMAND_MAIN(daemon,argc,argv)
{
    w_daemon_s *daemon;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
    {
        wind_daemon_print();
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"enable") == 0)
    {
        WIND_ASSERT_RETURN(argc == 3,W_ERR_INVALID);
        daemon = wind_daemon_get(argv[2]);
        WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_INVALID);
        wind_daemon_setflag(daemon,F_DAEMON_ENABLE);
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"disable") == 0)
    {
        WIND_ASSERT_RETURN(argc == 3,W_ERR_INVALID);
        daemon = wind_daemon_get(argv[2]);
        WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_INVALID);
        wind_daemon_clrflag(daemon,F_DAEMON_ENABLE);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(daemon);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
