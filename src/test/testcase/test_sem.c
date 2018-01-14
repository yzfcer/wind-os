/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_sem.c
** 创   建   人: 周江村
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2017/10/22 16:29:55
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 单元测试用例的测试模板
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************头文件定义***********************************************/
#include "cut.h"
#include "wind_sem.h"
/********************************************内部变量定义**********************************************/
sem_s *sems[4];





/********************************************内部函数定义*********************************************/


/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/


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
    EXPECT_NE(sems[0],NULL);
    EXPECT_STR_EQ(sems[0]->name,"test");
    EXPECT_EQ(sems[0]->sem_tot,3);
    EXPECT_EQ(sems[0]->sem_num,3);
    EXPECT_EQ(sems[0]->waitlist.head,NULL);
    EXPECT_EQ(sems[0]->waitlist.tail,NULL);
    err = wind_sem_free(sems[0]);
    EXPECT_EQ(ERR_OK,err);

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
    EXPECT_NE(sems[0],NULL);
    for(i = 0;i < 3;i ++)
    {
        err = wind_sem_fetch(sems[0],1000);
        EXPECT_EQ(err,ERR_OK);
        EXPECT_EQ(sems[0]->sem_num,sems[0]->sem_tot - 1 - i);
    }
    err = wind_sem_fetch(sems[0],1000);
    EXPECT_EQ(err,ERR_TIMEOUT);
    EXPECT_EQ(sems[0]->sem_num,0);

    for(i = 0;i < 3;i ++)
    {
        err = wind_sem_post(sems[0]);
        EXPECT_EQ(err,ERR_OK);
        EXPECT_EQ(sems[0]->sem_num,1 + i);
    }
    err = wind_sem_free(sems[0]);
    EXPECT_EQ(ERR_OK,err);

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
        EXPECT_NE(sems[0],NULL);
    }
    for(i = 0;i < 4;i ++)
    {
        err = wind_sem_free(sems[i]);
        EXPECT_EQ(ERR_OK,err);
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


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
