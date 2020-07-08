/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_ifconfig.c
** Author      : Jason Zhou
** Last Date   : 2020-06-27
** Description : net device configure command
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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if CMD_IFCONFIG_SUPPORT

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(ifconfig)
{
    wind_printf("to show or config net devices\r\n");
}

COMMAND_USAGE(ifconfig)
{
    wind_printf("ifconfig [devname]:--to show net device info.\r\n");
    wind_printf("ifconfig setip <devname> <ip> <mask> <gateway>:--to set net device ip address.\r\n");
    wind_printf("ifconfig setmac <devname> <macaddr> :--to set net device mac address.\r\n");
    wind_printf("ifconfig setvlan <devname> <vlanid> <prio> :--to set net device vlan info.\r\n");
}

COMMAND_MAIN(ifconfig,argc,argv)
{
    w_err_t err;
    w_bool_t res;
    w_uint32_t value;
    char *devname = (char*)W_NULL;
    w_netdev_s *netdev;
    w_uint32_t ip,mask,gw;
    w_vlan_s vlan;
    w_uint8_t mac[MAC_ADDR_LEN];
    if(argc <= 2)
    {
        if(argc == 2)
            devname = argv[1];
        return wind_netdev_print_detail(W_NULL);
    }
    if(wind_strcmp(argv[1],"setip") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 6,W_ERR_FAIL);
        
        netdev = wind_netdev_get(argv[2]);
        WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);
        
        err = wind_ip_from_str(&ip,argv[3]);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        
        err = wind_ip_from_str(&mask,argv[4]);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        
        err = wind_ip_from_str(&gw,argv[5]);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        
        return wind_netdev_set_ip(netdev,ip,mask,gw);
    }
    if(wind_strcmp(argv[1],"setmac") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 4,W_ERR_FAIL);
        
        netdev = wind_netdev_get(argv[2]);
        WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);
        
        err = wind_mac_from_str(mac,argv[3]);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        
        return wind_netdev_set_mac(netdev,mac);
    }
    if(wind_strcmp(argv[1],"setvlan") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 5,W_ERR_FAIL);
        
        netdev = wind_netdev_get(argv[2]);
        WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);
        
        wind_memset(&vlan,0,sizeof(w_vlan_s));
        res = wind_str_to_uint(argv[3],&value);
        WIND_ASSERT_RETURN(res == W_TRUE,W_ERR_FAIL);
        WIND_ASSERT_RETURN(value <= 4095,W_ERR_FAIL);
        WIND_ASSERT_RETURN(value >= 0,W_ERR_FAIL);
		vlan.vlanid = (w_uint16_t)value;
        
        res = wind_str_to_uint(argv[4],&value);
        WIND_ASSERT_RETURN(res == W_TRUE,W_ERR_FAIL);
        WIND_ASSERT_RETURN(value <= 7,W_ERR_FAIL);
        WIND_ASSERT_RETURN(value >= 0,W_ERR_FAIL);
        vlan.vlanpri = (w_uint16_t)value;
        
        return wind_netdev_set_vlan(netdev,&vlan);
    }

    return W_ERR_FAIL;
}

COMMAND_DEF(ifconfig);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
