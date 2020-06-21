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
#define NETNODE_HOOK_NULL {W_NULL,W_NULL},{W_NULL,W_NULL}
#define WIND_NETNODE_INFO(name) {WIND_OBJ(~WIND_NETNODE_MAGIC,0,#name),NET_STATI_NULL,NETNODE_HOOK_NULL,name##_init,name##_deinit,name##_input,name##_output}
#define WIND_NETNODE_DEF(name) w_netnode_s netnode_##name = WIND_NETNODE_INFO(name)
#define WIND_NETNODE_DECLARE(name) extern w_netnode_s netnode_##name;
#define NETNODE(name) &netnode_##name

typedef struct __w_netnode_s w_netnode_s;
struct __w_netnode_s
{
    w_obj_s obj;
    w_net_stati_s stati;
    w_dlist_s inhook_list;
    w_dlist_s outhook_list;
    w_err_t (*init)(w_netnode_s *netnode);
    w_err_t (*deinit)(w_netnode_s *netnode);
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

