/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_mutex.c
** 创   建   人: 周江村
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 互斥锁单元测试
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
#include "wind_mutex.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************内部变量定义**********************************************/
#if WIND_MUTEX_SUPPORT

static w_mutex_s *mutexs[4];


/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

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
    EXPECT_EQ(mutexs[0]->mutexed,W_FALSE);
    EXPECT_EQ(mutexs[0]->waitlist.head,NULL);
    EXPECT_EQ(mutexs[0]->waitlist.tail,NULL);
    err = wind_mutex_destroy(mutexs[0]);
    EXPECT_EQ(W_ERR_OK,err);
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
    err = wind_mutex_lock(mutexs[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(mutexs[0]->mutexed,W_TRUE);
    err = wind_mutex_unlock(mutexs[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(mutexs[0]->mutexed,W_FALSE);
    EXPECT_EQ(mutexs[0]->waitlist.head,NULL);
    EXPECT_EQ(mutexs[0]->waitlist.tail,NULL);
    err = wind_mutex_destroy(mutexs[0]);
    EXPECT_EQ(W_ERR_OK,err);
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
        err = wind_mutex_destroy(mutexs[i]);
        EXPECT_EQ(W_ERR_OK,err);
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

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
