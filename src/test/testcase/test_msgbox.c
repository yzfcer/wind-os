/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_msgbox.c
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
/*********************************************头文件定义***********************************************/
#include "wind_cut.h"
#include "wind_pool.h"
#include "wind_thread.h"
#include "wind_msgbox.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_MSGBOX_SUPPORT
/********************************************内部变量定义**********************************************/
static msgbox_s *msgbox;
typedef struct 
{
    msg_s msg;
    int value;
}test_msg_s;

WIND_POOL(testmsg,6,sizeof(test_msg_s));
WIND_POOL(testmsg_pool,4,sizeof(test_msg_s));

/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

CASE_SETUP(msgboxinfo)
{

}

CASE_TEARDOWN(msgboxinfo)
{

}

CASE_FUNC(msgboxinfo)
{
    w_err_t err;
    thread_s *thr;
    thr = wind_thread_current();
    msgbox = wind_msgbox_create("test");
    EXPECT_NE(msgbox,NULL);
    EXPECT_EQ(msgbox->magic,WIND_MSGBOX_MAGIC);
    EXPECT_EQ(msgbox->msgnum,0);
    EXPECT_EQ(msgbox->msglist.head,NULL);
    EXPECT_EQ(msgbox->msglist.tail,NULL);
    EXPECT_EQ(msgbox->owner,thr);
    err = wind_msgbox_destroy(msgbox);
    EXPECT_EQ(ERR_OK,err);
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
    msg_s *msg;
    test_msg_s *tmsg,*tmsg1;
    tmsg1 = wind_pool_malloc(testmsg_pool);
    msgbox = wind_msgbox_create("test");
    err = wind_msgbox_wait(msgbox,&msg,100);
    EXPECT_EQ(err,ERR_TIMEOUT);
    wind_msg_init(&tmsg1->msg,1,sizeof(test_msg_s),tmsg1);
    tmsg1->value = 1;
    err = wind_msgbox_post(msgbox,&tmsg1->msg);
    EXPECT_EQ(err,ERR_OK);
    err = wind_msgbox_wait(msgbox,&msg,100);
    EXPECT_EQ(err,ERR_OK);
    tmsg = (test_msg_s *)GET_MSG(msg,test_msg_s,msg.msgnode);
    EXPECT_EQ(tmsg->msg.msg_id,1);
    EXPECT_EQ(tmsg->msg.msg_len,sizeof(test_msg_s));
    EXPECT_EQ(tmsg->msg.msg_arg,tmsg1);
    err = wind_msgbox_wait(msgbox,&msg,100);
    EXPECT_EQ(err,ERR_TIMEOUT);
    wind_pool_free(testmsg_pool,tmsg1);
    tmsg1 = NULL;
    err = wind_msgbox_destroy(msgbox);
    EXPECT_EQ(ERR_OK,err);
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
SUITE_SETUP(test_msgbox)
{
    wind_pool_create("testmsg_pool",testmsg_pool,sizeof(testmsg_pool),sizeof(test_msg_s));
}

SUITE_TEARDOWN(test_msgbox)
{
    wind_pool_destroy(testmsg_pool);
}


TEST_CASES_START(test_msgbox)
TEST_CASE(msgboxinfo)
TEST_CASE(msgboxfunc)
TEST_CASE(msgbox_multthread)
TEST_CASES_END
TEST_SUITE(test_msgbox)

#endif 
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
