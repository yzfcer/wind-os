/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_framework.h / test_framework.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 15:42:27
** 描        述: 
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
** 描　述: wind_os的系统的测试框架文件，在框架下继承了各种功能的单元测试
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
#include "test_framework.h"



/********************************************内部变量定义**********************************************/



/********************************************内部函数定义*********************************************/

static ut_int32_t stringlenth(char *str)
{
    ut_int32_t i;
    for(i = 0;i < 65535;i ++)
    {
        if(str[i] == 0)
            break;
    }
    return i;
}

/********************************************全局变量定义**********************************************/
suite_list_s suite_list;
stati_info_s stati_info;
fail_info_s g_fail_obj[TEST_FAIL_LIST_CNT];
ut_uint32_t fail_index;

/********************************************全局函数定义**********************************************/
void stati_info_init(stati_info_s *sti)
{
    ut_uint32_t i;
    fail_info_s *fail;

    sti->case_err = 0;
    sti->suite_err = 0;
    
    sti->stat.failed_case = 0;
    sti->stat.passed_case = 0;
    sti->stat.failed_suite = 0;
    sti->stat.passed_suite = 0;
    
    sti->faillist = NULL;
    sti->lastfail = NULL;
    sti->failcnt = 0;
    fail_index = 0;
    for(i = 0;i < TEST_FAIL_LIST_CNT;i ++)
    {
        fail = &g_fail_obj[i];
        fail->line = 0;
        fail->next = NULL;
        fail->suite = NULL;
        fail->tcase = NULL;
    }
}
void test_framework_init(void)
{
    stati_info_s *sti;
    suite_list_s *tsg = &suite_list;

    sti = &stati_info;
    tsg->head = NULL;
    tsg->tail = NULL;
    tsg->cnt = 0;
    stati_info_init(sti);
}

err_t test_suite_register(test_suite_s *test_suite)
{
    suite_list_s *tsg = &suite_list;
    stati_info_s *sti;

    WIND_ASSERT_RETURN(test_suite == NULL,ERR_NULL_POINTER);
    sti = &stati_info;
    if(tsg->tail == NULL)
    {
        tsg->tail = test_suite;
        tsg->head = test_suite;
    }
    else
    {
        tsg->tail->next = test_suite;
        tsg->tail = test_suite;
    }
    test_suite->next = NULL;
    tsg->cnt ++;
    return ERR_OK;
}

void save_fail_info(ut_uint32_t line)
{
    stati_info_s *sti;
    fail_info_s *fail;
    sti = &stati_info;
    if(fail_index >= TEST_FAIL_LIST_CNT)
    {
        return;
    }
    fail = &g_fail_obj[fail_index ++];
    fail->suite = sti->suite;
    fail->tcase = sti->tcase;
    fail->line = line;
    if(sti->lastfail == NULL)
    {
        sti->faillist = fail;
        sti->lastfail = fail;
    }
    else
    {
        sti->lastfail->next = fail;
        sti->lastfail = fail;
    }
    fail->next = NULL;
    sti->failcnt ++;
}

void test_suite_err(ut_uint32_t line)
{
    test_suite_s *ts;
    test_case_s *tc;
    stati_info_s *sti;

    sti = &stati_info;
    ts = sti->suite;
    tc = sti->tcase;

    sti->suite_err ++;
    sti->case_err ++;
    save_fail_info(line);
}

void test_case_done(void)
{
    stati_info_s *sti;

    sti = &stati_info;
    if(sti->case_err > 0)
    {
        sti->stat.failed_case ++;
    }
    else
    {
        sti->stat.passed_case ++;
    }
    sti->case_err = 0;
}

void test_suite_done(void)
{
    stati_info_s *sti;

    sti = &stati_info;
    if(sti->suite_err > 0)
    {
        sti->stat.failed_suite ++;
    }
    else
    {
        sti->stat.passed_suite ++;
    }
    sti->suite_err = 0;

}
void print_boarder(ut_uint32_t space_cnt)
{
    char space[20];
    ut_uint32_t i,len;
    len = space_cnt > 20?20:space_cnt;
    for(i = 0;i < len;i ++)
        space[i] = ' ';
    for(;i < 20;i ++)
        space[i] = 0;

    TEST_STDOUT("------------");
    TEST_STDOUT("%s",space);
    TEST_STDOUT("------------");
    TEST_STDOUT("%s",space);
    TEST_STDOUT("------\r\n");
}

void print_header(ut_uint32_t space_cnt)
{
    char space[20];
    ut_uint32_t i,len;
    len = space_cnt > 20?20:space_cnt;
    for(i = 0;i < len;i ++)
        space[i] = ' ';
    for(;i < 20;i ++)
        space[i] = 0;

    TEST_STDOUT("%-12s","SUITE");
    TEST_STDOUT("%s",space);
    TEST_STDOUT("%-12s","CASE");
    TEST_STDOUT("%s",space);
    TEST_STDOUT("%-6s\r\n","LINE");
}

void print_fail_info(fail_info_s *fail,ut_uint32_t space_cnt)
{
    char space[20];
    ut_uint32_t i,len;
    len = space_cnt > 20?20:space_cnt;
    for(i = 0;i < len;i ++)
        space[i] = ' ';
    for(;i < 20;i ++)
        space[i] = 0;

    TEST_STDOUT("%-12s",fail->suite->name);
    TEST_STDOUT("%s",space);
    TEST_STDOUT("%-12s",fail->tcase->name);
    TEST_STDOUT("%s",space);
    TEST_STDOUT("%-6d\r\n",fail->line);
}

