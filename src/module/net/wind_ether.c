/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_ether.c
** Author      : Jason Zhou
** Last Date   : 2020-06-16
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-16
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-16
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_ether.h"
#include "wind_debug.h"
#include "wind_skb.h"
#include "wind_mac.h"
#include "wind_netnode.h"
#include "wind_netdev.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_ETHER_SUPPORT
static w_netnode_s *vlan_node = (w_netnode_s *)W_NULL;
static w_netnode_s *arp_node = (w_netnode_s *)W_NULL;
static w_netnode_s *ip_node = (w_netnode_s *)W_NULL;
static w_err_t ether_handle_req(w_netnode_s *netnode,w_skb_s *skb,w_etherhead_s *etherhead)
{
    return W_ERR_FAIL;
}

static w_err_t ether_handle_ack(w_netnode_s *netnode,w_skb_s *skb,w_etherhead_s *etherhead)
{
    return W_ERR_FAIL;
}

static w_err_t ether_init(w_netnode_s *netnode)
{
    ip_node = wind_netnode_get("ip");
    arp_node = wind_netnode_get("arp");
    vlan_node = wind_netnode_get("vlan");
    return W_ERR_OK;
}
static w_err_t ether_deinit(w_netnode_s *netnode)
{
    ip_node = (w_netnode_s*)W_NULL;
    arp_node = (w_netnode_s*)W_NULL;
    vlan_node = (w_netnode_s*)W_NULL;
    return W_ERR_FAIL;
}

static w_err_t ether_input(w_netnode_s *netnode,w_skb_s *skb)
{
    w_uint8_t dstmac[6];
    w_uint8_t *inmac;
    w_uint16_t l3proto;
    WIND_ASSERT_RETURN(skb != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(skb->packlen > 14,W_ERR_INVALID);
    WIND_ASSERT_RETURN(skb->indev != W_NULL,W_ERR_INVALID);
    wind_skb_get_ether_dstmac(skb,dstmac);
    inmac = skb->indev->param.mac;
    if(!wind_mac_is_equal(inmac,dstmac) &&
        !wind_mac_is_broad(dstmac) &&
        !wind_mac_is_mult(dstmac))
        return W_ERR_DROP_PACK;
    l3proto = wind_skb_get_ether_proto(skb);
    if(l3proto == PROTO_IP)
        return wind_netnode_recv(ip_node,skb);
    else if(l3proto == PROTO_ARP)
        return wind_netnode_recv(arp_node,skb);
    else if(l3proto == PROTO_802_1P)
        return wind_netnode_recv(vlan_node,skb);
    wind_error("proto 0x%4x is NOT supported",l3proto);
    return W_ERR_FAIL;
}

static w_err_t ether_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}



WIND_NETNODE_DEF(ether);
#endif // #if WIND_NET_ETHER_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

