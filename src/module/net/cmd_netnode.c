/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_netnode.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : netnode table configure command
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
#include "wind_netnode.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_NETNODE_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(netnode)
{
    wind_printf("to show or config netnode table\r\n");
}

COMMAND_USAGE(netnode)
{
    wind_printf("netnode list:--to show netnode object info.\r\n");
    wind_printf("netnode enable <netnode>:--to enable netnode function.\r\n");
    wind_printf("netnode disable <netnode>:--to disable netnode function.\r\n");
}

COMMAND_MAIN(netnode,argc,argv)
{
    w_err_t err;
    w_netnode_s *netnode;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_FAIL);
    if(wind_strcmp(argv[1],"list") == 0)
    {
        err = wind_netnode_print_list();
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_FAIL);
    netnode = wind_netnode_get(argv[2]);
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_FAIL);
    
    if(wind_strcmp(argv[1],"enable") == 0)
    {
        err = wind_obj_set_objflag(&netnode->obj,F_OBJ_ENABLE);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else if(wind_strcmp(argv[1],"disable") == 0)
    {
        err = wind_obj_clr_objflag(&netnode->obj,F_OBJ_ENABLE);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else
        err = W_ERR_FAIL;

    return err;
}

COMMAND_DEF(netnode);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
