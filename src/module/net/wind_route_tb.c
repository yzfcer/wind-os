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
#include "wind_string.h"
#include "wind_core.h"
#include "wind_ip.h"
#include "wind_netdev.h"
//#include "wind_timer.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT
#define IS_DESTIP_EQ(destip,subnet,maskbits) ((destip & (destip & (32 - maskbits))) == subnet)
w_route_tb_s *default_routetb = (w_route_tb_s *)W_NULL;
w_route_tb_s route_tb_list[WIND_ROUTE_TB_MAX_NUM];

static w_route_tb_s *route_tb_find_free(void)
{
    w_int32_t i;
    w_route_tb_s *rtb = (w_route_tb_s*)W_NULL;
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        if(route_tb_list[i].enable == 0)
        {
            rtb = &route_tb_list[i];
            break;
        }
    }
    return rtb;
}

w_err_t wind_route_tb_init(void)
{
    w_err_t err;
    wind_notice("net:init route table");
    err = wind_route_tb_clear();
    return err;
}

w_err_t wind_route_tb_deinit(void)
{
    wind_notice("net:deinit route table");
    return wind_route_tb_clear();
}


w_err_t wind_route_tb_insert(w_route_tb_s *route_tb)
{
    w_err_t err;
    w_route_tb_s *tmp_routetb;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        tmp_routetb = wind_route_tb_get(route_tb->destip);
        if(tmp_routetb == W_NULL)
            tmp_routetb = route_tb_find_free();
        WIND_ASSERT_BREAK(tmp_routetb != W_NULL,W_ERR_MEM,"route_tb table full");
        wind_memcpy(tmp_routetb,route_tb,sizeof(w_route_tb_s));
        tmp_routetb->enable = 1;
        tmp_routetb->destip = (tmp_routetb->destip & (0xffffffff << (32 - tmp_routetb->mask_bits)));
        if(tmp_routetb->is_default)
        {
            if(default_routetb != W_NULL)
                default_routetb->is_default = 0;
            default_routetb = tmp_routetb;
        }
    }while(0);
    wind_enable_switch();
    return err;
}

w_err_t wind_route_tb_remove(w_route_tb_s *route_tb)
{
    w_err_t err;
    w_route_tb_s *tmp_routetb;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        tmp_routetb = wind_route_tb_get(route_tb->destip);
        WIND_ASSERT_BREAK(tmp_routetb != W_NULL,W_ERR_FAIL,"find route table rule fail");
        if(tmp_routetb->mask_bits != route_tb->mask_bits)
            break;
        if(tmp_routetb->next_hop != route_tb->next_hop)
            break;
        if(wind_strcmp(tmp_routetb->dev_name,route_tb->dev_name) != 0)
            break;
        tmp_routetb->enable = 0;
        if(tmp_routetb == default_routetb)
            default_routetb = (w_route_tb_s *)W_NULL;
    }while(0);
    wind_enable_switch();
    return err;
}


w_err_t wind_route_tb_setdefault(char *dev_name)
{
    w_route_tb_s rtb;
    w_netdev_s *netdev;
    netdev = wind_netdev_get(dev_name);
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_INVALID);
    wind_disable_switch();
    if(default_routetb != W_NULL)
        default_routetb->is_default = 0;
    wind_memset(&rtb,0,sizeof(w_route_tb_s));
    rtb.dev_name = netdev->netnode.obj.name;
    rtb.is_default = 1;
    rtb.next_hop = netdev->param.gw;
    wind_route_tb_insert(&rtb);//test here
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_route_tb_clear(void)
{
    w_int32_t i;
    wind_disable_switch();
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        route_tb_list[i].enable = 0;
    }
    wind_enable_switch();
    return W_ERR_OK;
}

#if 0
w_err_t wind_route_tb_flush(void)
{
    w_int32_t i;
    w_uint32_t tick;
    tick = wind_get_tick();
    tick /= WIND_TICK_PER_SEC;
    wind_disable_switch();
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        //if((route_tb_list[i].enable) && (tick - route_tb_list[i].ttl >= WIND_ROUTE_TB_TTL))
        {
            route_tb_list[i].enable = 0;
            break;
        }
    }
    wind_enable_switch();
    return W_ERR_OK;
}
#endif

w_route_tb_s *wind_route_tb_get(w_uint32_t destip)
{
    w_int32_t i;
    w_route_tb_s *route_tb = W_NULL;
    //(destip & (0xffffffff << (32 - tmp_routetb->mask_bits)))
    wind_disable_switch();
    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        if(!route_tb_list[i].enable)
            continue;
        if(IS_DESTIP_EQ(destip,route_tb_list[i].destip,route_tb_list[i].mask_bits))
        {
            route_tb = &route_tb_list[i];
            break;
        }
    }
    wind_enable_switch();
    return route_tb;
}

w_err_t wind_route_tb_print(void)
{
    w_int32_t i;
    w_route_tb_s *rtb;
    char destipstr[16];
    char gwipstr[16];
    rtb = default_routetb;
    wind_printf("route table list:\r\n");
    if(rtb != W_NULL)
    {
        wind_ip_to_str(rtb->destip,destipstr);
        wind_ip_to_str(rtb->next_hop,gwipstr);
        wind_printf("default %s/%d via %s dev %s\r\n",destipstr,
            rtb->mask_bits,gwipstr,rtb->dev_name);
    }

    for(i = 0;i < WIND_ROUTE_TB_MAX_NUM;i ++)
    {
        rtb = &route_tb_list[i];
        if(!rtb->enable)
            continue;
        if(rtb->is_default)
            continue;
        wind_ip_to_str(rtb->destip,destipstr);
        wind_ip_to_str(rtb->next_hop,gwipstr);
        wind_printf("%s/%d via %s dev %s\r\n",destipstr,
            rtb->mask_bits,gwipstr,rtb->dev_name);
    }
    return W_ERR_OK;
}


#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

