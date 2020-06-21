/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_netdev.c
** Author      : Jason Zhou
** Last Date   : 2020-06-15
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-15
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-15
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_netdev.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

static w_netdev_s *netdev_list[WIND_NETDEV_MAX_NUM];
extern void wind_netdevs_register();
WIND_NETDEV_DECLARE(lo);
w_err_t _wind_netdev_mod_init(void)
{
    w_int32_t i;
    //DLIST_INIT(netdev_list);
    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
        netdev_list[i] = (w_netdev_s *)W_NULL;
    wind_netdev_register(WIND_NETDEV(lo));
    wind_netdevs_register();
    return W_ERR_OK;
}

w_netdev_s* wind_netdev_get(char *name)
{
    return (w_netdev_s*)wind_netnode_get(name);
}

w_err_t wind_netdev_register(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    return wind_netnode_register(&netdev->netnode);
}

w_err_t wind_netdev_unregister(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    return wind_netnode_unregister(&netdev->netnode);
}

w_err_t wind_netdev_enable(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(netdev->netnode.obj.magic == WIND_NETDEV_MAGIC,W_ERR_INVALID);
    SET_F_OBJ_ENABLE(netdev->netnode.obj);
    return W_ERR_OK;
}

w_err_t wind_netdev_disable(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(netdev->netnode.obj.magic == WIND_NETDEV_MAGIC,W_ERR_INVALID);
    CLR_F_OBJ_ENABLE(netdev->netnode.obj);
    return W_ERR_OK;
}

w_err_t wind_netdev_print_list(void)
{
    w_obj_s *obj;
    int i,cnt = 0;
    wind_printf("\r\nnetdev list:\r\n");

    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
    {
        if(netdev_list[i] == W_NULL)
            continue;
        obj = &netdev_list[i]->netnode.obj;
        wind_printf("%-12s ",obj->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

