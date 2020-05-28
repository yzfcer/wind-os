/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_msgbox.c
** Author      : Jason Zhou
** Last Date   : 2017/10/22 16:29:55
** Description : Message mailbox unit test unit test suite
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
/*********************************************header file***********************************************/
#include "wind_cut.h"
#include "wind_pool.h"
#include "wind_thread.h"
#include "wind_msgbox.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_MSGBOX_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************internal variables**********************************************/
static w_msgbox_s *msgbox;
static w_msgbox_s test_mb;
typedef struct 
{
    w_msg_s msg;
    int value;
}test_msg_s;

WIND_POOL(testmsg,6,sizeof(test_msg_s));
WIND_POOL(testmsg_pool,4,sizeof(test_msg_s));
#define MSG_TO_TEST_MSG(msg) (test_msg_s*)(((w_uint8_t*)(msg))-((w_addr_t)&(((test_msg_s*)0)->msg)))

/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/

CASE_SETUP(init) FUNC_EMPTY
CASE_TEARDOWN(init) FUNC_EMPTY
CASE_FUNC(init)
{
    w_err_t err;
    w_thread_s *thr;
    thr = wind_thread_current();
    err = wind_msgbox_init(&test_mb,"test");
    EXPECT_EQ(err,W_ERR_OK);
    msgbox = wind_msgbox_get("test");
    EXPECT_NE(msgbox,W_NULL);
    EXPECT_EQ(msgbox->obj.magic,WIND_MSGBOX_MAGIC);
    EXPECT_EQ(msgbox->msgnum,0);
    EXPECT_FALSE(IS_F_MSGBOX_POOL(msgbox));
    EXPECT_EQ(msgbox->msglist.head,W_NULL);
    EXPECT_EQ(msgbox->msglist.tail,W_NULL);
    EXPECT_EQ(msgbox->owner,thr);
    err = wind_msgbox_destroy(msgbox);
    EXPECT_EQ(W_ERR_OK,err);

    err = wind_msgbox_init(&test_mb,W_NULL);
    EXPECT_EQ(err,W_ERR_OK);
    msgbox = wind_msgbox_get(W_NULL);
    EXPECT_EQ(msgbox,W_NULL);
    err = wind_msgbox_destroy(&test_mb);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(test_mb.obj.magic,(~WIND_MSGBOX_MAGIC));
}

CASE_SETUP(info) FUNC_EMPTY
CASE_TEARDOWN(info) FUNC_EMPTY
CASE_FUNC(info)
{
    w_err_t err;
    w_thread_s *thr;
    thr = wind_thread_current();
    msgbox = wind_msgbox_create("test");
    EXPECT_NE(msgbox,W_NULL);
    EXPECT_EQ(msgbox->obj.magic,WIND_MSGBOX_MAGIC);
    EXPECT_EQ(msgbox->msgnum,0);
    EXPECT_TRUE(IS_F_MSGBOX_POOL(msgbox));
    EXPECT_EQ(msgbox->msglist.head,W_NULL);
    EXPECT_EQ(msgbox->msglist.tail,W_NULL);
    EXPECT_EQ(msgbox->owner,thr);
    err = wind_msgbox_destroy(msgbox);
    EXPECT_EQ(W_ERR_OK,err);
}

CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_err_t err;
    w_msg_s *msg;
    test_msg_s *tmsg,*tmsg1;
    tmsg1 = (test_msg_s *)wind_pool_malloc(testmsg_pool);
    msgbox = wind_msgbox_create("test");
    err = wind_msgbox_wait(msgbox,&msg,100);
    EXPECT_EQ(err,W_ERR_TIMEOUT);
    wind_msg_init(&tmsg1->msg,1,sizeof(test_msg_s),tmsg1);
    tmsg1->value = 1;
    err = wind_msgbox_post(msgbox,&tmsg1->msg);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_msgbox_wait(msgbox,&msg,100);
    EXPECT_EQ(err,W_ERR_OK);
    tmsg = MSG_TO_TEST_MSG(msg);
    EXPECT_EQ(tmsg->msg.msg_id,1);
    EXPECT_EQ(tmsg->msg.msg_len,sizeof(test_msg_s));
    EXPECT_EQ(tmsg->msg.msg_arg,tmsg1);
    err = wind_msgbox_wait(msgbox,&msg,100);
    EXPECT_EQ(err,W_ERR_TIMEOUT);
    wind_pool_free(testmsg_pool,tmsg1);
    tmsg1 = W_NULL;
    err = wind_msgbox_destroy(msgbox);
    EXPECT_EQ(W_ERR_OK,err);
}

CASE_SETUP(multthread) FUNC_EMPTY
CASE_TEARDOWN(multthread) FUNC_EMPTY
CASE_FUNC(multthread)
{

}
SUITE_SETUP(msgbox)
{
    wind_pool_create("testmsg_pool",testmsg_pool,sizeof(testmsg_pool),sizeof(test_msg_s));
    return W_ERR_OK;
}

SUITE_TEARDOWN(msgbox)
{
    wind_pool_destroy(testmsg_pool);
    return W_ERR_OK;
}


TEST_CASES_START(msgbox)
TEST_CASE(init)
TEST_CASE(info)
TEST_CASE(func)
TEST_CASE(multthread)
TEST_CASES_END
TEST_SUITE(msgbox)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif 
