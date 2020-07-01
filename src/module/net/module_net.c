/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_net.c
** Author      : Jason Zhou
** Last Date   : 2015/06/20
** Description : net stack module entry
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020/06/20
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
#include "wind_net.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




#if (WIND_MODULE_NET_SUPPORT)
FSM_MODEL_DELCARE(net);
















MODULE_INIT(net)
{
    w_err_t err;
    err = _wind_net_mod_init();
    return err;
}

MODULE_EXIT(net)
{
    w_err_t err;
    err = _wind_net_mod_init();
    return err;
}

MODULE_DEF(net, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
