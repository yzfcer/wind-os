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
#include "wind_mac.h"
#include "wind_debug.h"
#include "wind_vlan.h"
#include "wind_ip.h"
#include "wind_mac.h"
#include "wind_conv.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

#define WIND_NETDEV_MAGIC 0x375C6A71
#define NETDEV_MAX_NAMELEN 12

#define IS_F_NETDEV_ENABLE(netdev) IS_F_OBJ_ENABLE(netdev->netnode.obj)
#define SET_F_NETDEV_ENABLE(netdev) SET_F_OBJ_ENABLE(netdev->netnode.obj)
#define CLR_F_NETDEV_ENABLE(netdev) CLR_F_OBJ_ENABLE(netdev->netnode.obj)

#define IS_F_NETDEV_POOL(netdev) IS_F_OBJ_POOL(netdev->netnode.obj)
#define SET_F_NETDEV_POOL(netdev) SET_F_OBJ_POOL(netdev->netnode.obj)
#define CLR_F_NETDEV_POOL(netdev) CLR_F_OBJ_POOL(netdev->netnode.obj)

#define IS_F_NETDEV_HEAP(netdev) IS_F_OBJ_HEAP(netdev->netnode.obj)
#define SET_F_NETDEV_HEAP(netdev) SET_F_OBJ_HEAP(netdev->netnode.obj)
#define CLR_F_NETDEV_HEAP(netdev) CLR_F_OBJ_HEAP(netdev->netnode.obj)

#define F_NETDEV_UNICAST (0x01 << 0) //Mark whether the netdev object is enable unicast
#define IS_F_NETDEV_UNICAST(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_UNICAST) == F_NETDEV_UNICAST)
#define SET_F_NETDEV_UNICAST(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_UNICAST)
#define CLR_F_NETDEV_UNICAST(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_UNICAST))

#define F_NETDEV_BROADCAST (0x01 << 1) //Mark whether the netdev object is enable broadcast
#define IS_F_NETDEV_BROADCAST(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_BROADCAST) == F_NETDEV_BROADCAST)
#define SET_F_NETDEV_BROADCAST(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_BROADCAST)
#define CLR_F_NETDEV_BROADCAST(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_BROADCAST))

#define F_NETDEV_MULTCAST (0x01 << 2) //Mark whether the netdev object is enable unicast
#define IS_F_NETDEV_MULTCAST(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_MULTCAST) == F_NETDEV_MULTCAST)
#define SET_F_NETDEV_MULTCAST(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_MULTCAST)
#define CLR_F_NETDEV_MULTCAST(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_MULTCAST))

#define F_NETDEV_VLAN_EN (0x01 << 3) //Mark whether the netdev object is enable vlan
#define IS_F_NETDEV_VLAN_EN(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_VLAN_EN) == F_NETDEV_VLAN_EN)
#define SET_F_NETDEV_VLAN_EN(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_VLAN_EN)
#define CLR_F_NETDEV_VLAN_EN(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_VLAN_EN))

#define F_NETDEV_IP_READY (0x01 << 4) //Mark whether the netdev object ip address is ready
#define IS_F_NETDEV_IP_READY(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_IP_READY) == F_NETDEV_IP_READY)
#define SET_F_NETDEV_IP_READY(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_IP_READY)
#define CLR_F_NETDEV_IP_READY(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_IP_READY))

#define F_NETDEV_LOCAL (0x01 << 5) //Mark whether the netdev object ip address is a local device
#define IS_F_NETDEV_LOCAL(netdev) ((netdev->netnode.obj.userflag & F_NETDEV_LOCAL) == F_NETDEV_LOCAL)
#define SET_F_NETDEV_LOCAL(netdev) (netdev->netnode.obj.userflag |= F_NETDEV_LOCAL)
#define CLR_F_NETDEV_LOCAL(netdev) (netdev->netnode.obj.userflag &= (~F_NETDEV_LOCAL))


#define WIND_NETDEV_DEF(name) w_netdev_s netdev_##name = {WIND_NETNODE_INFO(name),{{0,0,0,0,0,0},0,0,0,0,IP_STATIC,{0,0,0,0}}};
#define WIND_NETDEV_DECLARE(name) extern w_netdev_s netdev_##name;
#define WIND_NETDEV(name) &netdev_##name
extern struct __w_netdev_s;
typedef struct __w_netdev_s w_netdev_s;

typedef enum
{
    IP_STATIC = 1,
    IP_DHCP   = 2,
    IP_PPPOE  = 3
}ip_type_e;

typedef struct 
{
    w_uint8_t mac[MAC_ADDR_LEN];
    w_uint8_t devid;
    w_uint32_t ip;
    w_uint32_t gw;
    w_uint32_t mask;
    ip_type_e iptype;
    w_vlan_s vlan;
}w_netdev_param_s;

struct __w_netdev_s
{
    w_netnode_s netnode;
    w_netdev_param_s param;
};

w_err_t _wind_netdev_mod_init(void);
w_netdev_s* wind_netdev_get(char *name);
w_netdev_s* wind_netdev_get_byid(w_uint16_t devid);

w_err_t wind_netdev_register(w_netdev_s *netdev);
w_err_t wind_netdev_unregister(w_netdev_s *netdev);
w_err_t wind_netdev_enable(w_netdev_s *netdev);
w_err_t wind_netdev_disable(w_netdev_s *netdev);
w_err_t wind_netdev_print_list(void);


w_err_t wind_netdev_set_ip(w_netdev_s *netdev,w_uint32_t ip,w_uint32_t mask,w_uint32_t gw);
w_err_t wind_netdev_set_vlan(w_netdev_s *netdev,w_vlan_s *vlan);
w_err_t wind_netdev_set_mac(w_netdev_s *netdev,w_uint8_t *mac);
w_err_t wind_netdev_set_iptype(w_netdev_s *netdev,ip_type_e iptype);
w_err_t wind_netdev_print_detail(char *devname);


#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_NETDEV_H__

