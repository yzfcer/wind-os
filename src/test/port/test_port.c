#include "test_port.h"
#include "cut.h"

DECLARE_SUITE(TestSuite1);
DECLARE_SUITE(TestSuite2);
void test_suite_register_all(void)
{
    REGSTER_SUITE(TestSuite1);
    REGSTER_SUITE(TestSuite2);
}



