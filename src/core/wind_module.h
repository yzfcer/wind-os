/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_module.h
**Author: Jason Zhou
**Last Date: 
**Description: wind-os模块框架
**              
**--------------History----------------------------------------------------------------------------
Author: 
** Version: v1.0
** Date: 
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
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
#endif // #ifdef __cplusplus


#if WIND_MODULE_SUPPORT

#define WIND_MODULE_MAGIC 0x6A47326A

#define F_MODULE_OPEN (0x01 << 0) //Mark whether the module object is already open
#define IS_F_MODULE_OPEN(module) ((module->obj.flag & F_MODULE_OPEN) == F_MODULE_OPEN)
#define SET_F_MODULE_OPEN(module) (module->obj.flag |= F_MODULE_OPEN)
#define CLR_F_MODULE_OPEN(module) (module->obj.flag &= (~F_MODULE_OPEN))

//Module status enumeration
typedef enum
{
    MOD_IDLE, //Idle state (initialization)
    MOD_OK,   //Normal state
    MOD_ERROR //Error state
}mod_status_e;



typedef struct __w_module_s w_module_s;
typedef struct __w_module_ops_s w_module_ops_s;

//Module object information
struct __w_module_s
{
    w_obj_s obj;          //Basic object information
    mod_status_e status;  //Module status
    w_int32_t version;    //Module version
    char *depend;         //Module dependency information
    w_err_t (*init)(void);//Module initialization function
    w_err_t (*exit)(void);//Module exit function
};

#define MODULE_INIT(module) w_err_t module##_init(void)
#define MODULE_EXIT(module) w_err_t module##_exit(void)


#define MODULE_DEF(module,version,depend) \
    w_module_s mod_##module = {WIND_OBJ(WIND_MODULE_MAGIC,0,#module),MOD_IDLE,version,depend,module##_init,module##_exit}
#define MODULE_DECLARE(module) extern w_module_s mod_##module;
#define MODULE(module) &mod_##module


w_err_t _wind_module_mod_init(void);
w_err_t _wind_register_modules(void);
w_module_s *wind_module_get(const char *name);

w_err_t wind_module_register(w_module_s *module);
w_err_t wind_module_unregister(w_module_s *module);


w_err_t wind_module_print_list(void);
w_err_t wind_module_print_detail(void);

#endif // #if WIND_MODULE_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_DEV_H__
