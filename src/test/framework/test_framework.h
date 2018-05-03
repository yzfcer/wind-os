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
#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__
/*********************************************ͷ�ļ�����***********************************************/
#include "test_port.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus



/***********************************************�궨��*************************************************/
#define TEST_CASE_NAME_LEN 24
#define TEST_SUITE_NAME_LEN 24
#define TEST_FAIL_LIST_CNT 100
extern void test_suite_err(unsigned line);

#define FAIL_EXPECT_OUT(x,y,than) test_printf("failed expection:FUNCTION:%s,LINE:%d:which expected "#x" %s "#y"\r\n",__FUNCTION__,__LINE__,than)


//#define EXPECT_EQ(x,y) test_printf("FILE:%s,LINE:%d,expected %s,in fact %s\r\n",__FILE__,__LINE__,#x,#y)

/**********************************************ö�ٶ���************************************************/
#include <stdio.h>

#ifndef NULL
#define NULL (void*)0
#endif
#define err_t ut_uint32_t
#define ERR_OK 0
#define ERR_COMMAN -1
#define TEST_ASSERT_RETURN(x,y)
#define test_printf wind_printf

/*********************************************�ṹ�嶨��***********************************************/
//ȫ�ֵ�test suite�б�
typedef struct __test_case_s test_case_s;
typedef struct __test_suite_s test_suite_s;
typedef struct __suite_list_s suite_list_s;
typedef struct __test_stati_s test_stati_s;
typedef struct __fail_info_s fail_info_s;
typedef struct __stati_info_s stati_info_s;

//���������ṹ
struct __test_case_s
{
    char name[TEST_CASE_NAME_LEN];
    void (*setup)(void);
    void (*teardown)(void);
    void (*test)(void);
};

//�����׽ṹ
struct __test_suite_s
{
    char name[TEST_SUITE_NAME_LEN];
    ut_uint32_t case_cnt;//��������������
    test_case_s *tcase;
    //void (*init)(void);
    void (*setup)(void);
    void (*teardown)(void);
    struct __test_suite_s *next;
};


struct __suite_list_s
{
    test_suite_s *head;
    test_suite_s *tail;
    ut_uint32_t cnt;
};

//ִ��ʧ�ܵ���Ϣ
struct __fail_info_s
{
    test_suite_s *suite;
    test_case_s *tcase;
    ut_uint32_t line;
    fail_info_s *next;
};

//����ִ�н��ͳ����Ϣ
struct __test_stati_s
{
    ut_uint32_t tot_suite;
    ut_uint32_t tot_case;
    ut_uint32_t passed_suite;
    ut_uint32_t failed_suite;
    ut_uint32_t passed_case;
    ut_uint32_t failed_case;
    fail_info_s fail_obj[TEST_FAIL_LIST_CNT];
};




struct __stati_info_s
{
    //ͳ����Ϣ
    test_stati_s stat;

    //������Ϣ�б�
    fail_info_s *failhead;
    fail_info_s *lastfail;
    ut_uint32_t failcnt;

    //��ǰ������Ϣ
    test_suite_s *suite;
    test_case_s *tcase;
    ut_uint32_t case_err;
    ut_uint32_t suite_err;
};

/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
static ut_int32_t stringlenth(char *str);
ut_int32_t stringcmp(const char *cs,const char *ct);


err_t test_suite_register(test_suite_s *test_suite);
void show_test_suites(void);
void cut_test_start(char* testsuite,char *testcase);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __TEST_FRAMEWORK_H__
