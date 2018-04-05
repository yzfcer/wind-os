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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
#include "cut.h"
#include "wind_mpool.h"
#include "wind_thread.h"
#include "wind_msgbox.h"
/********************************************内部变量定义**********************************************/
static msgbox_s *msgbox;
typedef struct 
{
    msg_s msg;
    int value;
}test_msg_s;

WIND_MPOOL(testmsg,6,sizeof(test_msg_s));

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
    msgbox = wind_msgbox_create("test",thr);
    EXPECT_NE(msgbox,NULL);
    EXPECT_EQ(msgbox->magic,WIND_MSGBOX_MAGIC);
    EXPECT_EQ(msgbox->num,0);
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
#if 0
    w_err_t err;
    msgbox = wind_msgbox_create("test");
    EXPECT_NE(msgbox,NULL);
    err = wind_msgbox_close(msgbox);
    EXPECT_EQ(ERR_OK,err);
    EXPECT_EQ(msgbox->msgboxed,B_TRUE);
    err = wind_msgbox_open(msgbox);
    EXPECT_EQ(ERR_OK,err);
    EXPECT_EQ(msgbox->msgboxed,B_FALSE);
    EXPECT_EQ(msgbox->waitlist.head,NULL);
    EXPECT_EQ(msgbox->waitlist.tail,NULL);
    err = wind_msgbox_free(msgbox);
    EXPECT_EQ(ERR_OK,err);
#endif
}



SUITE_SETUP(test_msgbox)
{

}

SUITE_TEARDOWN(test_msgbox)
{

}


TEST_CASES_START(test_msgbox)
TEST_CASE(msgboxinfo)
TEST_CASE(msgboxfunc)
//TEST_CASE(msgboxmulti)
TEST_CASES_END
TEST_SUITE(test_msgbox)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
