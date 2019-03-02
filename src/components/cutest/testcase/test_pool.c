/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_pool.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 内存池单元测试
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
    w_pool_s *pool = wind_pool_get_by_mem(test_pool);
    EXPECT_EQ(pool->magic,WIND_POOL_MAGIC);
    EXPECT_STR_EQ(pool->name,"test_pool");
    EXPECT_EQ(pool->size,sizeof(test_pool)-sizeof(w_pool_s)-8);
    EXPECT_EQ(pool->itemnum,TNUM);
    EXPECT_EQ(pool->itemsize,(((TSIZE+7)>>3)<<3)+sizeof(w_pihead_s));
}


CASE_SETUP(pool_alloc)
{
    int i;
    wind_pool_create("test_pool",test_pool,
                     sizeof(test_pool),TSIZE);
    //wind_pool_print_list();
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
    w_poolitem_s* itm;
    for(i = 0;i < TNUM;i ++)
    {
        testblk[i] = wind_pool_malloc(test_pool);
        EXPECT_NE(testblk[i],W_NULL);
        itm = (w_poolitem_s*)POOL_ITEM(testblk[i],w_poolitem_s,buff);
        EXPECT_TRUE(IS_F_POOLITEM_USED(itm));
        EXPECT_EQ(itm->head.next,W_NULL);
    }
    
    testblk[TNUM] = wind_pool_malloc(test_pool);
    EXPECT_EQ(testblk[TNUM],W_NULL);
    
    for(i = 0;i < TNUM;i ++)
    {
        wind_pool_free(test_pool,testblk[i]);
        itm = (w_poolitem_s*)POOL_ITEM(testblk[i],w_poolitem_s,buff);
        EXPECT_FALSE(IS_F_POOLITEM_USED(itm));
        EXPECT_EQ(itm->head.next,W_NULL);
    }
    err = wind_pool_free(test_pool,testblk[TNUM]);
    EXPECT_EQ(err,W_ERR_PTR_NULL);
    err = wind_pool_free(test_pool,(void*)&err);
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
