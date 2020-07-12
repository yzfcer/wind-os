/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_framework.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 15:42:27
** Description : wind_os Unit test framework
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

#include "test_framework.h"
#if WIND_MODULE_CUTEST_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

void test_suite_register_all(void);




static w_suite_list_s suite_list;
w_stati_info_s stati_info;



w_int32_t stringlenth(char *str)
{
    w_int32_t i;
    for(i = 0;i < 65535;i ++)
    {
        if(str[i] == 0)
            break;
    }
    return i;
}

w_int32_t stringcmp(const char *dest,const char *src)
{
    char __res;
    while (1) {
        if ((__res = (char)(*dest - *src++)) != 0 || !*dest++)
            break;
    }
    return __res;
}





static void test_stati_init(w_test_stati_s *tst)
{
    int i;
    w_fail_info_s *fail;
    tst->tot_suite = 0;
    tst->tot_case = 0;
    tst->failed_case = 0;
    tst->passed_case = 0;
    tst->failed_suite = 0;
    tst->passed_suite = 0;
    
    for(i = 0;i < TEST_FAIL_LIST_CNT;i ++)
    {
        fail = &tst->fail_obj[i];
        fail->line = 0;
        fail->next = (w_fail_info_s*)W_NULL;
        fail->suite = (w_test_suite_s*)W_NULL;
        fail->tcase = (w_test_case_s*)W_NULL;
    }
}

static void stati_info_init(w_stati_info_s *sti)
{
    test_stati_init(&sti->stat);
    
    sti->failhead = (w_fail_info_s*)W_NULL;
    sti->lastfail = (w_fail_info_s*)W_NULL;
    sti->failcnt = 0;
    sti->suite = (w_test_suite_s*)W_NULL;
    sti->tcase = (w_test_case_s*)W_NULL;
    sti->case_err = 0;
    sti->suite_err = 0;
    
}

void test_suite_list_init(void)
{
    w_suite_list_s *tsl = &suite_list;
    tsl->head = (w_test_suite_s*)W_NULL;
    tsl->tail = (w_test_suite_s*)W_NULL;
    tsl->cnt = 0;
}


err_t test_suite_register(w_test_suite_s *test_suite)
{
    w_suite_list_s *tsl = &suite_list;
    TEST_ASSERT_RETURN(test_suite == W_NULL,W_ERR_NULL_PTR);
    if(tsl->tail == W_NULL)
    {
        tsl->tail = test_suite;
        tsl->head = test_suite;
    }
    else
    {
        tsl->tail->next = test_suite;
        tsl->tail = test_suite;
    }
    test_suite->next = (w_test_suite_s*)W_NULL;
    tsl->cnt ++;
    return W_ERR_OK;
}

static w_int32_t is_in_errlist(w_uint32_t line)
{
    w_stati_info_s *sti;
    w_fail_info_s *fail;
    w_int32_t match = 0;
    sti = &stati_info;
    if(sti->failhead == W_NULL)
        return 0;
    for(fail = sti->failhead;fail != W_NULL;fail = fail->next)
    {
        do 
        {
            if(fail->suite != sti->suite)
                break;
            if(fail->tcase != sti->tcase)
                break;
            if(fail->line != line)
                break;
            match = 1;
        }while(0);
    }
    if(match)
        return 1;
    return 0;
}

static void save_fail_info(w_uint32_t line)
{
    w_stati_info_s *sti;
    w_fail_info_s *fail;
    sti = &stati_info;
    if(sti->failcnt >= TEST_FAIL_LIST_CNT)
        return;
     if(is_in_errlist(line))
        return;
    fail = &sti->stat.fail_obj[sti->failcnt ++];//&g_fail_obj[sti->failcnt ++];
    fail->suite = sti->suite;
    fail->tcase = sti->tcase;
    fail->line = line;
    if(sti->lastfail == W_NULL)
    {
        sti->failhead = fail;
        sti->lastfail = fail;
    }
    else
    {
        sti->lastfail->next = fail;
        sti->lastfail = fail;
    }
    fail->next = (w_fail_info_s*)W_NULL;
}

void test_suite_err(w_uint32_t line)
{
    w_stati_info_s *sti;
    sti = &stati_info;
    sti->suite_err ++;
    sti->case_err ++;
    save_fail_info(line);
}

void test_case_done(void)
{
    w_stati_info_s *sti;

    sti = &stati_info;
    if(sti->case_err > 0)
    {
        sti->stat.failed_case ++;
    }
    else
    {
        sti->stat.passed_case ++;
    }
}

void test_suite_done(void)
{
    w_stati_info_s *sti;

    sti = &stati_info;
    if(sti->suite_err > 0)
    {
        sti->stat.failed_suite ++;
    }
    else
    {
        sti->stat.passed_suite ++;
    }
}



void print_header(w_uint32_t space_cnt)
{
    char space[20];
    w_uint32_t i,len;
    len = space_cnt > 20?20:space_cnt;
    for(i = 0;i < len;i ++)
        space[i] = ' ';
    for(;i < 20;i ++)
        space[i] = 0;

    test_printf("%-12s","SUITE");
    test_printf("%s",space);
    test_printf("%-12s","CASE");
    test_printf("%s",space);
    test_printf("%-6s\r\n","LINE");
}

void print_fail_info(w_fail_info_s *fail,w_uint32_t space_cnt)
{
    char space[20];
    w_uint32_t i,len;
    len = space_cnt > 20?20:space_cnt;
    for(i = 0;i < len;i ++)
        space[i] = ' ';
    for(;i < 20;i ++)
        space[i] = 0;

    test_printf("%-12s",fail->suite->name);
    test_printf("%s",space);
    test_printf("%-12s",fail->tcase->name);
    test_printf("%s",space);
    test_printf("%-6d\r\n",fail->line);
}

