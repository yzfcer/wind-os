/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: snake_module.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: 贪吃蛇游戏
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
#include "wind_std.h"
#include "wind_cmd.h"
#include "wind_module.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_ECHO_SUPPORT)

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(snake)
{
    wind_printf("[WIN32] to play snakes war game.\r\n");
}

COMMAND_USAGE(snake)
{
    wind_printf("snake:--to start snakes war game.\r\n");
}
extern int snake_main(int argc,char **argv);
COMMAND_MAIN(snake,argc,argv)
{
    snake_main(argc,argv);
    _wind_std_init();
    return W_ERR_OK;
}
COMMAND_DEF(snake);


MODULE_INIT(snake)
{
    return wind_cmd_register(COMMAND(snake), 1);
}

MODULE_EXIT(snake)
{
    return wind_cmd_unregister(COMMAND(snake));
}

MODULE_DEF(snake, 0x0100,"shell");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
