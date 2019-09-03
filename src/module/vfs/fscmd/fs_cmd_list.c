/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: fs_cmd_list.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/5/13 20:24:37
** 描        述: 文件系统操作命令集注册接口
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/5/13 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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