void test_framework_summit(void)
{
    stati_info_s *sti;
    ut_uint32_t i;
    fail_info_s *fail;
    ut_uint32_t space_cnt = 4;
    sti = &stati_info;
    TEST_STDOUT("\r\n\r\n[-----------ALL TEST SUMMARY-----------]\r\n");
    TEST_STDOUT("total  suites:%d\r\n",sti->stat.tot_suite);
    TEST_STDOUT("passed suites:%d\r\n",sti->stat.passed_suite);
    TEST_STDOUT("failed suites:%d\r\n",sti->stat.failed_suite);

    TEST_STDOUT("\r\ntotal  cases:%d\r\n",sti->stat.tot_case);
    TEST_STDOUT("passed cases:%d\r\n",sti->stat.passed_case);
    TEST_STDOUT("failed cases:%d\r\n",sti->stat.failed_case);

    if(sti->faillist != NULL)
    {
        TEST_STDOUT("\r\nfailture list as following:\r\n\r\n",sti->stat.tot_case);
        fail = sti->faillist;
        print_boarder(space_cnt);
        print_header(space_cnt);
        print_boarder(space_cnt);
        for(i = 0;i < sti->failcnt;i ++)
        {
            print_fail_info(fail,space_cnt);
            fail = fail->next;
            if(fail == NULL)
                break;
        }
        print_boarder(space_cnt);

    }
}

ut_int32_t do_match(char *str,char *filter,ut_int32_t idx,ut_int32_t len)
{
    ut_int32_t i;
    for(i = 0;i < len;i ++)
    {
        if(str[i + idx] != filter[i])
            break;
    }
    return i;
}

ut_int32_t is_contain(char *str,char *filter,ut_int32_t len1,ut_int32_t len2)
{
    ut_int32_t i;
    ut_int32_t idx;
    if(len2 > len1)
        return 0;
    for(i = 0;i < len1;i++)
    {
        if(len1 - i < len2)
            return 0;
        idx = do_match(str,filter,i,len2);
        if(idx >= len2)
            return 1;
    }
    return 0;
}

ut_int32_t do_match_end(char *str,char *filter,ut_int32_t len1,ut_int32_t len2)
{
    ut_int32_t i;
    if(len2 > len1)
        return 0;
    for(i = 0;i < len2;i ++)
    {
        if(str[len1 - len2 + i] != filter[i])
            return 0;
    }
    return 1;
}

ut_int32_t do_match_head(char *str,char *filter,ut_int32_t len1,ut_int32_t len2)
{
    ut_int32_t i;
    if(len2 > len1)
        return 0;
    for(i = 0;i < len2;i ++)
    {
        if(str[i] != filter[i])
            return 0;
    }
    return 1;
}


ut_int32_t do_match_all(char *str,char *filter,ut_int32_t len1,ut_int32_t len2)
{
    ut_int32_t i;
    if(len2 != len1)
        return 0;
    for(i = 0;i < len2;i ++)
    {
        if(str[i] != filter[i])
            return 0;
    }
    return 1;
}



ut_int32_t is_match_str(char *str,char *filter)
{
    ut_int32_t len1,len2;
    len1 = stringlenth(str);
    len2 = stringlenth(filter);

    if((filter[0] == '*') && (filter[len2 - 1] == '*'))
    {
        return is_contain(str,&filter[1],len1,len2 - 2);
    }
    else if(filter[0] == '*')
    {
        return do_match_end(str,&filter[1],len1,len2 - 1);
    }
    else if(filter[len2 - 1] == '*')
    {
        return do_match_head(str,&filter[0],len1,len2 - 1);
    }
    else
    {
        return do_match_all(str,filter,len1,len2);
    }
}
void show_test_suites(void)
{
    ut_uint32_t i,j = 0;
    test_suite_s *ts;
    TEST_STDOUT("\r\nTest Suites List As Following:\r\n");
    ts = suite_list.head;
    for(i = 0;i < suite_list.cnt;i ++)
    {
        TEST_STDOUT("%d--%s\r\n", 1+j ++,ts->name);
        ts = ts->next;
    }
}


static void execute_one_case(test_suite_s *ts,test_case_s *tc)
{
    stati_info_s *sti = &stati_info;
    
    sti->tcase = tc;
    TEST_STDOUT("\r\n[######  Test Case:%s  ######]\r\n",tc->name);
    tc->setup();
    tc->test();
    tc->teardown();
    test_case_done();
    TEST_STDOUT("\r\n");
    
}

static void execute_one_suite(test_suite_s *ts,char *casefilter)
{
    ut_uint32_t i;
    test_case_s *tc;
    stati_info_s *sti = &stati_info;
    sti->stat.tot_suite ++;
    sti->stat.tot_case += ts->case_cnt;
    TEST_STDOUT("\r\n[**************  Test Suite:%s  **************] \r\n",ts->name);
    sti->suite = ts;
    ts->setup();
    for(i = 0;i < ts->case_cnt;i ++)
    {
        tc = &ts->tcase[i];
        if(is_match_str(tc->name,casefilter))
            execute_one_case(ts,tc);
    }
    ts->teardown();
    test_suite_done();
    TEST_STDOUT("++++-----------------++++\r\n\r\n");
}

void execute_all_suites(char* suitefilter,char *casefilter)
{
    test_suite_s *ts;
    stati_info_s *sti;
    sti = &stati_info;
    ts = suite_list.head;
    stati_info_init(sti);
    TEST_STDOUT("-------Test framework start-------\r\n");
    while(ts)
    {
        if(is_match_str(ts->name,suitefilter))
            execute_one_suite(ts,casefilter);
        ts = ts->next;
    }
    TEST_STDOUT("-------Test framework end-------\r\n");
    test_framework_summit();
}


void cut_test_start(char* suitefilter,char *casefilter)
{
    test_framework_init();
    test_suite_register_all();
    show_test_suites();
    execute_all_suites(suitefilter,casefilter);
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
