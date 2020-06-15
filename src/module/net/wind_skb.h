/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_skb.h
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
#include "wind_config.h"
#include "wind_type.h"
#ifndef WIND_SKB_H__
#define WIND_SKB_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#define PROTO_802_1P 0x8100
#define PROTO_IP     0x0800

typedef struct
{
	w_uint8_t lay2_idx;
	w_uint8_t lay3_idx;
	w_uint8_t lay4_idx;
	w_uint8_t lay7_idx;
	w_uint16_t packlen;
	w_uint8_t *packbuf;
	w_netdev_s *indev;
	w_netdev_s *outdev;
}w_skb_s;



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_SKB_H__

