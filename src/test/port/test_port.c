/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_port.h / test_port.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 15:42:27
** 描        述: wind_os的系统的测试框架移植接口
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2015/1/24 15:42:27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "test_port.h"
#include "wind_cut.h"

//DECLARE_SUITE(TestSuite1);
//DECLARE_SUITE(TestSuite2);
DECLARE_SUITE(test_pool);
DECLARE_SUITE(test_tree);
#if WIND_MUTEX_SUPPORT
DECLARE_SUITE(test_mutex);
#endif
#if WIND_SEM_SUPPORT
DECLARE_SUITE(test_sem);
#endif
#if WIND_PIPE_SUPPORT
DECLARE_SUITE(test_pipe);
#endif
#if WIND_MSGBOX_SUPPORT
DECLARE_SUITE(test_msgbox);
#endif
#if WIND_HEAP_SUPPORT
DECLARE_SUITE(test_heap);
#endif
#if WIND_SOFTIRQ_SUPPORT
DECLARE_SUITE(test_softirq);
#endif
#if WIND_FS_SUPPORT
//DECLARE_SUITE(test_treefs);
#endif

void test_suite_register_all(void)
{
    //REGSTER_SUITE(TestSuite1);
    //REGSTER_SUITE(TestSuite2);
    REGSTER_SUITE(test_pool);
    REGSTER_SUITE(test_tree);
#if WIND_MUTEX_SUPPORT
    REGSTER_SUITE(test_mutex);
#endif
#if WIND_SEM_SUPPORT
    REGSTER_SUITE(test_sem);
#endif
#if WIND_PIPE_SUPPORT
    REGSTER_SUITE(test_pipe);
#endif
#if WIND_MSGBOX_SUPPORT
    REGSTER_SUITE(test_msgbox);
#endif
#if WIND_HEAP_SUPPORT
    REGSTER_SUITE(test_heap);
#endif
#if WIND_SOFTIRQ_SUPPORT
    REGSTER_SUITE(test_softirq);
#endif
#if WIND_FS_SUPPORT
//    REGSTER_SUITE(test_treefs);
#endif
}



