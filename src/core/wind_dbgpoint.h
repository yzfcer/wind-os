/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_dbgpoint.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.01.28
**��        ��: ������һ�ֵ��Է������÷������Է����ͨ��������ʵ�ֱ����鿴���д
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.01.28
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.01.28
** �衡��: 
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

