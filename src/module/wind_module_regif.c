/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? wind_module_regif.c
**�?  �?  �? 周江�?
**最后修改日�? 
**�?       �? 模块注册接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建�? 周江�?
** �? �? v1.0
** 日　�? 2013.08.10
** 描　�? 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改�? 
** 日　�? 
** 描　�? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_module.h"
#include "wind_debug.h"
#if WIND_MODULE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
MODULE_DECLARE(shell);
MODULE_DECLARE(tank);
MODULE_DECLARE(tetris);
MODULE_DECLARE(gobang);
MODULE_DECLARE(pushbox);
MODULE_DECLARE(snake);

w_err_t _wind_register_modules(void)
{
	wind_module_register(MODULE(shell));
	wind_module_register(MODULE(tank));
	wind_module_register(MODULE(tetris));
	wind_module_register(MODULE(gobang));
	wind_module_register(MODULE(pushbox));
	wind_module_register(MODULE(snake));
    return W_ERR_OK;
}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif