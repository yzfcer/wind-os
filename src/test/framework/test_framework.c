/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_framework.h / test_framework.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2015/1/24 15:42:27
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2015/1/24 15:42:27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: wind_os��ϵͳ�Ĳ��Կ���ļ����ڿ���¼̳��˸��ֹ��ܵĵ�Ԫ����
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/
#include "test_framework.h"



/********************************************�ڲ���������**********************************************/
static suite_list_s suite_list;
stati_info_s stati_info;

/********************************************�ڲ���������*********************************************/

ut_int32_t stringlenth(char *str)
{
    ut_int32_t i;
    for(i = 0;i < 65535;i ++)
    {
        if(str[i] == 0)
            break;
    }
    return i;
}

ut_int32_t stringcmp(const char *cs,const char *ct)
{
    char __res;
    while (1) {
        if ((__res = (char)(*cs - *ct++)) != 0 || !*cs++)
            break;
    }
    return __res;
}


/********************************************ȫ�ֱ�������**********************************************/

/********************************************ȫ�ֺ�������**********************************************/
static void test_stati_init(test_stati_s *tst)
{
    int i;
    fail_info_s *fail;
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
        fail->next = NULL;
        fail->suite = NULL;
        fail->tcase = NULL;
    }
}

static void stati_info_init(stati_info_s *sti)
{
    test_stati_init(&sti->stat);
    
    sti->failhead = NULL;
    sti->lastfail = NULL;
    sti->failcnt = 0;
    sti->suite = NULL;
    sti->tcase = NULL;
    sti->case_err = 0;
    sti->suite_err = 0;
    
}

void test_framework_init(void)
{
    stati_info_s *sti;
    suite_list_s *tsl = &suite_list;

    tsl->head = NULL;
    tsl->tail = NULL;
    tsl->cnt = 0;
    
    sti = &stati_info;
    stati_info_init(sti);
}

err_t test_suite_register(test_suite_s *test_suite)
{
    suite_list_s *tsl = &suite_list;
    TEST_ASSERT_RETURN(test_suite == NULL,ERR_NULL_POINTER);
    if(tsl->tail == NULL)
    {
        tsl->tail = test_suite;
        tsl->head = test_suite;
    }
    else
    {
        tsl->tail->next = test_suite;
        tsl->tail = test_suite;
    }
    test_suite->next = NULL;
    tsl->cnt ++;
    return ERR_OK;
}

static ut_int32_t is_in_errlist(ut_uint32_t line)
{
    stati_info_s *sti;
    fail_info_s *fail;
    ut_int32_t match = 0;
    sti = &stati_info;
    if(sti->failhead == NULL)
        return 0;
    for(fail = sti->failhead;fail != NULL;fail = fail->next)
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

static void save_fail_info(ut_uint32_t line)
{
    stati_info_s *sti;
    fail_info_s *fail;
    sti = &stati_info;
    if(sti->failcnt >= TEST_FAIL_LIST_CNT)
        return;
     if(is_in_errlist(line))
        return;
    fail = &sti->stat.fail_obj[sti->failcnt ++];//&g_fail_obj[sti->failcnt ++];
    fail->suite = sti->suite;
    fail->tcase = sti->tcase;
    fail->line = line;
    if(sti->lastfail == NULL)
    {
        sti->failhead = fail;
        sti->lastfail = fail;
    }
    else
    {
        sti->lastfail->next = fail;
        sti->lastfail = fail;
    }
    fail->next = NULL;
}

void test_suite_err(ut_uint32_t line)
{
    stati_info_s *sti;
    sti = &stati_info;
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

    test_printf("%-12s","SUITE");
    test_printf("%s",space);
    test_printf("%-12s","CASE");
    test_printf("%s",space);
    test_printf("%-6s\r\n","LINE");
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

    test_printf("%-12s",fail->suite->name);
    test_printf("%s",space);
    test_printf("%-12s",fail->tcase->name);
    test_printf("%s",space);
    test_printf("%-6d\r\n",fail->line);
}

void test_framework_summit(void)
{
    stati_info_s *sti;
    ut_uint32_t i;
    fail_info_s *fail;
    ut_uint32_t space_cnt = 4;
    sti = &stati_info;
    test_printf("\r\n\r\n[************ALL TEST SUMMARY************]\r\n");
    test_printf("total  suites:%d\r\n",sti->stat.tot_suite);
    test_printf("passed suites:%d\r\n",sti->stat.passed_suite);
    test_printf("failed suites:%d\r\n",sti->stat.failed_suite);

    test_printf("\r\ntotal  cases:%d\r\n",sti->stat.tot_case);
    test_printf("passed cases:%d\r\n",sti->stat.passed_case);
    test_printf("failed cases:%d\r\n",sti->stat.failed_case);

    if(sti->failhead != NULL)
    {
        test_printf("\r\nfailture list as following:\r\n\r\n",sti->stat.tot_case);
        fail = sti->failhead;
        test_printf("--------------------------------------\r\n");
        print_header(space_cnt);
        test_printf("--------------------------------------\r\n");
        for(i = 0;i < sti->failcnt;i ++)
        {
            print_fail_info(fail,space_cnt);
            fail = fail->next;
            if(fail == NULL)
                break;
        }
        test_printf("--------------------------------------\r\n");

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
void show_test_cases(test_suite_s *ts)
{
    int i;
    for(i = 0;i < ts->case_cnt;i ++)
    {
        test_printf("|   |----%s\r\n",ts->tcase[i].name);
    }
}

void show_test_suites(void)
{
    ut_uint32_t i;
    test_suite_s *ts;
    test_printf("\r\nTest Suites List As Following:\r\n");
    if(suite_list.head == NULL)
    {
        test_framework_init();
        test_suite_register_all();
    }
    ts = suite_list.head;
    for(i = 0;i < suite_list.cnt;i ++)
    {
        test_printf("|---%s\r\n",ts->name);
        show_test_cases(ts);
        ts = ts->next;
    }
}


static void execute_one_case(test_suite_s *ts,test_case_s *tc)
{
    stati_info_s *sti = &stati_info;
    
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

static void execute_one_suite(test_suite_s *ts,char *casefilter)
{
    ut_uint32_t i;
    test_case_s *tc;
    stati_info_s *sti = &stati_info;
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
    test_suite_s *ts;
    stati_info_s *sti;
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
