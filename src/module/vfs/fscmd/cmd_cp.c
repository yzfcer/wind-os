/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_cp.c
** Author      : Jason Zhou
** Last Date   : 2020-07-05
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-07-05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-07-05
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "cmd_cp.h"
#include "wind_obj.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_CMD_CP_SUPPORT
COMMAND_DISC(cp)
{
}
COMMAND_USAGE(cp)
{
}
COMMAND_MAIN(cp,argc,argv)
{
    return W_ERR_OK;
}
COMMAND_DEF(cp);

#endif // #if WIND_CMD_CP_SUPPORT

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

