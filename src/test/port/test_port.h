#ifndef __TEST_PORT_H__
#define __TEST_PORT_H__
#include <wind_debug.h>
#define test_printf wind_printf
typedef unsigned int ut_uint32_t;
typedef int ut_int32_t;
void test_suite_register_all(void);
#endif
