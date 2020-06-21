/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_net_stati.h
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
#ifndef WIND_NET_STATI_H__
#define WIND_NET_STATI_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef struct
{
    w_uint64_t send_bytes_cnt;
    w_uint64_t recv_bytes_cnt;
    w_uint64_t drop_send_bytes_cnt;
    w_uint64_t drop_recv_bytes_cnt;
    
    w_uint32_t send_pack_cnt;
    w_uint32_t recv_pack_cnt;
    w_uint32_t drop_send_pack_cnt;
    w_uint32_t drop_recv_pack_cnt;
    w_uint32_t error_sendpack_cnt;
    w_uint32_t error_recvpack_cnt;
    
    
}w_net_stati_s;

#define NET_STATI_NULL {0,0,0,0,0,0,0,0}

#define SEND_PACK_CNT_INC(stati) stati.send_pack_cnt ++
#define RECV_PACK_CNT_INC(stati) stati.recv_pack_cnt ++
#define DROP_SEND_PACK_CNT_INC(stati) stati.drop_send_pack_cnt ++
#define DROP_RECV_PACK_CNT_INC(stati) stati.drop_recv_pack_cnt ++
#define ERROR_SEND_PACK_CNT_INC(stati) stati.error_sendpack_cnt ++
#define ERROR_RECV_PACK_CNT_INC(stati) stati.error_recvpack_cnt ++

#define SEND_BYTES_CNT_INC(stati,size) stati.send_bytes_cnt += size
#define RECV_BYTES_CNT_INC(stati,size) stati.recv_bytes_cnt += size
#define DROP_BYTES_CNT_INC(stati,size) stati.drop_bytes_cnt += size


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_NET_STATI_H__

