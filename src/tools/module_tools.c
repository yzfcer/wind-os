/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: module_tools.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/11/09 20:24:37
** 描        述: 创建文件系统命令模块
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/11/09 20:24:37
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
#include "wind_module.h"



#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_TOOLS_SUPPORT
#if CMD_PACK_SUPPORT
w_int32_t pack_main(w_int32_t argc,char **argv);

COMMAND_DISC(pack)
{
    wind_printf("[*PC*] to package binary files to a image file.\r\n");
}

COMMAND_USAGE(pack)
{
    wind_printf("pack cfg <boardname>:--to pack binary files with configuration defined by boardname.cfg.\r\n");
}

COMMAND_MAIN(pack,argc,argv)
{
    return pack_main(argc,argv);
}
COMMAND_DEF(pack);
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

MODULE_DEF(tools, 0x0100,"shell");


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
