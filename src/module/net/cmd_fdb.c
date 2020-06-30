/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_fdb.c
** Author      : Jason Zhou
** Last Date   : 2020-06-27
** Description : fdb table configure command
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
#include "wind_fdb.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_FDB_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/
static w_err_t gen_fdb(w_int32_t argc,char **argv,w_fdb_s *fdb)
{
    //char *sep;
    //char ipstr[16];
    //w_bool_t res;
    w_err_t err;
    w_int32_t idx;
    //w_uint32_t maskbits;
    w_netdev_s *netdev;

    idx = 1;
    WIND_ASSERT_RETURN(wind_strcmp(argv[idx+1],"dev") == 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN(wind_strcmp(argv[idx+3],"mac") == 0,W_ERR_FAIL);
    wind_memset(fdb,0,sizeof(w_fdb_s));
    netdev = wind_netdev_get(argv[idx+2]);
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);

	err = wind_mac_from_str(fdb->mac,argv[idx+4]);
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);
    fdb->dev_id = netdev->param.devid;
    fdb->vlanid = netdev->param.vlan.vlanid;
    fdb->is_local = IS_F_NETDEV_LOCAL(netdev)?1:0;
    return W_ERR_OK;
}


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(fdb)
{
    wind_printf("to show or config fdb table\r\n");
}

COMMAND_USAGE(fdb)
{
    wind_printf("fdb [show]:--to show fdb table info.\r\n");
    wind_printf("fdb add dev <debname> mac <macaddr>:--to add a fdb table rule.\r\n");
    wind_printf("fdb del dev <debname> mac <macaddr>:--to delete a fdb table rule.\r\n");
}

COMMAND_MAIN(fdb,argc,argv)
{
    w_err_t err;
    char *devname = (char*)W_NULL;
    w_fdb_s fdb;
    if(argc <= 2)
    {
        if(argc == 2)
            WIND_ASSERT_RETURN(wind_strcmp(argv[1],"show") == 0,W_ERR_INVALID);
        return wind_fdb_print();
    }
    
    WIND_ASSERT_RETURN(argc >= 6,W_ERR_FAIL);
    err = gen_fdb(argc,argv,&fdb);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    if(wind_strcmp(argv[1],"add") == 0)
    {
        err = wind_fdb_insert(&fdb);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else if(wind_strcmp(argv[1],"del") == 0)
    {
        err = wind_fdb_remove(&fdb);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    else
        err = W_ERR_FAIL;

    return err;
}

COMMAND_DEF(fdb);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
