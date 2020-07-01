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


w_err_t wind_mac_to_str(w_uint8_t *macaddr,char*macstr,char sepch)
{
    WIND_ASSERT_RETURN(macaddr != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(macstr != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN((sepch == ':') || (sepch == '-') || (sepch == '\0'),W_ERR_INVALID);
    if(sepch != '\0')
        wind_sprintf(macstr,"%02x%c%02x%c%02x%c%02x%c%02x%c%02x\0",
            macaddr[0],sepch,macaddr[1],sepch,macaddr[2],sepch,
            macaddr[3],sepch,macaddr[4],sepch,macaddr[5]);
    else
        wind_sprintf(macstr,"%02x%02x%02x%02x%02x%02x\0",
            macaddr[0],macaddr[1],macaddr[2],
            macaddr[3],macaddr[4],macaddr[5]);
    return W_ERR_OK;
}

static w_uint8_t hex_to_dec(char hexch)
{
    if(hexch >= '0' && hexch <= '9')
        return (w_uint8_t)(hexch - '0');
    if(hexch >= 'A' && hexch <= 'F')
        return (w_uint8_t)(hexch - 'A');
    if(hexch >= 'a' && hexch <= 'f')
        return (w_uint8_t)(hexch - 'a');
    return 0;
}

w_err_t wind_mac_from_str(w_uint8_t *macaddr,char *macstr)
{
    w_int32_t i,j;
    WIND_ASSERT_RETURN(macaddr != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(macstr != W_NULL,W_ERR_NULL_PTR);
    if((macstr[2] != '-') && (macstr[2] != ':'))
    {
        for(i = 0,j = 0;i < 6;i ++)
        {
            macaddr[i] = hex_to_dec(macstr[j++]);
            macaddr[i] = (macaddr[i] << 4) + hex_to_dec(macstr[j++]);
        }
    }
    else
    {
        for(i = 0,j = 0;i < 6;i ++)
        {
            macaddr[i] = hex_to_dec(macstr[j++]);
            macaddr[i] = (macaddr[i] << 4) + hex_to_dec(macstr[j++]);
            j ++;
        }
    }
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

w_bool_t wind_mac_is_zero(w_uint8_t *macaddr)
{
    w_int32_t i;
    for(i = 0;i < MAC_ADDR_LEN;i ++)
        if(macaddr[i] != 0x00)
            return W_FALSE;
    return W_TRUE;
}

w_bool_t wind_mac_is_mult(w_uint8_t *macaddr)
{
    if(macaddr[0] & 0x01)
        return W_TRUE;
    return W_FALSE;
}

w_bool_t wind_mac_is_equal(w_uint8_t *srcmac,w_uint8_t *dstmac)
{
    return (wind_memcmp(srcmac,dstmac,MAC_ADDR_LEN) == 0)?W_TRUE:W_FALSE;
}

#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

