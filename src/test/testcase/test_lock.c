/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_mutex.c
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
#include "wind_mutex.h"
/********************************************�ڲ���������**********************************************/
static mutex_s *mutexs[4];


/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/

CASE_SETUP(mutexinfo)
{

}

CASE_TEARDOWN(mutexinfo)
{

}

CASE_FUNC(mutexinfo)
{
    w_err_t err;
    mutexs[0] = wind_mutex_create("test");
    EXPECT_NE(mutexs[0],NULL);
    EXPECT_EQ(mutexs[0]->mutexed,B_FALSE);
    EXPECT_EQ(mutexs[0]->waitlist.head,NULL);
    EXPECT_EQ(mutexs[0]->waitlist.tail,NULL);
    err = wind_mutex_free(mutexs[0]);
    EXPECT_EQ(ERR_OK,err);
}

CASE_SETUP(mutexfunc)
{
    
}

CASE_TEARDOWN(mutexfunc)
{

}

CASE_FUNC(mutexfunc)
{
    w_err_t err;
    mutexs[0] = wind_mutex_create("test");
    EXPECT_NE(mutexs[0],NULL);
    err = wind_mutex_close(mutexs[0]);
    EXPECT_EQ(ERR_OK,err);
    EXPECT_EQ(mutexs[0]->mutexed,B_TRUE);
    err = wind_mutex_open(mutexs[0]);
    EXPECT_EQ(ERR_OK,err);
    EXPECT_EQ(mutexs[0]->mutexed,B_FALSE);
    EXPECT_EQ(mutexs[0]->waitlist.head,NULL);
    EXPECT_EQ(mutexs[0]->waitlist.tail,NULL);
    err = wind_mutex_free(mutexs[0]);
    EXPECT_EQ(ERR_OK,err);
}

CASE_SETUP(mutexmulti)
{
    
}

CASE_TEARDOWN(mutexmulti)
{

}

CASE_FUNC(mutexmulti)
{
    w_int32_t i;
    w_err_t err;
    for(i = 0;i < 4;i ++)
    {
        mutexs[i] = wind_mutex_create("test");
        EXPECT_NE(mutexs[0],NULL);
    }
    for(i = 0;i < 4;i ++)
    {
        err = wind_mutex_free(mutexs[i]);
        EXPECT_EQ(ERR_OK,err);
    }
}

SUITE_SETUP(test_mutex)
{

}

SUITE_TEARDOWN(test_mutex)
{

}


TEST_CASES_START(test_mutex)
TEST_CASE(mutexinfo)
TEST_CASE(mutexfunc)
TEST_CASE(mutexmulti)
TEST_CASES_END
TEST_SUITE(test_mutex)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
