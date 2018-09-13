/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_cut.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: �û�ʹ�õĲ��Կ��ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_CUT_H__
#define WIND_CUT_H__
#include "test_framework.h"

//����x == y
#define EXPECT_EQ(x,y) \
    do{if((x) != (y)) \
    {FAIL_EXPECT_OUT(x,y,"==");test_suite_err(__LINE__);}\
    }while(0)

//����x != y
#define EXPECT_NE(x,y) \
    do{if((x) == (y)) \
    {FAIL_EXPECT_OUT(x,y,"!=");test_suite_err(__LINE__);}\
    }while(0)

//����x > y
#define EXPECT_GREATER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,">");test_suite_err(__LINE__);}\
    }while(0)

//����x < y
#define EXPECT_SMALLER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,"<");test_suite_err(__LINE__);}\
    }while(0)

//����x <= y
#define EXPECT_NOT_GREATER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,"<=");test_suite_err(__LINE__);}\
    }while(0)

//����x >= y
#define EXPECT_NOT_SMALLER(x,y) \
    do{if((x) <= (y)) \
    {FAIL_EXPECT_OUT(x,y,">=");test_suite_err(__LINE__);}\
    }while(0)

//�����ַ�����ͬ
#define EXPECT_STR_EQ(x,y) \
    do{if(stringcmp((x),(y)) != 0) \
    {FAIL_EXPECT_OUT(x,y,"!=");test_suite_err(__LINE__);}\
    }while(0)


#define SUITE_SETUP(testsuite)  static void suite_setup_##testsuite(void)
#define SUITE_TEARDOWN(testsuite) static void suite_teardown_##testsuite(void)

#define CASE_SETUP(testcase) static void case_setup_##testcase(void)
#define CASE_TEARDOWN(testcase) static void case_teardown_##testcase(void)
#define CASE_FUNC(testcase) static void case_func_##testcase(void)

#define TEST_CASES_START(testsuite) static w_test_case_s test_cases_##testsuite[] = {
#define TEST_CASE(tc) {#tc,case_setup_##tc,case_teardown_##tc,case_func_##tc},
#define TEST_CASES_END };

#define TEST_SUITE(ts) w_test_suite_s suite_##ts = {#ts,\
sizeof(test_cases_##ts)/sizeof(w_test_case_s),\
test_cases_##ts,\
suite_setup_##ts,\
suite_teardown_##ts,\
W_NULL};
    
#define DECLARE_SUITE(ts) extern w_test_suite_s suite_##ts
#define REGSTER_SUITE(ts) test_suite_register(&suite_##ts)



#endif
