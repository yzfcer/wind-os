/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_route_tb.h
** Author      : Jason Zhou
** Last Date   : 2020-06-18
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-18
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-18
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_ROUTE_TB_H__
#define WIND_ROUTE_TB_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef struct 
{
    w_uint8_t port_id;
    w_uint8_t mask_bits;
    w_uint8_t is_default:1;
    w_uint8_t enable    :1;
    w_uint32_t destip;
    w_uint32_t next_hop;
}w_route_tb_s;

w_err_t wind_route_tb_insert(w_route_tb_s *route_tb);
w_err_t wind_route_tb_tb_clear(void);
w_err_t wind_route_tb_tb_init(void);
w_err_t wind_route_tb_tb_flush(void);
w_route_tb_s *wind_route_tb_get(w_uint32_t destip);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_ROUTE_TB_H__

