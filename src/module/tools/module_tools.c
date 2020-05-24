/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: module_tools.c
** **Author: Jason Zhou
** Last Date: 2018/11/09 20:24:37
** Description: 创建文件系统命令模块
**  
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2018/11/09 20:24:37
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"

#if WIND_MODULE_TOOLS_SUPPORT


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
    return W_ERR_OK;
}

MODULE_DEF(tools,0x0100,"shell");

#endif

