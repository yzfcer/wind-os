/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_mac.c
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
#include "wind_mac.h"
#include "wind_debug.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT


w_err_t wind_mac_to_str(w_uint8_t *macaddr,char*str,char *sepch)
{
    return W_ERR_OK;
}
w_err_t wind_mac_from_str(w_uint8_t *macaddr,char*str)
{
    return W_ERR_OK;
}

w_bool_t wind_mac_is_broad(w_uint8_t *macaddr)
{
    w_int32_t i;
    for(i = 0;i < MAC_ADDR_LEN;i ++)
        if(macaddr[i] != 0xff)
            return W_FALSE;
    return W_TRUE;
}


w_bool_t wind_mac_is_mult(w_uint8_t *macaddr)
{
    if(macaddr[0] & 0x01)
        return W_TRUE;
    return W_FALSE;
}

w_bool_t wind_mac_is_equal(w_uint8_t *srcamc,w_uint8_t *dstamc)
{
    return (wind_memcmp(srcamc,dstamc,MAC_ADDR_LEN) == 0)?W_TRUE:W_FALSE;
}

#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

