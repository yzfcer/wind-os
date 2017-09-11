#ifndef __CUT_H__
#define __CUT_H__
#include "test_framework.h"

//¶ÏÑÔx == y
#define EXPECT_EQ(x,y) \
    do{if((x) != (y)) \
    {FAIL_EXPECT_OUT(x,y,"==");test_suite_err(__LINE__);}\
    }while(0)

//¶ÏÑÔx != y
#define EXPECT_NE(x,y) \
    do{if((x) == (y)) \
    {FAIL_EXPECT_OUT(x,y,"!=");test_suite_err(__LINE__);}\
    }while(0)

//¶ÏÑÔx > y
#define EXPECT_GREATER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,">");test_suite_err(__LINE__);}\
    }while(0)

//¶ÏÑÔx < y
#define EXPECT_SMALLER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,"<");test_suite_err(__LINE__);}\
    }while(0)

//¶ÏÑÔx <= y
#define EXPECT_NOT_GREATER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,"<=");test_suite_err(__LINE__);}\
    }while(0)

//¶ÏÑÔx >= y
#define EXPECT_NOT_SMALLER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,">=");test_suite_err(__LINE__);}\
    }while(0)



#define SUITE_SETUP()  static void suite_setup(void)
#define SUITE_TEARDOWN() static void suite_teardown(void)

#define CASE_SETUP(testcase) static void case_setup_##testcase(void)
#define CASE_TEARDOWN(testcase) static void case_teardown_##testcase(void)
#define CASE_FUNC(testcase) static void case_func_##testcase(void)

#define TEST_CASE_START static test_case_s testcase[] = {
#define TEST_CASE(tc) {#tc,case_setup_##tc,case_teardown_##tc,case_func_##tc},
#define TEST_CASE_END };

#define TEST_SUITE(ts) test_suite_s suite_##ts = {#ts,\
sizeof(testcase)/sizeof(test_case_s),\
testcase,\
suite_setup,\
suite_teardown,\
NULL};
    
#define DECLARE_SUITE(ts) extern test_suite_s suite_##ts
#define REGSTER_SUITE(ts) test_suite_register(&suite_##ts)



#endif
