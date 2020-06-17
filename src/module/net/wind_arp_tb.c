/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_arp_tb.c
** Author      : Jason Zhou
** Last Date   : 2020-06-17
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-17
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-17
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_arp_tb.h"
#include "wind_timer.h"
#include "wind_string.h"
#include "wind_core.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
w_arp_tb_s arp_tb_list[WIND_ARP_TB_MAX_NUM];
static void arp_tb_timer(w_timer_s *timer,void *arg)
{
    wind_arp_tb_flush();
}

w_err_t wind_arp_tb_init(void)
{
    w_err_t err;
    w_timer_s *tmr;
    err = wind_arp_tb_clear();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    tmr = wind_timer_create("arp_tb_flush",2000,arp_tb_timer,W_NULL,F_TIMER_RUN | F_TIMER_REPEAT);
    WIND_ASSERT_RETURN(tmr != W_NULL,err);
    return err;
}

w_err_t wind_arp_tb_deinit(void)
{
    w_timer_s *tmr;
    tmr = wind_timer_get("arp_tb_flush");
    if(tmr != W_NULL)
        wind_timer_destroy(tmr);
    return W_ERR_OK;
}

w_err_t wind_arp_tb_update(w_arp_tb_s *arp_tb)
{
    w_int32_t i;
    w_err_t err;
    w_arp_tb_s *tmp_arptb;
    w_uint32_t tick;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        wind_disable_switch();
        tmp_arptb = wind_arp_tb_get(arp_tb->ipaddr);
        if(tmp_arptb == W_NULL)
        {
            for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
            {
                if(arp_tb_list[i].enable == 0)
                {
                    arp_tb_list[i].enable = 1;
                    tmp_arptb = &arp_tb_list[i];
                    break;
                }
            }
        }
        WIND_ASSERT_BREAK(tmp_arptb != W_NULL,W_ERR_MEM,"arp_tb table full");
        tick = wind_get_tick();
        tick /= WIND_TICK_PER_SEC;
        wind_memcpy(tmp_arptb->mac,arp_tb->mac,6);
        tmp_arptb->ipaddr = arp_tb->ipaddr;
        tmp_arptb->hwtype = arp_tb->hwtype;
        tmp_arptb->flags = arp_tb->flags;
        tmp_arptb->ttl = tick;
    }while(0);
    wind_enable_switch();
    return err;
}

w_err_t wind_arp_tb_clear(void)
{
    w_int32_t i;
    wind_disable_switch();
    for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
    {
        arp_tb_list[i].enable = 0;
    }
    wind_disable_switch();
    return W_ERR_OK;

}

w_err_t wind_arp_tb_flush(void)
{
    w_int32_t i;
    w_uint32_t tick;
    tick = wind_get_tick();
    tick /= WIND_TICK_PER_SEC;
    wind_disable_switch();
    for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
    {
        if((arp_tb_list[i].enable) && (tick - arp_tb_list[i].ttl >= WIND_ARP_TB_TTL))
        {
            arp_tb_list[i].enable = 0;
            break;
        }
    }
    wind_disable_switch();
    return W_ERR_OK;
}

w_arp_tb_s *wind_arp_tb_get(w_uint32_t ipaddr)
{
    w_int32_t i;
    w_arp_tb_s *arp_tb = W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
    {
        if((arp_tb_list[i].enable) && (ipaddr==arp_tb_list[i].ipaddr))
        {
            arp_tb = &arp_tb_list[i];
            break;
        }
    }
    wind_disable_switch();
    return arp_tb;
}


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

