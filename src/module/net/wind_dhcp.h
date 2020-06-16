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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_DHCP_H__

