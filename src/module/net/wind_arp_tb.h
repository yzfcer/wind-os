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
#define  WIND_ARP_TB_TTL 150

typedef struct 
{
    w_uint32_t ipaddr;
    w_uint8_t hwtype;
    w_uint8_t flags;
    w_uint8_t mac[6];
    w_uint16_t enable:1;
    w_uint16_t ttl;
}w_arp_tb_s;

w_err_t wind_arp_tb_init(void);
w_err_t wind_arp_tb_deinit(void);
w_err_t wind_arp_tb_update(w_arp_tb_s *arp_tb);
w_err_t wind_arp_tb_clear(void);
w_err_t wind_arp_tb_flush(void);
w_arp_tb_s *wind_arp_tb_get(w_uint32_t ipaddr);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_ARP_TB_H__

