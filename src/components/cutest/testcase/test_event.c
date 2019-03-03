/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_event.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/9/30 19:24:52
** ��        ��: �¼���Ԫ����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018/9/30 19:24:52
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************ͷ�ļ�����***********************************************/
#include "wind_cut.h"
#include "wind_event.h"
#if (WIND_CUTEST_SUPPORT && TEST_EVENT_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************�ڲ���������**********************************************/
w_event_s *events[4];
static w_uint32_t event_value;
static w_event_cb cb[2];
static w_event_s test_ev;




/********************************************�ڲ���������*********************************************/


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/


static void ev_callback1(w_event_s *event,void *arg)
{
    event_value += 10;
}

static void ev_callback2(w_event_s *event,void *arg)
{
    event_value += 20;
}

CASE_SETUP(eventinit)
{

}

CASE_TEARDOWN(eventinit)
{

}

CASE_FUNC(eventinit)
{
    w_err_t err;
    err = wind_event_init(&test_ev,"test");
    EXPECT_EQ(err,W_ERR_OK);
    events[0] = wind_event_get("test");
    EXPECT_EQ(events[0],&test_ev);
    EXPECT_EQ(events[0]->obj.magic,WIND_EVENT_MAGIC);
    EXPECT_STR_EQ(events[0]->obj.name,"test");
    
    EXPECT_EQ(events[0]->cblist.head,W_NULL);
    EXPECT_EQ(events[0]->cblist.tail,W_NULL);
    EXPECT_FALSE(IS_F_EVENT_POOL(events[0]));
    err = wind_event_destroy(events[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(events[0]->obj.magic,(~WIND_EVENT_MAGIC));

    err = wind_event_init(&test_ev,W_NULL);
    EXPECT_EQ(err,W_ERR_OK);
    events[0] = wind_event_get(W_NULL);
    EXPECT_EQ(events[0],W_NULL);
    err = wind_event_destroy(&test_ev);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(test_ev.obj.magic,(~WIND_EVENT_MAGIC));

}


CASE_SETUP(eventinfo)
{

}

CASE_TEARDOWN(eventinfo)
{

}

CASE_FUNC(eventinfo)
{
    w_err_t err;
    events[0] = wind_event_create("test");
    EXPECT_NE(events[0],W_NULL);
    events[1] = wind_event_get("test");
    EXPECT_EQ(events[0],events[1]);
    EXPECT_EQ(events[0]->obj.magic,WIND_EVENT_MAGIC);
    EXPECT_STR_EQ(events[0]->obj.name,"test");
    
    EXPECT_EQ(events[0]->cblist.head,W_NULL);
    EXPECT_EQ(events[0]->cblist.tail,W_NULL);
    EXPECT_TRUE(IS_F_EVENT_POOL(events[0]));
    err = wind_event_destroy(events[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(events[0]->obj.magic,(~WIND_EVENT_MAGIC));

}

CASE_SETUP(eventfunc)
{

}

CASE_TEARDOWN(eventfunc)
{

}

CASE_FUNC(eventfunc)
{
    w_err_t err;
    events[0] = wind_event_create("test");
    EXPECT_NE(events[0],W_NULL);
    cb[0].cb_fn = ev_callback1;
    err = wind_event_regcb(events[0],&cb[0]);
    EXPECT_EQ(W_ERR_OK,err);
    cb[1].cb_fn = ev_callback2;
    err = wind_event_regcb(events[0],&cb[1]);
    EXPECT_EQ(W_ERR_OK,err);
    event_value = 0;
    wind_event_trig(events[0],W_NULL);
    EXPECT_EQ(event_value,30);
    
    err = wind_event_unregcb(events[0],&cb[0]);
    EXPECT_EQ(W_ERR_OK,err);
    err = wind_event_unregcb(events[0],&cb[1]);
    EXPECT_EQ(W_ERR_OK,err);
    event_value = 0;
    wind_event_trig(events[0],W_NULL);
    EXPECT_EQ(event_value,0);
    
    err = wind_event_destroy(events[0]);
    EXPECT_EQ(W_ERR_OK,err);

}

CASE_SETUP(eventmulti)
{
    
}

CASE_TEARDOWN(eventmulti)
{
    w_err_t err;
    events[0] = wind_event_create("test1");
    EXPECT_NE(events[0],W_NULL);
    events[1] = wind_event_create("test2");
    EXPECT_NE(events[0],W_NULL);

    
    cb[0].cb_fn = ev_callback1;
    cb[1].cb_fn = ev_callback2;
    err = wind_event_regcb(events[0],&cb[0]);
    EXPECT_EQ(W_ERR_OK,err);
    err = wind_event_regcb(events[1],&cb[1]);
    EXPECT_EQ(W_ERR_OK,err);
    
    event_value = 0;
    wind_event_trig(events[0],W_NULL);
    EXPECT_EQ(event_value,10);
    wind_event_trig(events[1],W_NULL);
    EXPECT_EQ(event_value,30);

    event_value = 0;
    err = wind_event_unregcb(events[0],&cb[0]);
    EXPECT_EQ(W_ERR_OK,err);
    wind_event_trig(events[0],W_NULL);
    EXPECT_EQ(event_value,0);
    wind_event_trig(events[1],W_NULL);
    EXPECT_EQ(event_value,20);
    
    err = wind_event_unregcb(events[1],&cb[1]);
    EXPECT_EQ(W_ERR_OK,err);
    event_value = 0;
    wind_event_trig(events[1],W_NULL);
    EXPECT_EQ(event_value,0);
    
    err = wind_event_destroy(events[0]);
    EXPECT_EQ(W_ERR_OK,err);
    err = wind_event_destroy(events[1]);
    EXPECT_EQ(W_ERR_OK,err);
}

CASE_FUNC(eventmulti)
{


}


SUITE_SETUP(test_event)
{

}

SUITE_TEARDOWN(test_event)
{

}

TEST_CASES_START(test_event)
TEST_CASE(eventinit)
TEST_CASE(eventinfo)
TEST_CASE(eventfunc)
TEST_CASE(eventmulti)
TEST_CASES_END
TEST_SUITE(test_event)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
