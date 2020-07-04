/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_mv.c
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
#include "cmd_mv.h"
#include "wind_obj.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_CMD_MV_SUPPORT
COMMAND_DISC(mv)
{
}
COMMAND_USAGE(mv)
{
}
COMMAND_MAIN(mv,argc,argv)
{
    return W_ERR_OK;
}
COMMAND_DEF(mv);

#endif // #if WIND_CMD_MV_SUPPORT

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

