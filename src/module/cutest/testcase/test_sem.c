/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_sem.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 信号量单元测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
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
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_SEM_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************内部变量定义**********************************************/
w_sem_s *sems[4];
w_sem_s test_sm;





/********************************************内部函数定义*********************************************/


/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

CASE_SETUP(init) FUNC_EMPTY
CASE_TEARDOWN(init) FUNC_EMPTY
CASE_FUNC(init)
{
    w_err_t err;
    err = wind_sem_init(&test_sm,"test",3);
    EXPECT_EQ(err,W_ERR_OK);
    sems[0] = wind_sem_get("test");
    EXPECT_NE(sems[0],W_NULL);
    EXPECT_STR_EQ(sems[0]->obj.name,"test");
    EXPECT_EQ(sems[0]->sem_tot,3);
    EXPECT_FALSE(IS_F_SEM_POOL(sems[0]));
    EXPECT_EQ(sems[0]->sem_num,3);
    EXPECT_EQ(sems[0]->waitlist.head,W_NULL);
    EXPECT_EQ(sems[0]->waitlist.tail,W_NULL);
    err = wind_sem_destroy(sems[0]);
    EXPECT_EQ(W_ERR_OK,err);

    err = wind_sem_init(&test_sm,W_NULL,3);
    EXPECT_EQ(err,W_ERR_OK);
    sems[0] = wind_sem_get(W_NULL);
    EXPECT_EQ(sems[0],W_NULL);
    err = wind_sem_destroy(&test_sm);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(test_sm.obj.magic,(~WIND_SEM_MAGIC));
}

CASE_SETUP(info) FUNC_EMPTY
CASE_TEARDOWN(info) FUNC_EMPTY
CASE_FUNC(info)
{
    w_err_t err;
    sems[0] = wind_sem_create("test",3);
    EXPECT_NE(sems[0],W_NULL);
    EXPECT_STR_EQ(sems[0]->obj.name,"test");
    EXPECT_EQ(sems[0]->sem_tot,3);
    EXPECT_EQ(sems[0]->sem_num,3);
    EXPECT_TRUE(IS_F_SEM_POOL(sems[0]));
    EXPECT_EQ(sems[0]->waitlist.head,W_NULL);
    EXPECT_EQ(sems[0]->waitlist.tail,W_NULL);
    err = wind_sem_destroy(sems[0]);
    EXPECT_EQ(W_ERR_OK,err);

}

CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_int32_t i;
    w_err_t err;
    sems[0] = wind_sem_create("test",3);
    EXPECT_NE(sems[0],W_NULL);
    for(i = 0;i < 3;i ++)
    {
        err = wind_sem_wait(sems[0],100);
        EXPECT_EQ(err,W_ERR_OK);
        EXPECT_EQ(sems[0]->sem_num,sems[0]->sem_tot - 1 - i);
    }
    err = wind_sem_wait(sems[0],100);
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

CASE_SETUP(multi) FUNC_EMPTY
CASE_TEARDOWN(multi) FUNC_EMPTY
CASE_FUNC(multi)
{
    w_int32_t i;
    w_err_t err;
    const char *name[4] = {"t1","t2","t3","t4"};
    for(i = 0;i < 4;i ++)
    {
        sems[i] = wind_sem_create(name[i],1);
        EXPECT_NE(sems[0],W_NULL);
    }
    for(i = 0;i < 4;i ++)
    {
        err = wind_sem_destroy(sems[i]);
        EXPECT_EQ(W_ERR_OK,err);
    }
}


SUITE_SETUP(sem) FUNC_EMPTY
SUITE_TEARDOWN(sem) FUNC_EMPTY

TEST_CASES_START(sem)
TEST_CASE(init)
TEST_CASE(info)
TEST_CASE(func)
TEST_CASE(multi)
TEST_CASES_END
TEST_SUITE(sem)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
