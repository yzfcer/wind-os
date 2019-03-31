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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_cut.h"


#if WIND_CUTEST_SUPPORT



#if TEST_POOL_SUPPORT
DECLARE_SUITE(test_pool);
#endif
#if TEST_TREE_SUPPORT
DECLARE_SUITE(test_tree);
#endif
#if TEST_MUTEX_SUPPORT
DECLARE_SUITE(test_mutex);
#endif
#if TEST_SEM_SUPPORT
DECLARE_SUITE(test_sem);
#endif
#if TEST_EVENT_SUPPORT
DECLARE_SUITE(test_event);
#endif
#if TEST_PIPE_SUPPORT
DECLARE_SUITE(test_pipe);
#endif
#if TEST_MSGBOX_SUPPORT
DECLARE_SUITE(test_msgbox);
#endif
#if TEST_HEAP_SUPPORT
DECLARE_SUITE(test_heap);
#endif
#if TEST_SOFTIRQ_SUPPORT
DECLARE_SUITE(test_softirq);
#endif
#if TEST_TREEFS_SUPPORT
DECLARE_SUITE(test_treefs);
#endif
#if TEST_LISTFS_SUPPORT
DECLARE_SUITE(test_listfs);
#endif
#if TEST_FS_SUPPORT
DECLARE_SUITE(test_fs);
#endif

void test_suite_register_all(void)
{
#if TEST_POOL_SUPPORT
    REGSTER_SUITE(test_pool);
#endif
#if TEST_TREE_SUPPORT
    REGSTER_SUITE(test_tree);
#endif
#if TEST_MUTEX_SUPPORT
    REGSTER_SUITE(test_mutex);
#endif
#if TEST_SEM_SUPPORT
    REGSTER_SUITE(test_sem);
#endif
#if TEST_EVENT_SUPPORT
    REGSTER_SUITE(test_event);
#endif
#if TEST_PIPE_SUPPORT
    REGSTER_SUITE(test_pipe);
#endif
#if TEST_MSGBOX_SUPPORT
    REGSTER_SUITE(test_msgbox);
#endif
#if TEST_HEAP_SUPPORT
    REGSTER_SUITE(test_heap);
#endif
#if TEST_SOFTIRQ_SUPPORT
    REGSTER_SUITE(test_softirq);
#endif
#if TEST_TREEFS_SUPPORT
    REGSTER_SUITE(test_treefs);
#endif
#if TEST_LISTFS_SUPPORT
    REGSTER_SUITE(test_listfs);
#endif

#if TEST_FS_SUPPORT
    REGSTER_SUITE(test_fs);
#endif
}

#endif

