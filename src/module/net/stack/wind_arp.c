/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_arp.c
** Author      : Jason Zhou
** Last Date   : 2020-06-14
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-14
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-14
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_arp.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_ARP_SUPPORT

static w_err_t arp_handle_req(w_netnode_s *netnode,w_skb_s *skb,w_arp_s *arp)
{
    return W_ERR_FAIL;
}
static w_err_t arp_handle_ack(w_netnode_s *netnode,w_skb_s *skb,w_arp_s *arp)
{
    return W_ERR_FAIL;
}

static w_err_t arp_init(w_netnode_s *netnode)
{
    return W_ERR_OK;
}
static w_err_t arp_deinit(w_netnode_s *netnode)
{
    return W_ERR_OK;
}
static w_err_t arp_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t arp_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}


WIND_NETNODE_DEF(arp);
#endif // #if WIND_NET_ARP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

