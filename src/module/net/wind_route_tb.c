/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_route_tb.c
** Author      : Jason Zhou
** Last Date   : 2020-06-18
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-18
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-18
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_route_tb.h"
#include "wind_debug.h"
//#include "wind_timer.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


w_route_tb_s route_tb_list[WIND_ROUTE_TB_MAX_NUM];
#if 0
static void route_tb_timer(w_timer_s *timer,void *arg)
{
    //wind_route_tb_flush();
}
#endif

w_err_t wind_route_tb_init(void)
{
    w_err_t err;
    //w_timer_s *tmr;
    err = wind_route_tb_clear();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    //tmr = wind_timer_create("route_tb_flush",2000,route_tb_timer,W_NULL,F_TIMER_RUN | F_TIMER_REPEAT);
    //WIND_ASSERT_RETURN(tmr != W_NULL,err);
    return err;
}
?
w_err_t wind_route_tb_deinit(void)
{
    //w_timer_s *tmr;
    //tmr = wind_timer_get("route_tb_flush");
    //if(tmr != W_NULL)
    //    wind_timer_destroy(tmr);
    return W_ERR_OK;
}


w_err_t wind_route_tb_insert(w_route_tb_s *route_tb)
{
    w_int32_t i;
    w_err_t err;
    w_route_tb_s *tmp_routetb;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        wind_disable_switch();
        tmp_routetb = wind_route_tb_get(route_tb->destip);
        if(tmp_routetb == W_NULL)
        {
            for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
            {
                if(route_tb_list[i].enable == 0)
                {
                    route_tb_list[i].enable = 1;
                    tmp_routetb = &route_tb_list[i];
                    break;
                }
            }
        }
        WIND_ASSERT_BREAK(tmp_routetb != W_NULL,W_ERR_MEM,"route_tb table full");
        wind_memcpy(tmp_routetb,route_tb,sizeof(w_route_tb_s));
    }while(0);
    wind_enable_switch();
    return err;
}

w_err_t wind_route_tb_clear(void)
{
    w_int32_t i;
    wind_disable_switch();
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        route_tb_list[i].enable = 0;
    }
    wind_disable_switch();
    return W_ERR_OK;
}

w_err_t wind_route_tb_flush(void)
{
    w_int32_t i;
    w_uint32_t tick;
    tick = wind_get_tick();
    tick /= WIND_TICK_PER_SEC;
    wind_disable_switch();
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        if((route_tb_list[i].enable) && (tick - route_tb_list[i].ttl >= WIND_route_tb_TTL))
        {
            route_tb_list[i].enable = 0;
            break;
        }
    }
    wind_disable_switch();
    return W_ERR_OK;
}

w_route_tb_s *wind_route_tb_get(w_uint32_t destip)
{
    w_int32_t i;
    w_route_tb_s *route_tb = W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        if((route_tb_list[i].enable) && (destip==route_tb_list[i].destip))
        {
            route_tb = &route_tb_list[i];
            break;
        }
    }
    wind_disable_switch();
    return route_tb;
}


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

