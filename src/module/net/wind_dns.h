/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dns.h
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
#ifndef WIND_DNS_H__
#define WIND_DNS_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define DNS_QTYPE_A 0x0001
#define DNS_QTYPE_NS 0x0002
#define DNS_QTYPE_CNAME 0x0005
#define DNS_QTYPE_PTR 0x000C
#define DNS_QTYPE_HINFO 0x000D
#define DNS_QTYPE_MX 0x000F
#define DNS_QTYPE_AXFR 0x00FC
#define DNS_QTYPE_ANY 0x00FF

#define DNS_QTYPE_MD 0x0003
#define DNS_QTYPE_MF 0x0004
#define DNS_QTYPE_SOA 0x0006
#define DNS_QTYPE_MB 0x0007
#define DNS_QTYPE_MG 0x0008
#define DNS_QTYPE_MR 0x0009
#define DNS_QTYPE_NULL 0x000A
#define DNS_QTYPE_WKS 0x000B
#define DNS_QTYPE_MINFO 0x000E
#define DNS_QTYPE_TXT 0x0010

#define DNS_QCLASS_IN 0x0001
#define DNS_QCLASS_CS 0x0002
#define DNS_QCLASS_CH 0x0003
#define DNS_QCLASS_HS 0x0004
#define DNS_QCLASS_ANY 0x00FF





typedef struct
{
    w_uint16_t trans_id;
    w_uint16_t flags;
    w_uint16_t ques_num;
    w_uint16_t ans_rrs;
    w_uint16_t auth_rrs;
    w_uint16_t addi_rrs;
}w_dnshead_s;

typedef struct
{
    char *qname;
    w_uint16_t qtype;
    w_uint16_t qclass;
    w_uint16_t ans_rrs;
    w_uint16_t auth_rrs;
    w_uint16_t addi_rrs;
}w_dns_ques_s;

typedef struct
{
    char *dname;
    w_uint16_t qtype;
    w_uint16_t qclass;
    w_uint32_t ttl;
    w_uint16_t datalen;
    w_uint8_t *data;
}w_dns_ans_s;




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_DNS_H__