void test_framework_summit(void)
{
    w_stati_info_s *sti;
    w_uint32_t i;
    w_fail_info_s *fail;
    w_uint32_t space_cnt = 4;
    sti = &stati_info;
    test_printf("\r\n\r\n[************ALL TEST SUMMARY************]\r\n");
    test_printf("total  suites:%d\r\n",sti->stat.tot_suite);
    test_printf("passed suites:%d\r\n",sti->stat.passed_suite);
    test_printf("failed suites:%d\r\n",sti->stat.failed_suite);

    test_printf("\r\ntotal  cases:%d\r\n",sti->stat.tot_case);
    test_printf("passed cases:%d\r\n",sti->stat.passed_case);
    test_printf("failed cases:%d\r\n",sti->stat.failed_case);

    if(sti->failhead != W_NULL)
    {
        test_printf("\r\ntest failture list:\r\n",sti->stat.tot_case);
        fail = sti->failhead;
        test_printf("--------------------------------------\r\n");
        print_header(space_cnt);
        test_printf("--------------------------------------\r\n");
        for(i = 0;i < sti->failcnt;i ++)
        {
            print_fail_info(fail,space_cnt);
            fail = fail->next;
            if(fail == W_NULL)
                break;
        }
        test_printf("--------------------------------------\r\n");

    }
}

w_int32_t do_match(char *str,char *filter,w_int32_t idx,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        if(str[i + idx] != filter[i])
            break;
    }
    return i;
}

w_int32_t is_contain(char *str,char *filter,w_int32_t len1,w_int32_t len2)
{
    w_int32_t i;
    w_int32_t idx;
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

w_int32_t do_match_end(char *str,char *filter,w_int32_t len1,w_int32_t len2)
{
    w_int32_t i;
    if(len2 > len1)
        return 0;
    for(i = 0;i < len2;i ++)
    {
        if(str[len1 - len2 + i] != filter[i])
            return 0;
    }
    return 1;
}

w_int32_t do_match_head(char *str,char *filter,w_int32_t len1,w_int32_t len2)
{
    w_int32_t i;
    if(len2 > len1)
        return 0;
    for(i = 0;i < len2;i ++)
    {
        if(str[i] != filter[i])
            return 0;
    }
    return 1;
}


w_int32_t do_match_all(char *str,char *filter,w_int32_t len1,w_int32_t len2)
{
    w_int32_t i;
    if(len2 != len1)
        return 0;
    for(i = 0;i < len2;i ++)
    {
        if(str[i] != filter[i])
            return 0;
    }
    return 1;
}



w_int32_t is_match_str(char *str,char *filter)
{
    w_int32_t len1,len2;
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

void show_test_cases(w_test_suite_s *ts)
{
    w_uint32_t i;
    for(i = 0;i < ts->case_cnt;i ++)
    {
        test_printf("|   |----%s\r\n",ts->tcase[i].name);
    }
}

void show_test_suites(void)
{
    w_uint32_t i;
    w_test_suite_s *ts;
    test_printf("\r\nTest Suites List As Following:\r\n");
    test_printf("TestSuites\r\n");
    ts = suite_list.head;
    for(i = 0;i < suite_list.cnt;i ++)
    {
        test_printf("|---%s\r\n",ts->name);
        show_test_cases(ts);
        ts = ts->next;
    }
}


static void execute_one_case(w_test_suite_s *ts,w_test_case_s *tc)
{
    w_stati_info_s *sti = &stati_info;
    
    sti->tcase = tc;
    sti->stat.tot_case ++;
    sti->case_err = 0;
    test_printf("\r\n\r\n[++++++++++++ Test Case:%s ++++++++++++]\r\n",tc->name);
    tc->setup();
    tc->test();
    tc->teardown();
    test_case_done();
    test_printf("[++++++++++++ Test Case:%s ++++++++++++]\r\n\r\n",
                sti->case_err?"FAILED":"PASSED");
    
}

static void execute_one_suite(w_test_suite_s *ts,char *casefilter)
{
    w_uint32_t i;
    w_test_case_s *tc;
    w_stati_info_s *sti = &stati_info;
    sti->stat.tot_suite ++;
    sti->suite_err = 0;
    test_printf("\r\n\r\n[------------ Test Suite:%s ------------]\r\n",ts->name);
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
    test_printf("[------------ Test Suite:%s ------------]\r\n\r\n",
                sti->suite_err?"FAILED":"PASSED");
}

void execute_all_suites(char* suitefilter,char *casefilter)
{
    w_test_suite_s *ts;
    w_stati_info_s *sti;
    sti = &stati_info;
    ts = suite_list.head;
    stati_info_init(sti);
    test_printf("************Test framework start************\r\n");
    while(ts)
    {
        if(is_match_str(ts->name,suitefilter))
            execute_one_suite(ts,casefilter);
        ts = ts->next;
    }
    test_printf("*******Test framework end*******\r\n");
    test_framework_summit();
}


w_err_t cutest_main(w_int32_t argc,char **argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_FAIL);
    if(0 == stringcmp(argv[1],"list"))
    {
        show_test_suites();
        return W_ERR_OK;
    }
    else if(argc == 2)
    {
        execute_all_suites(argv[1],"*");
        return stati_info.stat.failed_suite == 0?W_ERR_OK:W_ERR_FAIL;
    }
    else if(argc >= 3)
    {
        execute_all_suites(argv[1],argv[2]);
        return stati_info.stat.failed_suite == 0?W_ERR_OK:W_ERR_FAIL;
    }
    return W_ERR_FAIL;
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
