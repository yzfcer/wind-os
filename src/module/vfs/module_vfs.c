/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_vfs.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : VFS structure definition
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"
#include "wind_fs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




#if WIND_MODULE_VFS_SUPPORT

















MODULE_INIT(vfs)
{
    return _wind_vfs_mod_init();
}

MODULE_EXIT(vfs)
{
    return W_ERR_OK;
}

MODULE_DEF(vfs, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
