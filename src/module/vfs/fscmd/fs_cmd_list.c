/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: fs_cmd_list.c
** **Author: Jason Zhou
** Last Date: 2018/5/13 20:24:37
** Description: 文件系统操作命令集注册接口
**  
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2018/5/13 20:24:37
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_MODULE_VFS_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_FS_SUPPORT
    COMMAND_DECLARE(mount);
    COMMAND_DECLARE(umount);
    COMMAND_DECLARE(format);
    COMMAND_DECLARE(cat);
    COMMAND_DECLARE(cd);
    COMMAND_DECLARE(ls);
    COMMAND_DECLARE(mkdir);
    COMMAND_DECLARE(pwd);
    COMMAND_DECLARE(rm);
    COMMAND_DECLARE(touch);
    COMMAND_DECLARE(write);
#endif


void _wind_vfs_register_fs_cmd(void)
{
#if CMD_FS_SUPPORT
    wind_cmd_register(COMMAND(mount));
    wind_cmd_register(COMMAND(umount));
    wind_cmd_register(COMMAND(format));
    wind_cmd_register(COMMAND(cat));
    wind_cmd_register(COMMAND(cd));
    wind_cmd_register(COMMAND(ls));
    wind_cmd_register(COMMAND(mkdir));
    wind_cmd_register(COMMAND(pwd));
    wind_cmd_register(COMMAND(rm));
    wind_cmd_register(COMMAND(touch));
    wind_cmd_register(COMMAND(write));
#endif
}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif
