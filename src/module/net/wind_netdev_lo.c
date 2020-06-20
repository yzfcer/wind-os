/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_netdev_lo.c
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
#include "wind_netdev_lo.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_core.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
static w_dlist_s skb_list;
static w_err_t lo_init(w_netnode_s *netnode)
{
    w_netdev_s *netdev;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    netdev = (w_netdev_s *)netnode;
    wind_memset(netdev->param.mac,0,sizeof(netdev->param.mac));
    netdev->param.ip = 0x7F000001;
    netdev->param.mask = 0xFF000000;
    netdev->param.gw = 0x7F000001;
    return W_ERR_OK;
}
static w_err_t lo_deinit(w_netnode_s *netnode)
{
    w_netdev_s *netdev;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    netdev = (w_netdev_s *)netnode;
    netdev->param.ip = 0;
    netdev->param.mask = 0;
    netdev->param.gw = 0;
    return W_ERR_OK;
}

static w_err_t lo_input(w_netnode_s *netnode,w_skb_s **skb)
{
    w_dnode_s *dnode;
	w_skb_s *skb1;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    wind_disable_switch();
    dnode = dlist_remove_head(&skb_list);
    wind_enable_switch();
    skb1 = NODE_TO_SKB(dnode);
	*skb = skb1;
    return W_ERR_OK;
}

static w_err_t lo_output(w_netnode_s *netnode,w_skb_s *skb)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    wind_disable_switch();
    dlist_insert_tail(&skb_list,&skb->dnode);
    wind_enable_switch();
    return W_ERR_OK;
}


WIND_NETDEV_DEF(lo);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

