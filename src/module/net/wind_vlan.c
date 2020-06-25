/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_vlan.c
** Author      : Jason Zhou
** Last Date   : 2020-06-14
** Descrvlantion : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-14
** Descrvlantion : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-14
** Descrvlantion : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_vlan.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_VLAN_SUPPORT


static w_err_t vlan_init(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t vlan_deinit(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}

static w_err_t vlan_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t vlan_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}

WIND_NETNODE_DEF(vlan);
#endif // #if WIND_NET_VLAN_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

