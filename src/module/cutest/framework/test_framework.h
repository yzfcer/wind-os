/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_framework.h / test_framework.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 15:42:27
** 描        述: wind_os的系统的测试框架文件，在框架下继承了各种功能的单元测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
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
#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__
/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_debug.h"
//#include "test_port.h"
#if WIND_MODULE_CUTEST_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus



/***********************************************宏定义*************************************************/
#define TEST_CASE_NAME_LEN 24
#define TEST_SUITE_NAME_LEN 24
#define TEST_FAIL_LIST_CNT 100
extern void test_suite_err(w_uint32_t line);

#define FAIL_EXPECT_FALSE_OUT(x,res) test_printf("failed expection:FUNCTION:%s,LINE:%d:which expected "#x" is %s\r\n",__FUNCTION__,__LINE__,res)

#define FAIL_EXPECT_OUT(x,y,than) test_printf("failed expection:FUNCTION:%s,LINE:%d:which expected "#x" %s "#y"\r\n",__FUNCTION__,__LINE__,than)


//#define EXPECT_EQ(x,y) test_printf("FILE:%s,LINE:%d,expected %s,in fact %s\r\n",__FILE__,__LINE__,#x,#y)

/**********************************************枚举定义************************************************/


#ifndef W_NULL
#define W_NULL (void*)0
#endif
#define err_t w_uint32_t
#define ERR_OK 0
#define ERR_FAIL -1
#define TEST_ASSERT_RETURN(x,y)
#define test_printf wind_printf

/*********************************************结构体定义***********************************************/
//全局的test suite列表
typedef struct __w_test_case_s w_test_case_s;
typedef struct __w_test_suite_s w_test_suite_s;
typedef struct __w_suite_list_s w_suite_list_s;
typedef struct __w_test_stati_s w_test_stati_s;
typedef struct __w_fail_info_s w_fail_info_s;
typedef struct __w_stati_info_s w_stati_info_s;

//测试用例结构
struct __w_test_case_s
{
    char name[TEST_CASE_NAME_LEN];
    w_err_t (*setup)(void);
    w_err_t (*teardown)(void);
    void (*test)(void);
};

//测试套结构
struct __w_test_suite_s
{
    char name[TEST_SUITE_NAME_LEN];
    w_uint32_t case_cnt;//测试用例的数量
    w_test_case_s *tcase;
    //void (*init)(void);
    w_err_t (*setup)(void);
    w_err_t (*teardown)(void);
    w_test_suite_s *next;
};


struct __w_suite_list_s
{
    w_test_suite_s *head;
    w_test_suite_s *tail;
    w_uint32_t cnt;
};

//执行失败的信息
struct __w_fail_info_s
{
    w_test_suite_s *suite;
    w_test_case_s *tcase;
    w_uint32_t line;
    w_fail_info_s *next;
};

//用例执行结果统计信息
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
    //统计信息
    w_test_stati_s stat;

    //错误信息列表
    w_fail_info_s *failhead;
    w_fail_info_s *lastfail;
    w_uint32_t failcnt;

    //当前测试信息
    w_test_suite_s *suite;
    w_test_case_s *tcase;
    w_uint32_t case_err;
    w_uint32_t suite_err;
};

/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/
static w_int32_t stringlenth(char *str);
w_int32_t stringcmp(const char *cs,const char *ct);


err_t test_suite_register(w_test_suite_s *test_suite);
void show_test_suites(void);
w_err_t cutest_main(w_int32_t argc,char **argv);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif 
#endif 
