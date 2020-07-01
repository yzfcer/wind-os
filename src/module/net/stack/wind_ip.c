/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_ip.c
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
#include "wind_ip.h"
#include "wind_skb.h"
#include "wind_netnode.h"
#include "wind_debug.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_IP_SUPPORT

w_err_t wind_ip_to_str(w_uint32_t ipaddr,char * ipstr)
{
    wind_sprintf(ipstr,"%d.%d.%d.%d",(ipaddr>>24) & 0xff,
        (ipaddr>>16) & 0xff,(ipaddr>>8) & 0xff,ipaddr & 0xff);
    return W_ERR_OK;
}

w_err_t wind_ip_from_str(w_uint32_t *ipaddr,char * ipstr)
{
    w_int32_t i,j,len;
    w_int32_t data[4] = {0,0,0,0};
    len = wind_strlen(ipstr);
    WIND_ASSERT_RETURN(len < 16,W_ERR_INVALID);
    for(i = 0,j = 0;i < len;i ++)
    {
        if((ipstr[i] <= '9') && (ipstr[i] >= '0'))
            data[j] = data[j] * 10 + ipstr[i] - '0';
        else if(ipstr[i] == '.')
            j ++;
        else
            return W_ERR_INVALID;
    }
    *ipaddr = 0;
    for(i = 0;i < 4;i ++)
    {
        WIND_ASSERT_RETURN(data[i] < 256,W_ERR_FAIL);
        *ipaddr = ((*ipaddr) << 8) + (data[i] & 0xff);
    }
    return W_ERR_OK;
}

w_int32_t wind_ip_to_maskbits(w_uint32_t ipaddr,w_uint32_t maskbits)
{
    w_int32_t i,cnt = 0;
    if (ipaddr == 0xffffffff) 
        return 32;
    if (ipaddr == 0xffffff00) 
        return 24;
    if (ipaddr == 0xffff0000) 
        return 16;
    if (ipaddr == 0xff000000) 
        return 8;
    for (i = 0; i < 32; i++)    
    {
        if ((ipaddr << i) & 0x80000000)
            cnt++;
        else
            break;
    }
    return cnt;
}

w_err_t wind_ip_from_maskbits(w_uint32_t *ipaddr,w_uint32_t maskbits)
{
    w_int32_t i,j;
    w_uint32_t ip_num = 0;
    WIND_ASSERT_RETURN(ipaddr != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(maskbits <= 32,W_ERR_INVALID);
 
    *ipaddr = 0;
    if (maskbits == 8)
        *ipaddr = 0xff000000;
    if (maskbits == 16)
        *ipaddr = 0xffff0000;
    if (maskbits == 24)
        *ipaddr = 0xffffff00;
    if (maskbits == 32)
        *ipaddr = 0xffffffff;
    if(*ipaddr != 0)
        return W_ERR_OK;

    for (i = maskbits, j = 31; i > 0; i --, j --)
    {
        ip_num += (1 << j);
    }
    *ipaddr = ip_num;
    return W_ERR_OK;
}


w_bool_t wind_ip_is_broad(w_uint32_t ipaddr,w_uint32_t mask)
{
    if((ipaddr | mask) == 0xffffffff)
        return W_TRUE;
    return W_FALSE;
}

w_bool_t wind_ip_is_mult(w_uint32_t ipaddr)
{
    if((ipaddr & 0xf0000000) == 0xe0000000)
            return W_TRUE;
        return W_FALSE;
}

w_bool_t wind_ip_is_zero(w_uint32_t ipaddr,w_uint32_t mask)
{
    if((ipaddr & (~mask)) == 0)
        return W_TRUE;
    return W_FALSE;
}


static w_err_t ip_init(w_netnode_s *netnode)
{
    return W_ERR_OK;
}
static w_err_t ip_deinit(w_netnode_s *netnode)
{
    return W_ERR_OK;
}

static w_err_t ip_input(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}
static w_err_t ip_output(w_netnode_s *netnode,w_skb_s *skb)
{
    return W_ERR_FAIL;
}

WIND_NETNODE_DEF(ip);
#endif // #if WIND_NET_IP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

