/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_tools.c
** Author      : Jason Zhou
** Last Date   : 2018/11/09 20:24:37
** Description : Tools module entry
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018/11/09 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"

#if WIND_MODULE_TOOLS_SUPPORT
#if CMD_SYSINFO_SUPPORT
COMMAND_DECLARE(sysinfo);
#endif
#if CMD_MKFS_SUPPORT
COMMAND_DECLARE(mkfs);
#endif
#if CMD_RCP_SUPPORT
COMMAND_DECLARE(rcp);
#endif
#if CMD_PACK_SUPPORT
COMMAND_DECLARE(pack);
#endif
#if CMD_EDSRC_SUPPORT
COMMAND_DECLARE(edsrc);
#endif


MODULE_INIT(tools)
{
#if CMD_MKFS_SUPPORT
    wind_cmd_register(COMMAND(mkfs));
#endif
#if CMD_RCP_SUPPORT
    wind_cmd_register(COMMAND(rcp));
#endif
#if CMD_PACK_SUPPORT
    wind_cmd_register(COMMAND(pack));
#endif
#if CMD_EDSRC_SUPPORT
    wind_cmd_register(COMMAND(edsrc));
#endif
    return W_ERR_OK;
}

MODULE_EXIT(tools)
{
#if CMD_MKFS_SUPPORT
   wind_cmd_unregister(COMMAND(mkfs));
#endif
#if CMD_RCP_SUPPORT
    wind_cmd_unregister(COMMAND(rcp));
#endif
#if CMD_PACK_SUPPORT
   wind_cmd_unregister(COMMAND(pack));
#endif
#if CMD_EDSRC_SUPPORT
   wind_cmd_unregister(COMMAND(edsrc));
#endif
    return W_ERR_OK;
}

MODULE_DEF(tools,0x0100,"shell");

#endif

