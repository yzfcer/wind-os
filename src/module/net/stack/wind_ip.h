/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_ip.h
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
#ifndef WIND_IP_H__
#define WIND_IP_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_IP_SUPPORT

w_err_t wind_ip_to_str(w_uint32_t ipaddr,char * ipstr);
w_err_t wind_ip_from_str(w_uint32_t *ipaddr,char * ipstr);
w_int32_t wind_ip_to_maskbits(w_uint32_t ipaddr,w_uint32_t maskbits);
w_err_t wind_ip_from_maskbits(w_uint32_t *ipaddr,w_uint32_t maskbits);

w_bool_t wind_ip_is_broad(w_uint32_t ipaddr,w_uint32_t mask);
w_bool_t wind_ip_is_mult(w_uint32_t ipaddr);
w_bool_t wind_ip_is_zero(w_uint32_t ipaddr,w_uint32_t mask);

#endif // #if WIND_NET_IP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_IP_H__

