/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_xmodem.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : XMODEM module entry
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020/05/11 00:24:37
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
#include "wind_xmodem.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




#if (WIND_MODULE_XMODEM_SUPPORT)
COMMAND_DECLARE(xmodem);

MODULE_INIT(xmodem)
{
    w_err_t err;
    err = wind_cmd_register(COMMAND(xmodem));
    return err;//_wind_xmodem_mod_init();
}

MODULE_EXIT(xmodem)
{
    w_err_t err;
    err = wind_cmd_unregister(COMMAND(xmodem));
    return err;//_wind_xmodem_mod_init();
}

MODULE_DEF(xmodem, 0x0100,"");

#endif // #if (WIND_MODULE_XMODEM_SUPPORT)
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
