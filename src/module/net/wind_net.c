/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_net.c
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
#include "wind_net.h"
#include "wind_netnode.h"
#include "wind_netdev.h"
#include "wind_arp_tb.h"
#include "wind_fdb.h"
#include "wind_cmd.h"
#include "wind_route_tb.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

#if WIND_NET_ETHER_SUPPORT
WIND_NETNODE_DECLARE(ether);
#endif
#if WIND_NET_VLAN_SUPPORT
WIND_NETNODE_DECLARE(vlan);
#endif
#if WIND_NET_ARP_SUPPORT
WIND_NETNODE_DECLARE(arp);
#endif
#if WIND_NET_ICMP_SUPPORT
WIND_NETNODE_DECLARE(icmp);
#endif
#if WIND_NET_IGMP_SUPPORT
WIND_NETNODE_DECLARE(igmp);
#endif
#if WIND_NET_IP_SUPPORT
WIND_NETNODE_DECLARE(ip);
#endif
#if WIND_NET_TCP_SUPPORT
WIND_NETNODE_DECLARE(tcp);
#endif
#if WIND_NET_UDP_SUPPORT
WIND_NETNODE_DECLARE(udp);
#endif
#if WIND_NET_DHCP_SUPPORT
WIND_NETNODE_DECLARE(dhcp);
#endif
#if WIND_NET_DNS_SUPPORT
WIND_NETNODE_DECLARE(dns);
#endif
#if WIND_NET_TFTP_SUPPORT
WIND_NETNODE_DECLARE(tftp);
#endif

static w_err_t wind_netnodes_regi(void)
{
#if WIND_NET_ETHER_SUPPORT
    wind_netnode_register(NETNODE(ether));
#endif
#if WIND_NET_VLAN_SUPPORT
    wind_netnode_register(NETNODE(vlan));
#endif
#if WIND_NET_ARP_SUPPORT
    wind_netnode_register(NETNODE(arp));
#endif
#if WIND_NET_ICMP_SUPPORT
    wind_netnode_register(NETNODE(icmp));
#endif
#if WIND_NET_IGMP_SUPPORT
    wind_netnode_register(NETNODE(igmp));
#endif
#if WIND_NET_IP_SUPPORT
    wind_netnode_register(NETNODE(ip));
#endif
#if WIND_NET_TCP_SUPPORT
    wind_netnode_register(NETNODE(tcp));
#endif
#if WIND_NET_UDP_SUPPORT
    wind_netnode_register(NETNODE(udp));
#endif
#if WIND_NET_DHCP_SUPPORT
    wind_netnode_register(NETNODE(dhcp));
#endif
#if WIND_NET_DNS_SUPPORT
    wind_netnode_register(NETNODE(dns));
#endif
#if WIND_NET_TFTP_SUPPORT
    wind_netnode_register(NETNODE(tftp));
#endif
    return W_ERR_OK;
}

static w_err_t wind_netnodes_unregi(void)
{
#if WIND_NET_ARP_SUPPORT
    wind_netnode_unregister(NETNODE(arp));
#endif
#if WIND_NET_ICMP_SUPPORT
    wind_netnode_unregister(NETNODE(icmp));
#endif
#if WIND_NET_IGMP_SUPPORT
    wind_netnode_unregister(NETNODE(igmp));
#endif
#if WIND_NET_IP_SUPPORT
    wind_netnode_unregister(NETNODE(ip));
#endif
#if WIND_NET_TCP_SUPPORT
    wind_netnode_unregister(NETNODE(tcp));
#endif
#if WIND_NET_UDP_SUPPORT
    wind_netnode_unregister(NETNODE(udp));
#endif
#if WIND_NET_DHCP_SUPPORT
    wind_netnode_unregister(NETNODE(dhcp));
#endif
#if WIND_NET_DNS_SUPPORT
    wind_netnode_unregister(NETNODE(dns));
#endif
#if WIND_NET_TFTP_SUPPORT
    wind_netnode_unregister(NETNODE(tftp));
#endif
    return W_ERR_OK;
}


#if CMD_IFCONFIG_SUPPORT
COMMAND_DECLARE(ifconfig);
#endif
#if CMD_ROUTE_SUPPORT
COMMAND_DECLARE(route);
#endif
w_err_t wind_netcmd_regi(void)
{
#if CMD_IFCONFIG_SUPPORT
    wind_cmd_register(COMMAND(ifconfig));
#endif
#if CMD_ROUTE_SUPPORT
    wind_cmd_register(COMMAND(route));
#endif
    return W_ERR_OK;
}

w_err_t wind_netcmd_unregi(void)
{
#if CMD_IFCONFIG_SUPPORT
    wind_cmd_unregister(COMMAND(ifconfig));
#endif
#if CMD_ROUTE_SUPPORT
    wind_cmd_unregister(COMMAND(route));
#endif
    return W_ERR_OK;
}

w_err_t _wind_net_mod_init(void)
{
    w_err_t err = W_ERR_OK;
    wind_route_tb_init();
    wind_arp_tb_init();
    wind_fdb_init();
    err = wind_netnodes_regi();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = _wind_netdev_mod_init();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_netcmd_regi();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

w_err_t _wind_net_mod_deinit(void)
{
    w_err_t err;
    err = wind_netcmd_unregi();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_netnodes_unregi();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    wind_route_tb_deinit();
    wind_arp_tb_deinit();
    wind_fdb_deinit();
    return err;
}


#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

