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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
w_err_t wind_ip_to_str(w_uint32_t ipaddr,char * ipstr)
{
    wind_sprintf(ipstr,"%d.%d.%d.%d",(ipaddr>>24) & 0xff,
        (ipaddr>>16) & 0xff,(ipaddr>>8) & 0xff,ipaddr & 0xff);
    return W_ERR_OK;
}

w_err_t wind_ip_from_str(w_uint32_t *ipaddr,char * ipstr)
{
    w_int32_t cnt;
    w_int32_t data[4];
    cnt = wind_sscanf(ipstr,"%d.%d.%d.%d",&data[0],&data[1],&data[2],&data[3]);
    WIND_ASSERT_RETURN(cnt == 4,W_ERR_FAIL);
    *ipaddr = 0;
    for(cnt = 0;cnt < 4;cnt ++)
    {
        WIND_ASSERT_RETURN(data[cnt] < 256,W_ERR_FAIL);
        *ipaddr = ((*ipaddr) << 8) + (data[cnt] & 0xff);
    }
    return W_ERR_OK;
}
static w_err_t ip_init(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
}
static w_err_t ip_deinit(w_netnode_s *netnode)
{
    return W_ERR_FAIL;
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
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

