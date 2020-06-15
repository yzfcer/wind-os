/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_skb.c
** Author      : Jason Zhou
** Last Date   : 2020-06-15
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-15
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-15
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_skb.h"
#include "wind_mac.h"
#include "wind_string.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

w_uint16_t wind_skb_get_uint16(w_skb_s *skb,w_uint16_t idx)
{
    w_uint16_t value;
    value = skb->packbuf[idx];
    value = (value << 8) + skb->packbuf[idx+1];
    return value;
}

w_uint32_t wind_skb_get_uint32(w_skb_s *skb,w_uint16_t idx)
{
    w_int32_t i,roll;
    w_uint16_t value = 0;
    for(i = 0,roll = 0;i < 4;i ++,roll += 8)
    {
        value = (value << roll) + skb->packbuf[idx+1];
    }
    return value;
}

void wind_skb_get_ether_srcmac(w_skb_s *skb,w_uint8_t *macaddr)
{
    wind_memcpy(&skb->packbuf[skb->lay2_idx+6],macaddr,MAC_ADDR_LEN);
}

void wind_skb_get_ether_dstmac(w_skb_s *skb,w_uint8_t *macaddr)
{
    wind_memcpy(&skb->packbuf[skb->lay2_idx],macaddr,MAC_ADDR_LEN);
}


w_uint16_t wind_skb_get_lay3_proto(w_skb_s *skb)
{
    w_uint16_t proto;
    proto = wind_skb_get_uint16(skb,skb->lay2_idx+12);
    if(proto == 0x8100)
        proto = wind_skb_get_uint16(skb,16);
    return proto;
}

w_bool_t wind_skb_has_vlan(w_skb_s *skb)
{
    w_uint16_t proto;
    proto = wind_skb_get_uint16(skb,skb->lay2_idx+12);
    return proto == 0x8100?W_TRUE:W_FALSE;
}

w_err_t wind_skb_get_vlan(w_skb_s *skb,w_vlan_s *vlan)
{
    w_uint16_t value;
    value = wind_skb_get_uint16(skb,14);
    vlan->vlanpri = ((value >> 13) & 0x07);
    vlan->vlantci = ((value >> 12) & 0x01);
    vlan->vlanid = ((value >> 0) & 0x3f);
    return W_ERR_OK;
}

w_uint8_t wind_skb_get_ip_version(w_skb_s *skb)
{
    return ((skb->packbuf[skb->lay3_idx] >> 4) & 0x0f) * 4;
}

w_uint8_t wind_skb_get_ip_headlen(w_skb_s *skb)
{
    return (skb->packbuf[skb->lay3_idx] & 0x0f);
}

w_uint8_t wind_skb_get_ip_tos(w_skb_s *skb)
{
    return skb->packbuf[skb->lay3_idx+1];
}

w_uint16_t wind_skb_get_ip_len(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay3_idx+2);
}

w_uint16_t wind_skb_get_ip_identification(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay3_idx+4);
}

w_uint8_t wind_skb_get_ip_flags(w_skb_s *skb)
{
    return ((skb->packbuf[skb->lay3_idx+6] >> 13) & 0x07);
}

w_uint16_t wind_skb_get_ip_offset(w_skb_s *skb)
{
    w_uint16_t value;
    value = wind_skb_get_uint16(skb,skb->lay3_idx+6);
    return (value & 0x1fff);
}

w_uint8_t wind_skb_get_ip_ttl(w_skb_s *skb)
{
    return skb->packbuf[skb->lay3_idx+8];
}

w_uint8_t wind_skb_get_lay4_proto(w_skb_s *skb)
{
    return skb->packbuf[skb->lay3_idx+9];
}

w_uint16_t wind_skb_get_ip_sum(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay3_idx+10);
}

w_uint32_t wind_skb_get_ip_src(w_skb_s *skb)
{
    return wind_skb_get_uint32(skb,skb->lay3_idx+12);
}

w_uint32_t wind_skb_get_ip_dest(w_skb_s *skb)
{
    return wind_skb_get_uint32(skb,skb->lay3_idx+16);
}

void wind_skb_set_lay3_idx(w_skb_s *skb)
{
    w_uint16_t proto = wind_skb_get_uint16(skb,skb->lay2_idx+12);
    if(proto == 0x8100)
        skb->lay3_idx = skb->lay2_idx + 18;
    else
        skb->lay3_idx = skb->lay2_idx + 14;
}

void wind_skb_set_lay4_idx(w_skb_s *skb)
{
    w_uint8_t headlen = wind_skb_get_ip_headlen(skb);
    skb->lay4_idx = skb->lay3_idx + headlen;
}


w_uint16_t wind_skb_get_udp_src_port(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx);
}

w_uint16_t wind_skb_get_udp_dest_port(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx+2);
}

w_uint16_t wind_skb_get_udp_datalen(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx+4);
}

w_uint16_t wind_skb_get_udp_sum(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx+6);
}


w_uint16_t wind_skb_get_tcp_src_port(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx);
}

w_uint16_t wind_skb_get_tcp_dest_port(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx+2);
}

w_uint32_t wind_skb_get_tcp_seqnum(w_skb_s *skb)
{
    return wind_skb_get_uint32(skb,skb->lay4_idx+4);
}

w_uint32_t wind_skb_get_tcp_acknum(w_skb_s *skb)
{
    return wind_skb_get_uint16(skb,skb->lay4_idx+8);
}

w_uint8_t wind_skb_get_tcp_headlen(w_skb_s *skb)
{
    return (skb->packbuf[skb->lay4_idx+12] >> 4);
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

