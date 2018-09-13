/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_sem.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: �ź�����Ԫ����
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
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************ͷ�ļ�����***********************************************/
#include "wind_cut.h"
#include "wind_sem.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_SEM_SUPPORT
/********************************************�ڲ���������**********************************************/
w_sem_s *sems[4];





/********************************************�ڲ���������*********************************************/


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/


CASE_SETUP(seminfo)
{

}

CASE_TEARDOWN(seminfo)
{

}

CASE_FUNC(seminfo)
{
    w_err_t err;
    sems[0] = wind_sem_create("test",3);
    EXPECT_NE(sems[0],W_NULL);
    EXPECT_STR_EQ(sems[0]->name,"test");
    EXPECT_EQ(sems[0]->sem_tot,3);
    EXPECT_EQ(sems[0]->sem_num,3);
    EXPECT_EQ(sems[0]->waitlist.head,W_NULL);
    EXPECT_EQ(sems[0]->waitlist.tail,W_NULL);
    err = wind_sem_destroy(sems[0]);
    EXPECT_EQ(W_ERR_OK,err);

}

CASE_SETUP(semfunc)
{

}

CASE_TEARDOWN(semfunc)
{

}

CASE_FUNC(semfunc)
{
    w_int32_t i;
    w_err_t err;
    sems[0] = wind_sem_create("test",3);
    EXPECT_NE(sems[0],W_NULL);
    for(i = 0;i < 3;i ++)
    {
        err = wind_sem_wait(sems[0],1000);
        EXPECT_EQ(err,W_ERR_OK);
        EXPECT_EQ(sems[0]->sem_num,sems[0]->sem_tot - 1 - i);
    }
    err = wind_sem_wait(sems[0],1000);
    EXPECT_EQ(err,W_ERR_TIMEOUT);
    EXPECT_EQ(sems[0]->sem_num,0);

    for(i = 0;i < 3;i ++)
    {
        err = wind_sem_post(sems[0]);
        EXPECT_EQ(err,W_ERR_OK);
        EXPECT_EQ(sems[0]->sem_num,1 + i);
    }
    err = wind_sem_destroy(sems[0]);
    EXPECT_EQ(W_ERR_OK,err);

}

CASE_SETUP(semmulti)
{
    
}

CASE_TEARDOWN(semmulti)
{

}

CASE_FUNC(semmulti)
{
    w_int32_t i;
    w_err_t err;
    for(i = 0;i < 4;i ++)
    {
        sems[i] = wind_sem_create("test",1);
        EXPECT_NE(sems[0],W_NULL);
    }
    for(i = 0;i < 4;i ++)
    {
        err = wind_sem_destroy(sems[i]);
        EXPECT_EQ(W_ERR_OK,err);
    }
}


SUITE_SETUP(test_sem)
{

}

SUITE_TEARDOWN(test_sem)
{

}


TEST_CASES_START(test_sem)
TEST_CASE(seminfo)
TEST_CASE(semfunc)
TEST_CASE(semmulti)
TEST_CASES_END
TEST_SUITE(test_sem)

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
