/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_route.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : route table configure command
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
#include "wind_netdev.h"
#include "wind_route.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_ROUTE_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(route)
{
    wind_printf("to show or config route table\r\n");
}

COMMAND_USAGE(route)
{
    wind_printf("route [show]:--to show route table info.\r\n");
    wind_printf("route add <destnet> via <ip> dev <debname>:--to add a route table rule.\r\n");
    wind_printf("route del <destnet> via <ip> dev <debname>:--to delete a route table rule.\r\n");
}

COMMAND_MAIN(route,argc,argv)
{
    w_err_t err;
    w_bool_t res;
    //w_uint32_t value;
    char *devname = (char*)W_NULL;
    //w_netdev_s *netdev;
    //w_uint32_t ip,mask,gw;
    //w_vlan_s vlan;
    //w_uint8_t mac[MAC_ADDR_LEN];
    if(argc <= 2)
    {
        if(argc == 2)
            WIND_ASSERT_RETURN(wind_strcmp(argv[1],"show") == 0,W_ERR_INVALID);
        return wind_route_tb_print();
    }
    else if(wind_strcmp(argv[1],"add") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 5,W_ERR_FAIL);
    }
    else if(wind_strcmp(argv[1],"del") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 5,W_ERR_FAIL);
    }

    return W_ERR_FAIL;
}

COMMAND_DEF(route);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
