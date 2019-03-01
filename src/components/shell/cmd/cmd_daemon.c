/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_daemon.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2019/1/29 21:45:25
** 描        述: daemon对象操作命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019/1/29 21:45:25
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_daemon.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_DAEMON_SUPPORT)

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
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
