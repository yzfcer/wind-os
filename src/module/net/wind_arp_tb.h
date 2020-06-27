/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_arp_tb.h
** Author      : Jason Zhou
** Last Date   : 2020-06-17
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-17
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-17
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_ARP_TB_H__
#define WIND_ARP_TB_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT
#define  WIND_ARP_TB_TTL 150
#define  WIND_ARP_TB_MAX_TTL 7200
typedef struct 
{
    w_uint32_t ipaddr;
    w_uint8_t hwtype;
    w_uint8_t flags;
    w_uint8_t mac[6];
    w_uint16_t enable:1;
    w_uint16_t is_static:1;
    w_uint16_t ttl;
    char *dev_name;
}w_arp_tb_s;

w_err_t wind_arp_tb_init(void);
w_err_t wind_arp_tb_deinit(void);
w_err_t wind_arp_tb_insert(w_arp_tb_s *arp_tb);
w_err_t wind_arp_tb_remove(w_arp_tb_s *arp_tb);
w_err_t wind_arp_tb_clear(void);
w_err_t wind_arp_tb_flush(void);
w_err_t wind_arp_tb_set_ttl(w_uint32_t ttl);
w_int32_t wind_arp_tb_get_ttl(void);
w_err_t wind_arp_tb_flush(void);
w_arp_tb_s *wind_arp_tb_get(w_uint32_t ipaddr);

w_err_t wind_arp_tb_print(void);

#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_ARP_TB_H__

