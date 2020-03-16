/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_reset.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 重启系统命令
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_board_port.h"
#include "wind_thread.h"
#if (CMD_RESET_SUPPORT)

COMMAND_DISC(reset)
{
    wind_printf("to reset system.\r\n");
}

COMMAND_USAGE(reset)
{
    wind_printf("reset:--to reset system.\r\n");
}

COMMAND_MAIN(reset,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    wind_printf("system will reset now!\r\n");
    wind_thread_sleep(1000);
    wind_system_reset();
    while(1);
}

COMMAND_DEF(reset);
#endif
