/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_port.h / test_port.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 15:42:27
** Description : wind_os的系统的测试框架移植接口
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 15:42:27
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_cut.h"


#if WIND_MODULE_CUTEST_SUPPORT



#if TEST_POOL_SUPPORT
DECLARE_SUITE(pool);
#endif
#if TEST_TREE_SUPPORT
DECLARE_SUITE(tree);
#endif
#if TEST_MUTEX_SUPPORT
DECLARE_SUITE(mutex);
#endif
#if TEST_SEM_SUPPORT
DECLARE_SUITE(sem);
#endif
#if TEST_EVENT_SUPPORT
DECLARE_SUITE(event);
#endif
#if TEST_PIPE_SUPPORT
DECLARE_SUITE(pipe);
#endif
#if TEST_MSGBOX_SUPPORT
DECLARE_SUITE(msgbox);
#endif
#if TEST_HEAP_SUPPORT
DECLARE_SUITE(heap);
#endif
#if TEST_SOFTIRQ_SUPPORT
DECLARE_SUITE(softirq);
#endif
#if TEST_TREEFS_SUPPORT
DECLARE_SUITE(treefs);
#endif
#if TEST_LISTFS_SUPPORT
DECLARE_SUITE(listfs);
#endif
#if TEST_HOSTFS_SUPPORT
DECLARE_SUITE(hostfs);
#endif
#if TEST_FS_SUPPORT
DECLARE_SUITE(fs);
#endif
#if TEST_FILEPATH_SUPPORT
DECLARE_SUITE(filepath);
#endif
#if TEST_DIAGNOSE_SUPPORT
DECLARE_SUITE(diagnose);
#endif
#if TEST_DICT_SUPPORT
DECLARE_SUITE(dict);
#endif
#if TEST_XML_SUPPORT
DECLARE_SUITE(xml);
#endif


extern void test_suite_list_init(void);
void test_suite_register_all(void)
{
    test_suite_list_init();

#if TEST_POOL_SUPPORT
    REGSTER_SUITE(pool);
#endif
#if TEST_TREE_SUPPORT
    REGSTER_SUITE(tree);
#endif
#if TEST_MUTEX_SUPPORT
    REGSTER_SUITE(mutex);
#endif
#if TEST_SEM_SUPPORT
    REGSTER_SUITE(sem);
#endif
#if TEST_EVENT_SUPPORT
    REGSTER_SUITE(event);
#endif
#if TEST_PIPE_SUPPORT
    REGSTER_SUITE(pipe);
#endif
#if TEST_MSGBOX_SUPPORT
    REGSTER_SUITE(msgbox);
#endif
#if TEST_HEAP_SUPPORT
    REGSTER_SUITE(heap);
#endif
#if TEST_SOFTIRQ_SUPPORT
    REGSTER_SUITE(softirq);
#endif
#if TEST_TREEFS_SUPPORT
    REGSTER_SUITE(treefs);
#endif
#if TEST_LISTFS_SUPPORT
    REGSTER_SUITE(listfs);
#endif
#if TEST_HOSTFS_SUPPORT
    REGSTER_SUITE(hostfs);
#endif

#if TEST_FS_SUPPORT
    REGSTER_SUITE(fs);
#endif
#if TEST_FILEPATH_SUPPORT
    REGSTER_SUITE(filepath);
#endif
#if TEST_DIAGNOSE_SUPPORT
    REGSTER_SUITE(diagnose);
#endif
#if TEST_DICT_SUPPORT
    REGSTER_SUITE(dict);
#endif
#if TEST_XML_SUPPORT
    REGSTER_SUITE(xml);
#endif
}

#endif

