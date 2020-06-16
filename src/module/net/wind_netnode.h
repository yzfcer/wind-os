/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_netnode.h
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
#ifndef WIND_NETNODE_H__
#define WIND_NETNODE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_skb.h"
#include "wind_debug.h"
#include "wind_net_stati.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#define WIND_NETNODE_MAGIC 0x35FA518C

#define F_NETNODE_ENABLE (0x01 << 0) //Mark whether the netnode object is enable
#define IS_F_NETNODE_ENABLE(netnode) ((netnode->obj.flag & F_NETNODE_ENABLE) == F_NETNODE_ENABLE)
#define SET_F_NETNODE_ENABLE(netnode) (netnode->obj.flag |= F_NETNODE_ENABLE)
#define CLR_F_NETNODE_ENABLE(netnode) (netnode->obj.flag &= (~F_NETNODE_ENABLE))

typedef struct __w_netnode_s w_netnode_s;
struct __w_netnode_s
{
    w_obj_s obj;
    w_net_stati_s stati;
    w_err_t (*input)(w_netnode_s *netnode,w_skb_s *skb);
    w_err_t (*output)(w_netnode_s *netnode,w_skb_s *skb);
};

w_err_t _wind_netnode_mod_init(void);
w_netnode_s* wind_netnode_get(char *name);

w_err_t wind_netnode_register(w_netnode_s *netnode);
w_err_t wind_netnode_unregister(w_netnode_s *netnode);
w_err_t wind_netnode_enable(w_netnode_s *netnode);
w_err_t wind_netnode_disable(w_netnode_s *netnode);

w_err_t wind_netnode_recv(w_netnode_s * netnode,w_skb_s *skb);
w_err_t wind_netnode_send(w_netnode_s * netnode,w_skb_s *skb);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_NETNODE_H__

