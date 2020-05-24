/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: test_pool.c
** **Author: Jason Zhou
** Last Date: 2017/10/22 16:29:55
** Description: 内存池单元测试
**  
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2017/10/22 16:29:55
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cut.h"
#include "wind_pool.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_POOL_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define TNUM 5
#define TSIZE 13


WIND_POOL(test_pool,TNUM,TSIZE);
static w_uint8_t *testblk[TNUM+1];


CASE_SETUP(info)
{
    wind_pool_create("test_pool",test_pool,sizeof(test_pool),TSIZE);
    return W_ERR_OK;
}

CASE_TEARDOWN(info)
{
    wind_pool_destroy(test_pool);
    return W_ERR_OK;
}

CASE_FUNC(info)
{
    w_pool_s *pool = wind_pool_get_by_mem(test_pool);
    EXPECT_EQ(pool->obj.magic,WIND_POOL_MAGIC);
    EXPECT_STR_EQ(pool->obj.name,"test_pool");
    EXPECT_EQ(pool->size,sizeof(test_pool)-sizeof(w_pool_s)-8);
    EXPECT_EQ(pool->itemnum,TNUM);
    EXPECT_EQ(pool->itemsize,(((TSIZE+7)>>3)<<3)+sizeof(w_pihead_s));
}


CASE_SETUP(alloc)
{
    int i;
    wind_pool_create("test_pool",test_pool,
                     sizeof(test_pool),TSIZE);
    //wind_pool_print_detail();
    for(i = 0;i < TNUM+1;i ++)
        testblk[i] = 0;
    return W_ERR_OK;
}


CASE_TEARDOWN(alloc)
{
    int i;
    wind_pool_destroy(test_pool);
    for(i = 0;i < TNUM+1;i ++)
        testblk[i] = 0;
    return W_ERR_OK;
}

CASE_FUNC(alloc)
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


SUITE_SETUP(pool) FUNC_EMPTY
SUITE_TEARDOWN(pool) FUNC_EMPTY


TEST_CASES_START(pool)
TEST_CASE(info)
TEST_CASE(alloc)
TEST_CASES_END
TEST_SUITE(pool)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
