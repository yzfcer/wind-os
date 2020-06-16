/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_icmp.h
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
#include "wind_debug.h"
#ifndef WIND_ICMP_H__
#define WIND_ICMP_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef struct
{
    w_uint8_t type;
    w_uint8_t code;
    w_uint16_t chksum;
    w_uint16_t sessionid;
    w_uint16_t seq;
}w_icmphead_s;




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_ICMP_H__

