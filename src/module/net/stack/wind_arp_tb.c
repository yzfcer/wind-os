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
#include "wind_ip.h"
#include "wind_mac.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_ARP_SUPPORT
static w_uint32_t arp_tb_ttl = WIND_ARP_TB_TTL;
w_arp_tb_s arp_tb_list[WIND_ARP_TB_MAX_NUM];
static void arp_tb_timer(w_timer_s *timer,void *arg)
{
    wind_arp_tb_flush();
}

w_arp_tb_s *find_free_arp_tb(void)
{
    w_int32_t i;
    w_arp_tb_s *new_arptb = (w_arp_tb_s*)W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
    {
        if(arp_tb_list[i].enable)
            continue;
        arp_tb_list[i].enable = 1;
        new_arptb = &arp_tb_list[i];
        break;
    }
    wind_enable_switch();
    return new_arptb;
}

w_err_t wind_arp_tb_init(void)
{
    w_err_t err;
    w_timer_s *tmr;
    wind_notice("net:init arp table");
    err = wind_arp_tb_clear();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    tmr = wind_timer_create("arp",2000,arp_tb_timer,W_NULL,F_TIMER_RUN | F_TIMER_REPEAT);
    WIND_ASSERT_RETURN(tmr != W_NULL,err);
    return err;
}

w_err_t wind_arp_tb_deinit(void)
{
    w_timer_s *tmr;
    wind_notice("net:deinit arp table");
    tmr = wind_timer_get("arp_tb_flush");
    if(tmr != W_NULL)
        wind_timer_destroy(tmr);
    return W_ERR_OK;
}



w_err_t wind_arp_tb_insert(w_arp_tb_s *arp_tb)
{
    w_err_t err;
    w_arp_tb_s *new_arptb;
    w_uint32_t tick;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        new_arptb = wind_arp_tb_get(arp_tb->ipaddr);
        if(new_arptb == W_NULL)
            new_arptb = find_free_arp_tb();
        WIND_ASSERT_BREAK(new_arptb != W_NULL,W_ERR_MEM,"get arp_tb obj fail");
        tick = wind_get_tick();
        tick /= WIND_TICK_PER_SEC;
        wind_memcpy(new_arptb->mac,arp_tb->mac,6);
        new_arptb->ipaddr = arp_tb->ipaddr;
        new_arptb->hwtype = arp_tb->hwtype;
        new_arptb->flags = arp_tb->flags;
        new_arptb->ttl = (w_uint16_t)tick;
    }while(0);
    wind_enable_switch();
    return err;
}

w_err_t wind_arp_tb_remove(w_arp_tb_s *arp_tb)
{
    w_err_t err;
    w_arp_tb_s *tmp_arptb;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        tmp_arptb = wind_arp_tb_get(arp_tb->ipaddr);
        WIND_ASSERT_BREAK(tmp_arptb != W_NULL,W_ERR_FAIL,"find arp table rule fail");
        if(wind_memcmp(tmp_arptb->mac,arp_tb->mac,MAC_ADDR_LEN) != 0)
            break;
        tmp_arptb->enable = 0;
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
    wind_enable_switch();
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
        if(!arp_tb_list[i].enable)
            continue;
        if(arp_tb_list[i].is_static)
            continue;
        if(tick - arp_tb_list[i].ttl >= arp_tb_ttl)
        {
            arp_tb_list[i].enable = 0;
            break;
        }
    }
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_arp_tb_set_ttl(w_uint32_t ttl)
{
    w_err_t err;
    w_timer_s *timer;
    if(ttl > WIND_ARP_TB_MAX_TTL)
        ttl = WIND_ARP_TB_MAX_TTL;
    arp_tb_ttl = ttl;
    timer = wind_timer_get("arp");
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_FAIL);
    err = wind_timer_set_period(timer,arp_tb_ttl);
    return err;
}

w_int32_t wind_arp_tb_get_ttl(void)
{
    return arp_tb_ttl;
}


w_arp_tb_s *wind_arp_tb_get(w_uint32_t ipaddr)
{
    w_int32_t i;
    w_arp_tb_s *arp_tb = (w_arp_tb_s*)W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
    {
        if(!arp_tb_list[i].enable)
            continue;
        if(ipaddr==arp_tb_list[i].ipaddr)
        {
            arp_tb = &arp_tb_list[i];
            break;
        }
    }
    wind_enable_switch();
    return arp_tb;
}

w_err_t wind_arp_tb_print(void)
{
    w_int32_t i;
    w_arp_tb_s *arpt;
    char macstr[20];
    char ipstr[16];
    wind_print_space(7);
    wind_printf("%-20s %-16s %-6s %-12s\r\n","mac_addr","ip_addr","hwtype","dev");
    wind_print_space(7);
    for(i = 0;i < WIND_ARP_TB_MAX_NUM;i ++)
    {
        if(!arp_tb_list[i].enable)
            continue;
        arpt = &arp_tb_list[i];
        wind_mac_to_str(arpt->mac,macstr,':');
        wind_ip_to_str(arpt->ipaddr,ipstr);
        wind_printf("%-20s %-16s %-6d %-12s",macstr,ipstr,arpt->hwtype,arpt->dev_name);
    }
    wind_print_space(7);
    return W_ERR_OK;
}

#endif // #if WIND_NET_ARP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

