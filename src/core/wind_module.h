/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_module.h
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: wind-osģ����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_MODULE_H__
#define WIND_MODULE_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_module.h"
#include "wind_dlist.h"
#include "wind_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_MODULE_SUPPORT

#define WIND_MODULE_MAGIC 0x6A47326A

#define F_MODULE_OPEN (0x01 << 0) //���module�����Ƿ��Ѿ���
#define IS_F_MODULE_OPEN(module) ((module->obj.flag & F_MODULE_OPEN) == F_MODULE_OPEN)
#define SET_F_MODULE_OPEN(module) (module->obj.flag |= F_MODULE_OPEN)
#define CLR_F_MODULE_OPEN(module) (module->obj.flag &= (~F_MODULE_OPEN))





typedef struct __w_module_s w_module_s;
typedef struct __w_module_ops_s w_module_ops_s;
struct __w_module_s
{
    w_obj_s obj;
    w_int32_t version;
    w_err_t (*init)(void);
    w_err_t (*exit)(void);
};

#define MODULE_INIT(module) w_err_t module##_init(void)
#define MODULE_EXIT(module) w_err_t module##_exit(void)


#define MODULE_DEF(module,version) \
    w_module_s mod_##module = {{(~WIND_MODULE_MAGIC),#module,{W_NULL,W_NULL},0,0},devtype,devid,W_NULL,ops}
#define MODULE_DECLARE(module) extern w_module_s mod_##module;
#define MODULE(module) &mod_##module


w_err_t _wind_module_mod_init(void);
w_err_t _wind_register_modules(void);

w_err_t wind_module_register(w_module_s *module);
w_err_t wind_module_unregister(w_module_s *module);

w_module_s *wind_module_get(const char *name);

w_err_t wind_module_print(void);

#endif

#ifdef __cplusplus
}
#endif

#endif //WIND_DEV_H__
