/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_arp.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : arp table configure command
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
#include "wind_arp_tb.h"
#include "wind_ip.h"
#include "wind_mac.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_ARP_SUPPORT)

/********************************************internal variables**********************************************/

/********************************************internal functions**********************************************/
static w_err_t gen_arp_tb(w_int32_t argc,char **argv,w_arp_tb_s *arptb)
{
    w_err_t err;
    wind_memset(arptb,0,sizeof(w_arp_tb_s));
    err = wind_ip_from_str(&arptb->ipaddr,argv[2]);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_mac_from_str(arptb->mac,argv[3]);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    arptb->hwtype = 1;
    arptb->flags = 0;
    arptb->enable = 1;
    arptb->is_static = 1;
    return W_ERR_OK;
}


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(arp)
{
    wind_printf("to show or config arp table\r\n");
}

COMMAND_USAGE(arp)
{
    wind_printf("arp [show]:--to show arp table info.\r\n");
    wind_printf("arp add <ip> <mac>:--to add a arp table rule.\r\n");
    wind_printf("arp del <ip> <mac>:--to delete a arp table rule.\r\n");
}

COMMAND_MAIN(arp,argc,argv)
{
    w_err_t err;
    char *devname = (char*)W_NULL;
    w_arp_tb_s arp_tb;
    if(argc <= 2)
    {
        if(argc == 2)
            WIND_ASSERT_RETURN(wind_strcmp(argv[1],"show") == 0,W_ERR_INVALID);
        return wind_arp_tb_print();
    }
    
    WIND_ASSERT_RETURN(argc >= 4,W_ERR_FAIL);
    err = gen_arp_tb(argc,argv,&arp_tb);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    if(wind_strcmp(argv[1],"add") == 0)
    {
        err = wind_arp_tb_insert(&arp_tb);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else if(wind_strcmp(argv[1],"del") == 0)
    {
        err = wind_arp_tb_remove(&arp_tb);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else
        err = W_ERR_FAIL;

    return err;
}

COMMAND_DEF(arp);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
