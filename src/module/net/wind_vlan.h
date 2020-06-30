/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_vlan.h
** Author      : Jason Zhou
** Last Date   : 2020-06-14
** Descrvlantion : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-14
** Descrvlantion : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-14
** Descrvlantion : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_VLAN_H__
#define WIND_VLAN_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_VLAN_SUPPORT

#define VLAN_MODE_NONE  0
#define VLAN_MODE_PRIO  1
#define VLAN_MODE_TAG   2

typedef struct 
{
    w_uint8_t vlan_mode;
    w_uint16_t vlanid:12;
    w_uint16_t vlanpri:3;
    w_uint16_t vlantci:1;
}w_vlan_s;


#endif // #if WIND_NET_VLAN_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_VLAN_H__

