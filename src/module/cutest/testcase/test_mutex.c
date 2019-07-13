/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_mutex.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 互斥锁单元测试
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
#include "wind_mutex.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_MUTEX_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************内部变量定义**********************************************/

static w_mutex_s *mutexs[4];
static w_mutex_s test_mtx;


/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
CASE_SETUP(mutexinit)
{

}

CASE_TEARDOWN(mutexinit)
{

}

CASE_FUNC(mutexinit)
{
    w_err_t err;
    w_mutex_s *mtx;
    err = wind_mutex_init(&test_mtx,"test");
    EXPECT_EQ(err,W_ERR_OK);
    mtx = wind_mutex_get("test");
    EXPECT_EQ(&test_mtx,mtx);
    EXPECT_EQ(mtx->obj.magic,WIND_MUTEX_MAGIC);
    EXPECT_FALSE(IS_F_MUTEX_LOCKED(mtx));
    EXPECT_FALSE(IS_F_MUTEX_POOL(mtx));
    EXPECT_EQ(mtx->waitlist.head,W_NULL);
    EXPECT_EQ(mtx->waitlist.tail,W_NULL);
    err = wind_mutex_destroy(&test_mtx);
    EXPECT_EQ(W_ERR_OK,err);

    err = wind_mutex_init(&test_mtx,W_NULL);
    EXPECT_EQ(err,W_ERR_OK);
    mtx = wind_mutex_get(W_NULL);
    EXPECT_EQ(mtx,W_NULL);
    err = wind_mutex_destroy(&test_mtx);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(test_mtx.obj.magic,(~WIND_MUTEX_MAGIC));

}


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
    EXPECT_NE(mutexs[0],W_NULL);
    EXPECT_FALSE(IS_F_MUTEX_LOCKED(mutexs[0]));
    EXPECT_TRUE(IS_F_MUTEX_POOL(mutexs[0]));
    EXPECT_EQ(mutexs[0]->waitlist.head,W_NULL);
    EXPECT_EQ(mutexs[0]->waitlist.tail,W_NULL);
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
    EXPECT_NE(mutexs[0],W_NULL);
    err = wind_mutex_lock(mutexs[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_TRUE(IS_F_MUTEX_LOCKED(mutexs[0]));
    err = wind_mutex_unlock(mutexs[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_FALSE(IS_F_MUTEX_LOCKED(mutexs[0]));
    EXPECT_EQ(mutexs[0]->waitlist.head,W_NULL);
    EXPECT_EQ(mutexs[0]->waitlist.tail,W_NULL);
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
    const char *name[4] = {"t1","t2","t3","t4"};
    for(i = 0;i < 4;i ++)
    {
        mutexs[i] = wind_mutex_create(name[i]);
        EXPECT_NE(mutexs[0],W_NULL);
    }
    for(i = 0;i < 4;i ++)
    {
        err = wind_mutex_destroy(mutexs[i]);
        EXPECT_EQ(W_ERR_OK,err);
    }
}

SUITE_SETUP(mutex)
{

}

SUITE_TEARDOWN(mutex)
{

}


TEST_CASES_START(mutex)
TEST_CASE(mutexinit)
TEST_CASE(mutexinfo)
TEST_CASE(mutexfunc)
TEST_CASE(mutexmulti)
TEST_CASES_END
TEST_SUITE(mutex)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
