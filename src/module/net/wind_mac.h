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
#include "wind_config.h"
#include "wind_type.h"
#ifndef WIND_MAC_H__
#define WIND_MAC_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define MAC_ADDR_LEN 6
//typedef char w_macaddr_t[MAC_ADDR_LEN];
w_err_t wind_mac_to_str(w_uint8_t *macaddr,char*str,char *sepch);
w_err_t wind_mac_to_arr(w_uint8_t *macaddr,char*str);
w_bool_t wind_mac_is_broad(w_uint8_t *macaddr);
w_bool_t wind_mac_is_mult(w_uint8_t *macaddr);
w_bool_t wind_mac_is_equal(w_uint8_t *srcamc,w_uint8_t *dstamc);



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_MAC_H__

