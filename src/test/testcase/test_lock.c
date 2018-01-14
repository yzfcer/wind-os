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
#include "wind_lock.h"
/********************************************内部变量定义**********************************************/
static lock_s *locks[4];


/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

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
