/****************************************Copyright (c)**************************************************
**                                       ?? ?? ?? ??
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**??  ??  ?? wind_module_regif.c
**??  ??  ?? 周江??
**最后修改日?? 
**??       ?? 模块注册接口
**              
**--------------History----------------------------------------------------------------------------
** 创建?? 周江??
** ?? ?? v1.0
** 日　?? 2013.08.10
** 描　?? First version
**
**--------------Cureent version----------------------------------------------------------------------------
** 修改?? 
** 日　?? 
** 描　?? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_module.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_SUPPORT


#if WIND_MODULE_SHELL_SUPPORT
MODULE_DECLARE(shell);
#endif
#if WIND_MODULE_DB_SUPPORT
MODULE_DECLARE(db);
#endif
#if WIND_MODULE_VFS_SUPPORT
MODULE_DECLARE(vfs);
#endif
#if WIND_MODULE_CUTEST_SUPPORT
MODULE_DECLARE(cutest);
#endif
#if WIND_MODULE_LUA_SUPPORT
MODULE_DECLARE(lua);
#endif
#if WIND_MODULE_CJSON_SUPPORT
MODULE_DECLARE(cjson);
#endif
#if WIND_MODULE_DICT_SUPPORT
MODULE_DECLARE(dict);
#endif

#if WIND_MODULE_TOOLS_SUPPORT
MODULE_DECLARE(tools);
#endif
#if WIND_MODULE_XML_SUPPORT
MODULE_DECLARE(xml);
#endif
#if WIND_MODULE_LOG_SUPPORT
MODULE_DECLARE(log);
#endif

w_err_t _wind_register_modules(void)
{
#if WIND_MODULE_SHELL_SUPPORT
    wind_module_register(MODULE(shell));
#endif
#if WIND_MODULE_VFS_SUPPORT
    wind_module_register(MODULE(vfs));
#endif
#if WIND_MODULE_DB_SUPPORT
    wind_module_register(MODULE(db));
#endif
#if WIND_MODULE_CUTEST_SUPPORT
    wind_module_register(MODULE(cutest));
#endif
#if WIND_MODULE_LUA_SUPPORT
    wind_module_register(MODULE(lua));
#endif
#if WIND_MODULE_CJSON_SUPPORT
    wind_module_register(MODULE(cjson));
#endif
#if WIND_MODULE_DICT_SUPPORT
        wind_module_register(MODULE(dict));
#endif
#if WIND_MODULE_TOOLS_SUPPORT
	wind_module_register(MODULE(tools));
#endif

#if WIND_MODULE_XML_SUPPORT
    wind_module_register(MODULE(xml));
#endif
#if WIND_MODULE_LOG_SUPPORT
    wind_module_register(MODULE(log));
#endif
    return W_ERR_OK;
}

#endif // #if WIND_MODULE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
