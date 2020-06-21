/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_arp.h
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
#ifndef WIND_ARP_H__
#define WIND_ARP_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_dlist.h"
#include "wind_netdev.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define ARP_TABLE_NUM 32
#define ARP_REQ 1
#define ARP_ACK 2
#define RARP_REQ 3
#define RARP_ACK 4

typedef struct 
{
    w_uint16_t hw_type;
    w_uint16_t pro_type;
    w_uint8_t mac_len;
    w_uint8_t ip_len;
    w_uint16_t op_type;
    w_uint8_t srcmac[6];
    w_uint8_t dstmac[6];
    w_uint32_t srcip;
    w_uint32_t dstip;
}w_arp_s;



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_ARP_H__

