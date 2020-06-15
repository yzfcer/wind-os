/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_netdev.h
** Author      : Jason Zhou
** Last Date   : 2020-06-15
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-15
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-15
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#ifndef WIND_NETDEV_H__
#define WIND_NETDEV_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef struct __w_netdev_s w_netdev_s;
struct __w_netdev_s
{
    w_obj_s obj;
    w_uint32_t ip;
    w_uint32_t gw;
    w_uint32_t mask;
    w_vlan_s vlan;
    
    w_err_t (*init)(w_netdev_s *netdev);
    w_err_t (*input)(w_netdev_s *netdev,w_skb_s **skb);
    w_err_t (*output)(w_netdev_s *netdev,w_skb_s *skb);
};

w_err_t _wind_netdev_mod_init(void);
w_netdev_s* wind_netdev_get(char *name);

w_err_t wind_netdev_register(w_netdev_s *netdev);
w_err_t wind_netdev_unregister(w_netdev_s *netdev);
w_err_t wind_netdev_enable(w_netdev_s *netdev);
w_err_t wind_netdev_disable(w_netdev_s *netdev);



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_NETDEV_H__

