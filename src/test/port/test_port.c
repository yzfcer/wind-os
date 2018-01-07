#include "test_port.h"
#include "cut.h"

DECLARE_SUITE(TestSuite1);
DECLARE_SUITE(TestSuite2);
DECLARE_SUITE(test_mpool);
DECLARE_SUITE(test_tree);
DECLARE_SUITE(test_lock);
DECLARE_SUITE(test_sem);

void test_suite_register_all(void)
{
    REGSTER_SUITE(TestSuite1);
    REGSTER_SUITE(TestSuite2);
    REGSTER_SUITE(test_mpool);
    REGSTER_SUITE(test_tree);
    REGSTER_SUITE(test_lock);
    REGSTER_SUITE(test_sem);
}



