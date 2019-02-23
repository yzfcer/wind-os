/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_cutest.c
**创   建   人: Jason Zhou
**最后修改日期: 2013.11.06
**描        述: 系统的控制台命令test处理函数，提供一些系统内核测试的命令的响应函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2013.11.06
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
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
#include "wind_cut.h"

#include "wind_cmd.h"
#include "test_framework.h"

#if (WIND_CONSOLE_SUPPORT && CMD_CUTEST_SUPPORT)

COMMAND_DISC(cutest)
{
    wind_printf("to test some test cases.\r\n");
}

COMMAND_USAGE(cutest)
{
    wind_printf("cutest list:--to show all test suites and cases list.\r\n");
    wind_printf("cutest <suitename> <casename>:--to test some appointed test cases.\r\n");
    wind_printf("       suitename:--use *to test all test suite.\r\n");
    wind_printf("       casename:--use *to test all test suite.\r\n");
}

COMMAND_MAIN(cutest,argc,argv)
{
    return cutest_main(argc,argv);
}

COMMAND_DEF(cutest);


#endif
