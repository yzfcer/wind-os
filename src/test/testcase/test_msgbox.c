/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_msgbox.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2017/10/22 16:29:55
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: ��Ԫ���������Ĳ���ģ��
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/
#include "cut.h"
#include "wind_mpool.h"
#include "wind_thread.h"
#include "wind_msgbox.h"
/********************************************�ڲ���������**********************************************/
static msgbox_s *msgbox;
typedef struct 
{
    msg_s msg;
    int value;
}test_msg_s;

WIND_MPOOL(testmsg,6,sizeof(test_msg_s));

/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/

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
