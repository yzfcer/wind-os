/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_diagnose.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.02.24
**��        ��: ϵͳ������ϴ���ģ��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.02.24
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.02.24
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_DIAGNOSE_H__
#define WIND_DIAGNOSE_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_DIAGNOSE_SUPPORT
#define WIND_DIAGNOSE_MAGIC 0x387A5247
typedef w_int32_t (*diagnose_fn)(void);
typedef enum
{
    DIAG_RES_OK = 0,
    DIAG_RES_ERROR,
    DIAG_RES_DIAG_MAGIC_ERROR,
    DIAG_RES_OBJ_MAGIC_ERROR,
    DIAG_RES_FUNC_NULL,
}w_diag_res_e;

typedef struct _wind_diagnose
{
    w_obj_s obj;
    w_int32_t result;
    diagnose_fn diagnose_func;
}w_diagnose_s;


#define DIAGNOSENOSE_DEF(name,func) \
    static w_diagnose_s g_diagnose_##name = \
    {{(~WIND_DIAGNOSE_MAGIC),#name,{W_NULL,W_NULL},0,0},0,func}
#define DIAGNOSENOSE(name) &g_diagnose_##name

w_err_t _wind_diagnose_mod_init(void);
w_diagnose_s *wind_diagnose_get(const char *name);
w_err_t wind_diagnose_register(w_diagnose_s *diagnose);
w_err_t wind_diagnose_unregister(w_diagnose_s *diagnose);
w_err_t wind_diagnose_check(void);
w_err_t wind_diagnose_print(void);

#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_DIAGNOSE_H__

