/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dns.c
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
#include "wind_dns.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_DNS_SUPPORT

static w_err_t dns_init(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t dns_deinit(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t dns_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t dns_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}



WIND_NETNODE_DEF(dns);
#endif // #if WIND_NET_DNS_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

