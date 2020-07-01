/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_rcp.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : File copy command between host file system and wind OS file system
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
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if (CMD_RCP_SUPPORT)


COMMAND_DISC(rcp)
{
    wind_printf("[*PC*] to copy file(s) between host fs and wind-os fs [NOT SUPPORTED NOW].\r\n");
}

COMMAND_USAGE(rcp)
{
    wind_printf("rcp in <src_file> <dest_file>:--to copy file from host fs to wind-os fs.\r\n");
    wind_printf("rcp out <src_file> <dest_file>:--to copy file from wind-os fs to host fs.\r\n");
}

COMMAND_MAIN(rcp,argc,argv)
{
    wind_error("command is NOT supported right now\r\n");
    return W_ERR_OK;
}

COMMAND_DEF(rcp);


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
