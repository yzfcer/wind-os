/****************************************Copyright (c)**************************************************
**                                       ?? ?? ?? ??
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**??  ??  ?? wind_module_regif.c
**??  ??  ?? �ܽ�??
**����޸���?? 
**??       ?? ģ��ע��ӿ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ����?? �ܽ�??
** ?? ?? v1.0
** �ա�?? 2013.08.10
** �衡?? ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸�?? 
** �ա�?? 
** �衡?? 
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
#if WIND_MODULE_SHELL_SUPPORT
MODULE_DECLARE(shell);
#endif
#if WIND_MODULE_DB_SUPPORT
MODULE_DECLARE(db);
#endif
#if WIND_MODULE_CUTEST_SUPPORT
MODULE_DECLARE(cutest);
#endif
#if WIND_MODULE_GAME_SUPPORT
MODULE_DECLARE(tank);
MODULE_DECLARE(tetris);
MODULE_DECLARE(gobang);
MODULE_DECLARE(pushbox);
MODULE_DECLARE(snake);
#endif
#if WIND_MODULE_PACK_SUPPORT
MODULE_DECLARE(MODULE(pack));
#endif
#if WIND_MODULE_RCP_SUPPORT
MODULE_DECLARE(MODULE(rcp));
#endif
#if WIND_MODULE_MKFS_SUPPORT
MODULE_DECLARE(MODULE(mkfs));
#endif
#if WIND_MODULE_PACK_SUPPORT
MODULE_DECLARE(MODULE(pack));
#endif

w_err_t _wind_register_modules(void)
{
#if WIND_MODULE_SHELL_SUPPORT
    wind_module_register(MODULE(shell));
#endif
#if WIND_MODULE_DB_SUPPORT
wind_module_register(MODULE(db));
#endif
#if WIND_MODULE_CUTEST_SUPPORT
wind_module_register(MODULE(cutest));
#endif
#if WIND_MODULE_GAME_SUPPORT
	wind_module_register(MODULE(tank));
	wind_module_register(MODULE(tetris));
	wind_module_register(MODULE(gobang));
	wind_module_register(MODULE(pushbox));
	wind_module_register(MODULE(snake));
#endif
#if WIND_MODULE_PACK_SUPPORT
	wind_module_register(MODULE(pack));
#endif
#if WIND_MODULE_RCP_SUPPORT
	wind_module_register(MODULE(rcp));
#endif
#if WIND_MODULE_MKFS_SUPPORT
	wind_module_register(MODULE(mkfs));
#endif
#if WIND_MODULE_PACK_SUPPORT
	wind_module_register(MODULE(pack));
#endif	
    return W_ERR_OK;
}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif
