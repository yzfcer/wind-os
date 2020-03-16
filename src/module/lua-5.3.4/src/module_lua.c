/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: module_lua.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: lua脚本启动命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2015/1/24 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_debug.h"
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_LUA_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(lua)
{
    wind_printf("to enter lua shell mode.\r\n");
}

COMMAND_USAGE(lua)
{
    wind_printf("lua:--to run into lua application.\r\n");
    wind_printf("lua test:--to run a test lua script.\r\n");
    wind_printf("lua file <luafile>:--to run a lua script file.\r\n");
}

extern int lua_main (int argc, char **argv);
COMMAND_MAIN(lua,argc,argv)
{
    w_int32_t ret;
    ret = lua_main (argc, argv);
    return ret == 0?W_ERR_OK:W_ERR_FAIL;
}

COMMAND_DEF(lua);


//--------------------------------------------------------------------------
MODULE_INIT(lua)
{
    w_err_t err = W_ERR_NOT_SUPPORT;
    err = wind_cmd_register(COMMAND(lua));
    return err;
}

MODULE_EXIT(lua)
{
        w_err_t err = W_ERR_NOT_SUPPORT;
        err = wind_cmd_unregister(COMMAND(lua));
        return err;
}

MODULE_DEF(lua, 0x0100,"shell");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
