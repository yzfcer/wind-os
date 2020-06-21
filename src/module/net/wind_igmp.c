/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_igmp.c
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
#include "wind_igmp.h"
#include "wind_debug.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_IGMP_SUPPORT

static w_err_t igmp_handle_req(w_netnode_s *netnode,w_skb_s *skb,w_igmphead_s *igmphead)
{
    return W_ERR_FAIL;
}

static w_err_t igmp_handle_ack(w_netnode_s *netnode,w_skb_s *skb,w_igmphead_s *igmphead)
{
    return W_ERR_FAIL;
}
static w_err_t igmp_init(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t igmp_deinit(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}

static w_err_t igmp_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t igmp_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}



WIND_NETNODE_DEF(igmp);
#endif // #if WIND_NET_IGMP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

