/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_test.c
**创   建   人: 周江村
**最后修改日期: 2013.11.06
**描        述: 系统的控制台命令test处理函数，提供一些系统内核测试的命令的响应函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.11.06
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.11.06
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_softirq.h"
#include "wind_heap.h"
#include "cut.h"

#include "wind_cmd.h"
#include "test_framework.h"

#if WIND_CONSOLE_SUPPORT

w_err_t cmd_testheap_main(w_int32_t argc,char **argv)
{
#if WIND_HEAP_SUPPORT
    if(0 == wind_strcmp(argv[0],"heap"))
    {
        wind_printf("heaptest not support yet\r\n");
        return ERR_OK;
    }
#endif
    return ERR_COMMAN;
}

COMMAND_DISC(test)
{
    wind_printf("to test some test cases.\r\n");
}

COMMAND_USAGE(test)
{
    wind_printf("test show:to show all test suites and cases list.\r\n");
    wind_printf("test <suitename> <casename>:to test some appointed test cases.\r\n");
    wind_printf("suitename:use *to test all test suite.\r\n");
    wind_printf("casename:use *to test all test suite.\r\n");
}

COMMAND_MAIN(test,argc,argv)
{
    if(0 == wind_strcmp(argv[1],"heap"))
    {
        return cmd_testheap_main(argc,argv);
    }
    else if(0 == wind_strcmp(argv[1],"show"))
    {
        show_test_suites();
    }
    else if(argc >= 3)
    {
        cut_test_start(argv[1],argv[2]);
    }
    return  ERR_COMMAN;
}

COMMAND_DEF(test);

void wind_cmd_register_cmd_test(console_s *ctrl)
{
    wind_cmd_register(COMMAND(test),1);
}


#endif
