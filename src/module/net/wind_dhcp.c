/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dhcp.c
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
#include "wind_dhcp.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

static w_err_t dhcp_init(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t dhcp_deinit(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}

static w_err_t dhcp_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t dhcp_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}

w_err_t wind_dhcp_get_opt(w_dhcp_ops_s *opt,w_skb_s *skb,w_int32_t idx)
{
    opt->optid = skb->packbuf[idx];
    opt->lenth = skb->packbuf[idx+1];
    opt->data = wind_netmem_malloc(opt->lenth);
    WIND_ASSERT_RETURN(opt->data != W_NULL,W_ERR_MEM);
    wind_memcpy(opt->data,&skb->packbuf[idx+2],opt->lenth);
    return W_ERR_OK;
}

WIND_NETNODE_DEF(dhcp);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

