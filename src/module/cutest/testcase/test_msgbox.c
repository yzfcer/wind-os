/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_msgbox.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 消息邮箱单元测试单元测试
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
#include "wind_pool.h"
#include "wind_thread.h"
#include "wind_msgbox.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_MSGBOX_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************内部变量定义**********************************************/
static w_msgbox_s *msgbox;
static w_msgbox_s test_mb;
typedef struct 
{
    w_msg_s msg;
    int value;
}test_msg_s;

WIND_POOL(testmsg,6,sizeof(test_msg_s));
WIND_POOL(testmsg_pool,4,sizeof(test_msg_s));
#define MSG_TO_TEST_MSG(msg) (test_msg_s*)(((w_uint8_t*)(msg))-((w_uint32_t)&(((test_msg_s*)0)->msg)))

/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

CASE_SETUP(msgboxinit)
{

}

CASE_TEARDOWN(msgboxinit)
{

}

CASE_FUNC(msgboxinit)
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

CASE_SETUP(msgboxinfo)
{

}

CASE_TEARDOWN(msgboxinfo)
{

}

CASE_FUNC(msgboxinfo)
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

CASE_SETUP(msgboxfunc)
{
    
}

CASE_TEARDOWN(msgboxfunc)
{

}

CASE_FUNC(msgboxfunc)
{
    w_err_t err;
    w_msg_s *msg;
    test_msg_s *tmsg,*tmsg1;
    tmsg1 = wind_pool_malloc(testmsg_pool);
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

CASE_SETUP(msgbox_multthread)
{
    
}

CASE_TEARDOWN(msgbox_multthread)
{

}

CASE_FUNC(msgbox_multthread)
{

}
SUITE_SETUP(msgbox)
{
    wind_pool_create("testmsg_pool",testmsg_pool,sizeof(testmsg_pool),sizeof(test_msg_s));
}

SUITE_TEARDOWN(msgbox)
{
    wind_pool_destroy(testmsg_pool);
}


TEST_CASES_START(msgbox)
TEST_CASE(msgboxinit)
TEST_CASE(msgboxinfo)
TEST_CASE(msgboxfunc)
TEST_CASE(msgbox_multthread)
TEST_CASES_END
TEST_SUITE(msgbox)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif 
