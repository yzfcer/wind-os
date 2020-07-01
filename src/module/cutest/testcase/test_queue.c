/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_queue.c
** Author      : Jason Zhou
** Last Date   : 2017/10/22 16:29:55
** Description : Queue unit test suite
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2017/10/22 16:29:55
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cut.h"
#include "wind_queue.h"
#include "wind_string.h"
#include "wind_queue.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_QUEUE_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




w_uint8_t queuebuf[128];













CASE_SETUP(queueinfo) FUNC_EMPTY
CASE_TEARDOWN(queueinfo) FUNC_EMPTY
CASE_FUNC(queueinfo)
{
    w_err_t err;
    w_queue_s *q = (w_queue_s *)queuebuf;
    err = wind_queue_create(queuebuf,sizeof(queuebuf),sizeof(w_int32_t));
    EXPECT_NE(err,W_ERR_OK);
    EXPECT_EQ(q->magic,WIND_QUEUE_MAGIC);
    EXPECT_EQ(q->rd,q->buf);
    EXPECT_EQ(q->wr,q->buf);
    EXPECT_EQ(q->itemsize,sizeof(w_int32_t));
    EXPECT_EQ(q->count,0);
    EXPECT_EQ(q->capacity,(sizeof(queuebuf) - (w_uint32_t)(w_addr_t)(((w_queue_s *)0)->buf)) / q->itemsize);
    EXPECT_EQ(q->end,q->buf + q->capacity * q->itemsize);
    err = wind_queue_destory(q);
    EXPECT_EQ(W_ERR_OK,err);

}

CASE_SETUP(queuefunc) FUNC_EMPTY
CASE_TEARDOWN(queuefunc) FUNC_EMPTY
CASE_FUNC(queuefunc)
{
    w_err_t err;
    w_int32_t i;
    w_int32_t res;
    w_int32_t va = 100;
    
    err = wind_queue_create(queuebuf,sizeof(queuebuf),sizeof(w_int32_t));
    EXPECT_NE(err,W_ERR_OK);
    for(i = 0;i < 50;i ++)
    {
        va = 100 + i;
        res = wind_queue_write(queuebuf,&va,sizeof(w_int32_t));
        EXPECT_EQ(res,sizeof(w_int32_t));
        
    }
    
    for(i = 0;i < 50;i ++)
    {
        res = wind_queue_read(queuebuf,&va,sizeof(w_int32_t));
        EXPECT_EQ(res,sizeof(w_int32_t));
        EXPECT_EQ(va,100 + i);
    }
    err = wind_queue_destory(queuebuf);
    EXPECT_EQ(W_ERR_OK,err);

}





SUITE_SETUP(queue) FUNC_EMPTY
SUITE_TEARDOWN(queue) FUNC_EMPTY

TEST_CASES_START(queue)
TEST_CASE(queueinfo)
TEST_CASE(queuefunc)
TEST_CASES_END
TEST_SUITE(queue)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif 
