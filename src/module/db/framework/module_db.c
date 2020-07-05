/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : module_db.c
** Author      : Jason Zhou
** Last Date   : 2017.12.19
** Description : DB module entry
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2017.12.19
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2017.12.19
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_debug.h"
#include "db_if.h"
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if (WIND_MODULE_DB_SUPPORT)


#if CMD_DB_SUPPORT
COMMAND_DECLARE(db);
COMMAND_DECLARE(tbmodel);
#endif




MODULE_INIT(db)
{
    w_err_t err;
#if CMD_DB_SUPPORT
    err = wind_cmd_register(COMMAND(db));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_cmd_register(COMMAND(tbmodel));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
#endif
    err = _wind_db_mod_init();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

MODULE_EXIT(db)
{
    w_err_t err = W_ERR_OK;
#if CMD_DB_SUPPORT
    err = wind_cmd_unregister(COMMAND(db));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_cmd_unregister(COMMAND(tbmodel));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
#endif
    return err;
}

MODULE_DEF(db, 0x0100,"");

#endif //#if WIND_MODULE_DB_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
