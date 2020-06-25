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
#ifndef WIND_NETDEV_H__
#define WIND_NETDEV_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_netnode.h"
#include "wind_skb.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

#define WIND_NETDEV_MAGIC 0x375C6A71

#define F_NETDEV_UNICAST (0x01 << 0) //Mark whether the object is enable unicast
#define IS_F_NETDEV_UNICAST(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_UNICAST) == F_NETDEV_UNICAST)
#define SET_F_NETDEV_UNICAST(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_UNICAST)
#define CLR_F_NETDEV_UNICAST(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_UNICAST))

#define F_NETDEV_BROADTCAST (0x01 << 1) //Mark whether the object is enable broadcast
#define IS_F_NETDEV_BROADTCAST(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_BROADTCAST) == F_NETDEV_BROADTCAST)
#define SET_F_NETDEV_BROADTCAST(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_BROADTCAST)
#define CLR_F_NETDEV_BROADTCAST(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_BROADTCAST))

#define F_NETDEV_MULTCAST (0x01 << 2) //Mark whether the object is enable unicast
#define IS_F_NETDEV_MULTCAST(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_MULTCAST) == F_NETDEV_MULTCAST)
#define SET_F_NETDEV_MULTCAST(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_MULTCAST)
#define CLR_F_NETDEV_MULTCAST(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_MULTCAST))


#define WIND_NETDEV_DEF(name) w_netdev_s netdev_##name = {WIND_NETNODE_INFO(name),{{0,0,0,0,0,0},0,0,0}};
#define WIND_NETDEV_DECLARE(name) extern w_netdev_s netdev_##name;
#define WIND_NETDEV(name) &netdev_##name
extern struct __w_netdev_s;
typedef struct __w_netdev_s w_netdev_s;
typedef struct 
{
    w_uint8_t mac[6];
    w_uint32_t ip;
    w_uint32_t gw;
    w_uint32_t mask;
}w_netdev_param_s;

struct __w_netdev_s
{
    w_netnode_s netnode;
    w_netdev_param_s param;
};

w_err_t _wind_netdev_mod_init(void);
w_netdev_s* wind_netdev_get(char *name);

w_err_t wind_netdev_register(w_netdev_s *netdev);
w_err_t wind_netdev_unregister(w_netdev_s *netdev);
w_err_t wind_netdev_enable(w_netdev_s *netdev);
w_err_t wind_netdev_disable(w_netdev_s *netdev);
w_err_t wind_netdev_print_list(void);


#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_NETDEV_H__

