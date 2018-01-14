/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_sem.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2017/10/22 16:29:55
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: ��Ԫ���������Ĳ���ģ��
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/
#include "cut.h"
#include "wind_lock.h"
/********************************************�ڲ���������**********************************************/
static lock_s *locks[4];


/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/

CASE_SETUP(lockinfo)
{

}

CASE_TEARDOWN(lockinfo)
{

}

CASE_FUNC(lockinfo)
{
    w_err_t err;
    locks[0] = wind_lock_create("test");
    EXPECT_NE(locks[0],NULL);
    EXPECT_EQ(locks[0]->locked,B_FALSE);
    EXPECT_EQ(locks[0]->waitlist.head,NULL);
    EXPECT_EQ(locks[0]->waitlist.tail,NULL);
    err = wind_lock_free(locks[0]);
    EXPECT_EQ(ERR_OK,err);
}

CASE_SETUP(lockfunc)
{
    
}

CASE_TEARDOWN(lockfunc)
{

}

CASE_FUNC(lockfunc)
{
    w_err_t err;
    locks[0] = wind_lock_create("test");
    EXPECT_NE(locks[0],NULL);
    err = wind_lock_close(locks[0]);
    EXPECT_EQ(ERR_OK,err);
    EXPECT_EQ(locks[0]->locked,B_TRUE);
    err = wind_lock_open(locks[0]);
    EXPECT_EQ(ERR_OK,err);
    EXPECT_EQ(locks[0]->locked,B_FALSE);
    EXPECT_EQ(locks[0]->waitlist.head,NULL);
    EXPECT_EQ(locks[0]->waitlist.tail,NULL);
    err = wind_lock_free(locks[0]);
    EXPECT_EQ(ERR_OK,err);
}

CASE_SETUP(lockmulti)
{
    
}

CASE_TEARDOWN(lockmulti)
{

}

CASE_FUNC(lockmulti)
{
    w_int32_t i;
    w_err_t err;
    for(i = 0;i < 4;i ++)
    {
        locks[i] = wind_lock_create("test");
        EXPECT_NE(locks[0],NULL);
    }
    for(i = 0;i < 4;i ++)
    {
        err = wind_lock_free(locks[i]);
        EXPECT_EQ(ERR_OK,err);
    }
}

SUITE_SETUP(test_lock)
{

}

SUITE_TEARDOWN(test_lock)
{

}


TEST_CASES_START(test_lock)
TEST_CASE(lockinfo)
TEST_CASE(lockfunc)
TEST_CASE(lockmulti)
TEST_CASES_END
TEST_SUITE(test_lock)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
