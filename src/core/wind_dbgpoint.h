/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dbgpoint.h
** Author      : Jason Zhou
** Last Date   : 2019.01.28
** Description : View and rewrite variables through debuging command
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.01.28
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.01.28
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_DBGPOINT_H__
#define WIND_DBGPOINT_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_DBGPOINT_SUPPORT
#define WIND_DBGPOINT_MAGIC 0x357892A9
typedef w_int32_t (*dbgpoint_read_fn)(w_uint8_t *buff,w_int32_t lenth);
typedef w_int32_t (*dbgpoint_write_fn)(w_uint8_t *buff,w_int32_t lenth);

typedef struct _wind_dbgpoint
{
    w_obj_s obj;             //Basic object information
    w_int32_t lenth;         //Debug point cache length
    w_mutex_s mutex;             //Debug point mutex
    dbgpoint_read_fn read;   //Function for reading information
    dbgpoint_write_fn write; //Function for writing information
}w_dbgpoint_s;

#define DBGP_DEF(name,lenth,read,write) \
    static g_dbgp_##name = \
    {WIND_OBJ(WIND_DBGPOINT_SUPPORT,0,#name),lenth,read,write};
#define DBGP(name) &g_dbgp_##name


w_err_t _wind_dbgpoint_mod_init(void);
w_dbgpoint_s *wind_dbgpoint_get(const char *name);
w_err_t wind_dbgpoint_register(w_dbgpoint_s *dbgpoint);
w_err_t wind_dbgpoint_unregister(w_dbgpoint_s *dbgpoint);
w_int32_t wind_dbgpoint_read(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len);
w_int32_t wind_dbgpoint_write(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len);
w_err_t wind_dbgpoint_print(void);

#endif // #if WIND_DBGPOINT_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif// #ifndef WIND_DBGPOINT_H__

