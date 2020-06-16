/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_udp.c
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
#include "wind_udp.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

static w_err_t udp_handle_req(w_netnode_s *netnode,w_skb_s *skb,w_udphead_s *udphead)
{
    return W_ERR_FAIL;
}
static w_err_t udp_handle_ack(w_netnode_s *netnode,w_skb_s *skb,w_udphead_s *udphead)
{
    return W_ERR_FAIL;
}
static w_err_t udp_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t udp_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

