/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: module_cutest.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: cutest模块入口
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
#include "test_framework.h"
#include "wind_module.h"
#include "wind_cmd.h"
#include "wind_conv.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (WIND_MODULE_CUTEST_SUPPORT)

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(cutest)
{
    wind_printf("to test some test cases.\r\n");
}

COMMAND_USAGE(cutest)
{
    wind_printf("cutest list:--to show all test suites and cases list.\r\n");
    wind_printf("cutest <suitename> <casename> [times]:--to test some appointed test cases.\r\n");
    wind_printf("       suitename:--use *to test all test suite.\r\n");
    wind_printf("       casename:--use *to test all test suite.\r\n");
}

COMMAND_MAIN(cutest,argc,argv)
{
    w_err_t err;
    w_int32_t i,times = 1;
    if(argc >= 4)
    {
        wind_str_to_int(argv[3],&times);
        times = times < 1?1:times;
        times = times > 100?100:times;
    }
    
    for(i = 0;i < times;i ++)
    {
        err = cutest_main(argc,argv);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
    }
    return err;
}

COMMAND_DEF(cutest);




MODULE_INIT(cutest)
{
    w_err_t err = W_ERR_NOT_SUPPORT;
#if CMD_CUTEST_SUPPORT
    err = wind_cmd_register(COMMAND(cutest));
#endif
    return err;
}

MODULE_EXIT(cutest)
{
    w_err_t err = W_ERR_NOT_SUPPORT;
#if CMD_CUTEST_SUPPORT
    err = wind_cmd_unregister(COMMAND(cutest));
#endif
    return err;
}

MODULE_DEF(cutest, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
