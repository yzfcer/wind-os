/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_dbgpoint.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.01.28
**描        述: 新增加一种调试方法，该方法可以方便的通过命令来实现变量查看与改写
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.01.28
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.01.28
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_DBGPOINT_H__
#define WIND_DBGPOINT_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_DBGPOINT_SUPPORT
#define WIND_DBGPOINT_MAGIC 0x357892A9
typedef w_int32_t (*dbgpoint_read_fn)(w_uint8_t *buff,w_int32_t lenth);
typedef w_int32_t (*dbgpoint_write_fn)(w_uint8_t *buff,w_int32_t lenth);

typedef struct _wind_dbgpoint
{
    w_obj_s obj;
    w_int32_t lenth;
    void *mutex;
    dbgpoint_read_fn read;
    dbgpoint_write_fn write;
}w_dbgpoint_s;

#define DBGP_DEF(name,lenth,read,write) \
    static g_dbgp_##name = \
    {{(~WIND_DBGPOINT_SUPPORT),#name,{W_NULL,W_NULL},0,0},lenth,read,write};
#define DBGP(name) &g_dbgp_##name


w_err_t _wind_dbgpoint_mod_init(void);
w_dbgpoint_s *wind_dbgpoint_get(const char *name);
w_err_t wind_dbgpoint_register(w_dbgpoint_s *dbgpoint);
w_err_t wind_dbgpoint_unregister(w_dbgpoint_s *dbgpoint);
w_int32_t wind_dbgpoint_read(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len);
w_int32_t wind_dbgpoint_write(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len);
w_err_t wind_dbgpoint_print(void);

#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_DBGPOINT_H__

