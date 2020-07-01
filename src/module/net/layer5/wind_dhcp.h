/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dhcp.h
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
#ifndef WIND_DHCP_H__
#define WIND_DHCP_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_skb.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_DHCP_SUPPORT

#define  DHCPTYPE_DISCOVER 0x01
#define  DHCPTYPE_OFFER    0x02
#define  DHCPTYPE_REQUEST  0x03
#define  DHCPTYPE_DECLINE  0x04
#define  DHCPTYPE_ACK      0x05
#define  DHCPTYPE_NAK      0x06
#define  DHCPTYPE_RELEASE  0x07
#define  DHCPTYPE_INFORM   0x08

typedef struct
{
    w_uint8_t optype;
    w_uint8_t htype;
    w_uint8_t hlen;
    w_uint8_t hops;
    w_uint32_t trans_id;
    w_uint16_t seconds;
    w_uint16_t flags;
    w_uint32_t cli_addr;
    w_uint32_t your_addr;
    w_uint32_t next_addr;
    w_uint32_t relay_addr;
    w_uint8_t  cli_mac[16];
    w_uint8_t  *server_name; //64 bytes
    w_uint8_t  *file;//128 bytes

}w_dhcphead_s;

typedef struct 
{
    w_uint8_t optid;
    w_uint8_t lenth;
    w_uint8_t *data;
}w_dhcp_ops_s;

w_err_t wind_dhcp_get_opt(w_dhcp_ops_s *opt,w_skb_s *skb,w_int32_t idx);


#endif // #if WIND_NET_DHCP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_DHCP_H__

