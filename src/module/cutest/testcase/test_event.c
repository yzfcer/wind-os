/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_event.c
** Author      : Jason Zhou
** Last Date   : 2018/9/30 19:24:52
** Description : Event function test suite
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018/9/30 19:24:52
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
#include "wind_event.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_EVENT_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus



w_event_s *events[4];
static w_uint32_t event_value;
static w_event_cb cb[2];
static w_event_s test_ev;














static void ev_callback1(w_event_s *event,void *arg)
{
    event_value += 10;
}

static void ev_callback2(w_event_s *event,void *arg)
{
    event_value += 20;
}

CASE_SETUP(init) FUNC_EMPTY
CASE_TEARDOWN(init) FUNC_EMPTY
CASE_FUNC(init)
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
    EXPECT_FALSE(IS_F_OBJ_POOL(events[0]->obj));
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


CASE_SETUP(info) FUNC_EMPTY
CASE_TEARDOWN(info) FUNC_EMPTY
CASE_FUNC(info)
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
	EXPECT_TRUE(IS_F_OBJ_POOL(events[0]->obj));
    err = wind_event_destroy(events[0]);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(events[0]->obj.magic,(~WIND_EVENT_MAGIC));

}

CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
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

CASE_SETUP(multi) FUNC_EMPTY
CASE_TEARDOWN(multi)
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
    return W_ERR_OK;
}

CASE_FUNC(multi) 
{

}


SUITE_SETUP(event) FUNC_EMPTY
SUITE_TEARDOWN(event) FUNC_EMPTY
TEST_CASES_START(event)
TEST_CASE(init)
TEST_CASE(info)
TEST_CASE(func)
TEST_CASE(multi)
TEST_CASES_END
TEST_SUITE(event)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
