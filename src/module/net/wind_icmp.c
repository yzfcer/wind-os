/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_icmp.c
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
#include "wind_icmp.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

static w_err_t icmp_handle_req(w_netnode_s *netnode,w_skb_s *skb,w_icmphead_s *icmphead)
{
    return W_ERR_FAIL;
}
static w_err_t icmp_handle_ack(w_netnode_s *netnode,w_skb_s *skb,w_icmphead_s *icmphead)
{
    return W_ERR_FAIL;
}
static w_err_t icmp_init(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t icmp_deinit(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t icmp_input(w_netnode_s *netnode,w_skb_s **skb)
{
    return W_ERR_FAIL;
}
static w_err_t icmp_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}



WIND_NETNODE_DEF(icmp);
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

