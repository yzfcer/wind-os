/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_pool.c
** 创   建   人: 周江村
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 内存池单元测试
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
#include "wind_cut.h"
#include "wind_pool.h"
#if (WIND_CUTEST_SUPPORT && TEST_POOL_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define TNUM 5
#define TSIZE 13


WIND_POOL(test_pool,TNUM,TSIZE);
void *testblk[TNUM+1];


CASE_SETUP(pool_info)
{
    wind_pool_create("test_pool",test_pool,sizeof(test_pool),TSIZE);
}

CASE_TEARDOWN(pool_info)
{
    wind_pool_destroy(test_pool);
}

CASE_FUNC(pool_info)
{
    w_pool_s *pool = (w_pool_s *)test_pool;
    EXPECT_EQ(pool->magic,WIND_POOL_MAGIC);
    EXPECT_STR_EQ(pool->name,"test_pool");
    EXPECT_EQ(pool->size,sizeof(test_pool)-sizeof(w_pool_s));
    EXPECT_EQ(pool->itemnum,TNUM);
    EXPECT_EQ(pool->itemsize,(((TSIZE+3)>>2)<<2)+sizeof(w_pool_item_s));
}


CASE_SETUP(pool_alloc)
{
    int i;
    wind_pool_create("test_pool",test_pool,
                     sizeof(test_pool),TSIZE);
    for(i = 0;i < TNUM+1;i ++)
        testblk[i] = 0;
}


CASE_TEARDOWN(pool_alloc)
{
    int i;
    wind_pool_destroy(test_pool);
    for(i = 0;i < TNUM+1;i ++)
        testblk[i] = 0;
}

CASE_FUNC(pool_alloc)
{
    int i;
    w_err_t err;
    w_pool_item_s* itm;
    for(i = 0;i < TNUM;i ++)
    {
        testblk[i] = wind_pool_malloc(test_pool);
        EXPECT_NE(testblk[i],W_NULL);
        itm = (w_pool_item_s*)((w_uint32_t)testblk[i] - sizeof(w_pool_item_s*));
        EXPECT_EQ(itm->flag,POOL_BLK_USED);
        EXPECT_EQ(itm->next,W_NULL);
    }
    testblk[TNUM] = wind_pool_malloc(test_pool);
    EXPECT_EQ(testblk[TNUM],W_NULL);
    
    for(i = 0;i < TNUM;i ++)
    {
        wind_pool_free(test_pool,testblk[i]);
        itm = (w_pool_item_s*)((w_uint32_t)testblk[i] - sizeof(w_pool_item_s*));
        EXPECT_EQ(itm->flag,POOL_BLK_FREE);
        EXPECT_EQ(itm->next,W_NULL);
    }
    err = wind_pool_free(test_pool,testblk[TNUM]);
    EXPECT_EQ(err,W_ERR_PTR_NULL);
    err = wind_pool_free(test_pool,(void*)0xffffffff);
    EXPECT_EQ(err,W_ERR_INVALID);
    err = wind_pool_free(test_pool,testblk[0]);
    EXPECT_EQ(err,W_ERR_INVALID);
}


SUITE_SETUP(test_pool){}
SUITE_TEARDOWN(test_pool){}



TEST_CASES_START(test_pool)
TEST_CASE(pool_info)
TEST_CASE(pool_alloc)
TEST_CASES_END
TEST_SUITE(test_pool)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
