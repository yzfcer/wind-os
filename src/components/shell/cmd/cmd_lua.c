/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_lua.c
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
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_LUA_SUPPORT)

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(lua)
{
    wind_printf("to enter lua shell mode.\r\n");
}

COMMAND_USAGE(lua)
{
    wind_printf("lua:--to run into lua application.\r\n");
}

extern int lua_main (int argc, char **argv);
COMMAND_MAIN(lua,argc,argv)
{
    w_int32_t ret;
    ret = lua_main (argc, argv);
    return ret == 0?W_ERR_OK:W_ERR_FAIL;
}

COMMAND_DEF(lua);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
