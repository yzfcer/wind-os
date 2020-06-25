/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_igmp.h
** Author      : Jason Zhou
** Last Date   : 2020-06-16
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-16
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-16
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_IGMP_H__
#define WIND_IGMP_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_IGMP_SUPPORT

#define IGMP_V1 0x0
#define IGMP_V2 0x1
#define IGMP_V3 0x2

#define IGMP_TYPE_QUERY   0x1
#define IGMP_TYPE_REPORT1 0x2
#define IGMP_TYPE_REPORT2 0x6
#define IGMP_TYPE_LEAVE   0x7

typedef struct
{
    w_uint8_t type;
    w_uint8_t max_resp;
    w_uint16_t chksum;
    w_uint32_t group_addr;
}w_igmphead_s;



#endif // #if WIND_NET_IGMP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_IGMP_H__

