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
#if WIND_MODULE_NET_SUPPORT
static w_dlist_s skb_list;
static w_netnode_s *ether_node = W_NULL;
WIND_NETDEV_DECLARE(lo);

static w_err_t lo_init(w_netnode_s *netnode)
{
    w_netdev_s *netdev;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    ether_node = wind_netnode_get("ether");
    WIND_ASSERT_RETURN(ether_node != W_NULL,W_ERR_FAIL);
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
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    netdev = (w_netdev_s *)netnode;
    netdev->param.ip = 0;
    netdev->param.mask = 0;
    netdev->param.gw = 0;
    return W_ERR_OK;
}

static w_err_t lo_input(w_netnode_s *netnode,w_skb_s *skb)
{
    w_dnode_s *dnode;
	w_skb_s *skb1;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    wind_disable_switch();
    dnode = dlist_remove_head(&skb_list);
    wind_enable_switch();
    skb1 = NODE_TO_SKB(dnode);
    WIND_CHECK_RETURN(skb1 != W_NULL,W_ERR_FAIL);
    wind_memcpy(skb,skb1,sizeof(w_skb_s));
    skb->indev = WIND_NETDEV(lo);
    skb1->packbuf = W_NULL;
    wind_skb_free(skb1);
    if(ether_node != W_NULL)
        wind_netnode_recv(ether_node,skb);
    return W_ERR_OK;
}

static w_err_t lo_output(w_netnode_s *netnode,w_skb_s *skb)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    wind_disable_switch();
    dlist_insert_tail(&skb_list,&skb->dnode);
    wind_enable_switch();
    return W_ERR_OK;
}


WIND_NETDEV_DEF(lo);


#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

