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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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
w_err_t _wind_net_mod_init(void)
{
    w_err_t err;
    err = wind_netnodes_regi();
    return err;
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

