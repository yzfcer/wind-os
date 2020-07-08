/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_route.c
** Author      : Jason Zhou
** Last Date   : 2020-06-27
** Description : route table configure command
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-27
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
#include "wind_route_tb.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_ROUTE_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/
static w_err_t gen_route_tb(w_int32_t argc,char **argv,w_route_tb_s *rtb)
{
    char *sep;
    char ipstr[16];
    w_bool_t res;
    w_err_t err;
    w_int32_t ipstrlen,idx;
    w_uint32_t maskbits;
    w_netdev_s *netdev;
    //w_uint8_t is_default;
    if(wind_strcmp(argv[2],"default") == 0)
    {
        rtb->is_default = 1;
        idx = 3;
    }
    else
    {
        rtb->is_default = 0;
        idx = 2;
    }
    
    WIND_ASSERT_RETURN(wind_strcmp(argv[idx+1],"via") == 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN(wind_strcmp(argv[idx+3],"dev") == 0,W_ERR_FAIL);
    
    sep = wind_strchr(argv[idx],'/');
    WIND_ASSERT_RETURN(sep != W_NULL,W_ERR_FAIL);
    ipstrlen = (w_int32_t)(w_addr_t)(sep - argv[idx] - 1);
    WIND_ASSERT_RETURN(ipstrlen < 16,W_ERR_FAIL);
    wind_memset(ipstr,0,sizeof(ipstr));
    wind_memcpy(ipstr,argv[idx],ipstrlen);
    err = wind_ip_from_str(&rtb->destip,ipstr);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);

    res = wind_str_to_uint(sep + 1,&maskbits);
    WIND_ASSERT_RETURN(res == W_TRUE,W_ERR_FAIL);
    WIND_ASSERT_RETURN(maskbits <= 32,W_ERR_FAIL);
    rtb->mask_bits = (w_uint8_t)maskbits;

    err = wind_ip_from_str(&rtb->next_hop,argv[idx+2]);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);

    netdev = wind_netdev_get(argv[idx+4]);
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);
    rtb->dev_name = (char*)wind_obj_name(&netdev->netnode.obj);
    rtb->enable = 1;
    return W_ERR_OK;
}


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(route)
{
    wind_printf("to show or config route table\r\n");
}

COMMAND_USAGE(route)
{
    wind_printf("route [show]:--to show route table info.\r\n");
    wind_printf("route add [default] <destnet/maskbits> via <ip> dev <devname>:--to add a route table rule.\r\n");
    wind_printf("route del <destnet/maskbits> via <ip> dev <devname>:--to delete a route table rule.\r\n");
}

COMMAND_MAIN(route,argc,argv)
{
    w_err_t err;
    //w_bool_t res;
    char *devname = (char*)W_NULL;
    w_route_tb_s route_tb;
    if(argc <= 2)
    {
        if(argc == 2)
            WIND_ASSERT_RETURN(wind_strcmp(argv[1],"show") == 0,W_ERR_INVALID);
        return wind_route_tb_print();
    }
    
    WIND_ASSERT_RETURN(argc >= 7,W_ERR_FAIL);
    err = gen_route_tb(argc,argv,&route_tb);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    if(wind_strcmp(argv[1],"add") == 0)
    {
        err = wind_route_tb_insert(&route_tb);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else if(wind_strcmp(argv[1],"del") == 0)
    {
        err = wind_route_tb_remove(&route_tb);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else
        err = W_ERR_FAIL;

    return err;
}

COMMAND_DEF(route);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
