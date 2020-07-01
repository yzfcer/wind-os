/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_icmp.h
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
#ifndef WIND_ICMP_H__
#define WIND_ICMP_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_ICMP_SUPPORT

#define ICMP_TYPE_ECHO_REPLAY  0x00
#define ICMP_TYPE_UNREACHABLE  0x03
#define ICMP_TYPE_INHIBIT_SRC  0x04
#define ICMP_TYPE_REDIRECT     0x05
#define ICMP_TYPE_ECHO_REQUEST 0x08
#define ICMP_TYPE_ROUTER_BROAD  0x09
#define ICMP_TYPE_ROUTER_REQ   0x0A
#define ICMP_TYPE_TIMEOUT      0x0B
#define ICMP_TYPE_SUBNET_REQ  0x11
#define ICMP_TYPE_SUBNET_ACK  0x12

typedef struct
{
    w_uint8_t type;
    w_uint8_t code;
    w_uint16_t chksum;
    w_uint16_t sessionid;
    w_uint16_t seq;
}w_icmphead_s;




#endif // #if WIND_NET_ICMP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_ICMP_H__

