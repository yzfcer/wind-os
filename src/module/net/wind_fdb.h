/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_fdb.h
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
#ifndef WIND_FDB_H__
#define WIND_FDB_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT
#define  WIND_FDB_TTL 120

typedef struct 
{
    w_uint8_t is_local:1;
    w_uint8_t enable:1;
    w_uint8_t mac[6];
    w_uint8_t dev_id;
    w_uint16_t vlanid;
    w_uint16_t ttl;
}w_fdb_s;

w_err_t wind_fdb_init(void);
w_err_t wind_fdb_deinit(void);
w_err_t wind_fdb_insert(w_fdb_s *fdb);
w_err_t wind_fdb_remove(w_fdb_s *fdb);
w_err_t wind_fdb_clear(void);
w_err_t wind_fdb_flush(void);
w_fdb_s *wind_fdb_get(w_uint8_t *mac);
w_err_t wind_fdb_print(void);

#endif //#if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_FDB_H__

