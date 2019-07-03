/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: gobang_module.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: 五子棋游戏
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
COMMAND_DISC(gobang)
{
    wind_printf("[WIN32] to play gobang war game.\r\n");
}

COMMAND_USAGE(gobang)
{
    wind_printf("gobang:--to start gobang game.\r\n");
}
extern int gobang_main(int argc,char **argv);
COMMAND_MAIN(gobang,argc,argv)
{
    gobang_main(argc,argv);
    _wind_std_init();
    //while(wind_std_input((w_uint8_t *)&ch,1));
    return W_ERR_OK;
}
COMMAND_DEF(gobang);



MODULE_INIT(gobang)
{
    return wind_cmd_register(COMMAND(gobang), 1);
}

MODULE_EXIT(gobang)
{
    return wind_cmd_unregister(COMMAND(gobang));
}

MODULE_DEF(gobang, 0x0100,"shell");


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
