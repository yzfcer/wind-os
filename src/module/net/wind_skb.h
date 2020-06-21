/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_skb.h
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
#ifndef WIND_SKB_H__
#define WIND_SKB_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

#define NODE_TO_SKB(node) NODEPTR_TO_ITEMPTR(node,w_skb_s,dnode)

#define PROTO_ARP    0x0806
#define PROTO_802_1P 0x8100
#define PROTO_IP     0x0800
#define PROTO_PPPOE  0x8864
#define PROTO_PPP    0x0021

#define PROTO_ICMP   0x01
#define PROTO_IGMP   0x02
#define PROTO_TCP    0x06
#define PROTO_UDP    0x11

typedef struct __w_netdev_s w_netdev_s;

typedef struct 
{
    w_uint32_t vlanid:12;
    w_uint32_t vlanpri:3;
    w_uint32_t vlantci:3;
}w_vlan_s;

typedef struct
{
    w_dnode_s dnode;
    w_uint8_t lay2_idx;
    w_uint8_t lay3_idx;
    w_uint8_t lay4_idx;
    w_uint8_t lay5_idx;
    w_uint16_t packlen;
    w_uint8_t *packbuf;
    w_netdev_s *indev;
    w_netdev_s *outdev;
}w_skb_s;

w_skb_s *wind_skb_alloc(void);
w_err_t wind_skb_free(w_skb_s *skb);
w_err_t wind_skb_alloc_buf(w_skb_s *skb,w_int32_t buflen);

w_uint16_t wind_skb_get_uint16(w_skb_s *skb,w_uint16_t idx);
w_uint32_t wind_skb_get_uint32(w_skb_s *skb,w_uint16_t idx);

void wind_skb_get_ether_srcmac(w_skb_s *skb,w_uint8_t *macaddr);
void wind_skb_get_ether_dstmac(w_skb_s *skb,w_uint8_t *macaddr);
w_uint16_t wind_skb_get_lay3_proto(w_skb_s *skb);
w_bool_t wind_skb_has_vlan(w_skb_s *skb);
w_err_t wind_skb_get_vlan(w_skb_s *skb,w_vlan_s *vlan);

w_uint8_t wind_skb_get_ip_version(w_skb_s *skb);
w_uint8_t wind_skb_get_ip_headlen(w_skb_s *skb);
w_uint8_t wind_skb_get_ip_tos(w_skb_s *skb);
w_uint16_t wind_skb_get_ip_len(w_skb_s *skb);
w_uint16_t wind_skb_get_ip_identification(w_skb_s *skb);
w_uint8_t wind_skb_get_ip_flags(w_skb_s *skb);
w_uint16_t wind_skb_get_ip_offset(w_skb_s *skb);
w_uint8_t wind_skb_get_ip_ttl(w_skb_s *skb);
w_uint8_t wind_skb_get_lay4_proto(w_skb_s *skb);
w_uint16_t wind_skb_get_ip_sum(w_skb_s *skb);
w_uint32_t wind_skb_get_ip_src(w_skb_s *skb);
w_uint32_t wind_skb_get_ip_dest(w_skb_s *skb);

void wind_skb_set_lay3_idx(w_skb_s *skb);
void wind_skb_set_lay4_idx(w_skb_s *skb);

w_uint16_t wind_skb_get_udp_src_port(w_skb_s *skb);
w_uint16_t wind_skb_get_udp_dest_port(w_skb_s *skb);
w_uint16_t wind_skb_get_udp_datalen(w_skb_s *skb);
w_uint16_t wind_skb_get_udp_sum(w_skb_s *skb);

w_uint16_t wind_skb_get_tcp_src_port(w_skb_s *skb);
w_uint16_t wind_skb_get_tcp_dest_port(w_skb_s *skb);
w_uint32_t wind_skb_get_tcp_seqnum(w_skb_s *skb);
w_uint32_t wind_skb_get_tcp_acknum(w_skb_s *skb);
w_uint8_t wind_skb_get_tcp_headlen(w_skb_s *skb);



#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_SKB_H__

