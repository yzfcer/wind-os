/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_tcp.h
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
#ifndef WIND_TCP_H__
#define WIND_TCP_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_TCP_SUPPORT

typedef struct
{
    w_uint16_t src_port;
    w_uint16_t dst_port;
    w_uint32_t seq_num;
    w_uint32_t ack_num;
    w_uint16_t offset;
    w_uint8_t flags;
    w_uint16_t window;
    w_uint16_t chksum;
    w_uint16_t emerge_ptr;
    w_uint32_t group_addr;
}w_tcphead_s;




#endif // #if WIND_NET_TCP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_TCP_H__

