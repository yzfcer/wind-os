/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_mac.h
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
#ifndef WIND_MAC_H__
#define WIND_MAC_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

#define MAC_ADDR_LEN 6
w_err_t wind_mac_to_str(w_uint8_t *macaddr,char*macstr,char sepch);
w_err_t wind_mac_from_str(w_uint8_t *macaddr,char*macstr);
w_bool_t wind_mac_is_broad(w_uint8_t *macaddr);
w_bool_t wind_mac_is_zero(w_uint8_t *macaddr);
w_bool_t wind_mac_is_mult(w_uint8_t *macaddr);
w_bool_t wind_mac_is_equal(w_uint8_t *srcmac,w_uint8_t *dstmac);



#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_MAC_H__
