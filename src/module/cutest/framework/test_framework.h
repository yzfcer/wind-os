/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: test_framework.h / test_framework.c
** ** Author      : Jason Zhou
** Last Date: 2015/1/24 15:42:27
** Description : wind_os的系统的测试框架文件，在框架下继承了各种功能的单元测试
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
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__
/*********************************************header file***********************************************/
#include "wind_config.h"
#include "wind_debug.h"
//#include "test_port.h"
#if WIND_MODULE_CUTEST_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus



/***********************************************macros*************************************************/
#define TEST_CASE_NAME_LEN 24
#define TEST_SUITE_NAME_LEN 24
#define TEST_FAIL_LIST_CNT 100
extern void test_suite_err(w_uint32_t line);

#define FAIL_EXPECT_FALSE_OUT(x,res) test_printf("failed expection:FUNCTION:%s,LINE:%d:which expected "#x" is %s\r\n",__FUNCTION__,__LINE__,res)

#define FAIL_EXPECT_OUT(x,y,than) test_printf("failed expection:FUNCTION:%s,LINE:%d:which expected "#x" %s "#y"\r\n",__FUNCTION__,__LINE__,than)


//#define EXPECT_EQ(x,y) test_printf("FILE:%s,LINE:%d,expected %s,in fact %s\r\n",__FILE__,__LINE__,#x,#y)

/***********************************************enum*************************************************/


#ifndef W_NULL
#define W_NULL (void*)0
#endif
#define err_t w_uint32_t
#define ERR_OK 0
#define ERR_FAIL -1
#define TEST_ASSERT_RETURN(x,y)
#define test_printf wind_printf

/***********************************************struct*************************************************/
//全局的test suite列表
typedef struct __w_test_case_s w_test_case_s;
typedef struct __w_test_suite_s w_test_suite_s;
typedef struct __w_suite_list_s w_suite_list_s;
typedef struct __w_test_stati_s w_test_stati_s;
typedef struct __w_fail_info_s w_fail_info_s;
typedef struct __w_stati_info_s w_stati_info_s;

//testcase structure
struct __w_test_case_s
{
    char name[TEST_CASE_NAME_LEN];//testcase name
    w_err_t (*setup)(void);//setup function of the testcase
    w_err_t (*teardown)(void);//teardown function of the testcase
    void (*test)(void);//test function of the testcase
};

//test suite structure
struct __w_test_suite_s
{
    char name[TEST_SUITE_NAME_LEN];//test suite name
    w_uint32_t case_cnt;//testcase count
    w_test_case_s *tcase;//pointor to the first testcase of the suite
    //void (*init)(void);
    w_err_t (*setup)(void);//setup function of the test suite 
    w_err_t (*teardown)(void);//teardown function of the test suite 
    w_test_suite_s *next;//pointor to the next suite
};

//test suite list head
struct __w_suite_list_s
{
    w_test_suite_s *head;
    w_test_suite_s *tail;
    w_uint32_t cnt;
};

//执行失败的信息
//failed execution information
struct __w_fail_info_s
{
    w_test_suite_s *suite;
    w_test_case_s *tcase;
    w_uint32_t line;
    w_fail_info_s *next;
};

//testcases executing statistics result 
struct __w_test_stati_s
{
    w_uint32_t tot_suite;
    w_uint32_t tot_case;
    w_uint32_t passed_suite;
    w_uint32_t failed_suite;
    w_uint32_t passed_case;
    w_uint32_t failed_case;
    w_fail_info_s fail_obj[TEST_FAIL_LIST_CNT];
};




struct __w_stati_info_s
{
    //statistics information
    w_test_stati_s stat;

    //error information
    w_fail_info_s *failhead;
    w_fail_info_s *lastfail;
    w_uint32_t failcnt;

    //current test information
    w_test_suite_s *suite;
    w_test_case_s *tcase;
    w_uint32_t case_err;
    w_uint32_t suite_err;
};

/********************************************global variable declare**********************************************/



/********************************************global function declare**********************************************/
//static w_int32_t stringlenth(char *str);
w_int32_t stringcmp(const char *cs,const char *ct);


err_t test_suite_register(w_test_suite_s *test_suite);
void show_test_suites(void);
w_err_t cutest_main(w_int32_t argc,char **argv);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif 
#endif 
