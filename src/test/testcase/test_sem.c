/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_sem.c
** 创   建   人: 周江村
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 信号量单元测试
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
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************头文件定义***********************************************/
#include "wind_cut.h"
#include "wind_sem.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_SEM_SUPPORT
/********************************************内部变量定义**********************************************/
w_sem_s *sems[4];





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
