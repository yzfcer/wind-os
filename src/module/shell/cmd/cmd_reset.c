/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: cmd_reset.c
**Author: Jason Zhou
**Last Date: 
**Description: 重启系统命令
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
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
