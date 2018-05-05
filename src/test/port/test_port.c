#include "test_port.h"
#include "wind_cut.h"

//DECLARE_SUITE(TestSuite1);
//DECLARE_SUITE(TestSuite2);
DECLARE_SUITE(test_mpool);
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

void test_suite_register_all(void)
{
    //REGSTER_SUITE(TestSuite1);
    //REGSTER_SUITE(TestSuite2);
    REGSTER_SUITE(test_mpool);
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
}



