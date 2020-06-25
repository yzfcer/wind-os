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
#include "wind_core.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

static w_netdev_s *netdev_list[WIND_NETDEV_MAX_NUM];
extern void wind_netdevs_register();
WIND_NETDEV_DECLARE(lo);

static w_err_t add_netdev(w_netdev_s *netdev)
{
    w_int32_t i;
    w_err_t err = W_ERR_FAIL;
    wind_disable_switch();
    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
    {
        if(netdev_list[i] != W_NULL)
            continue;
        netdev_list[i] = netdev;
        err = W_ERR_OK;
        break;
    }
    wind_enable_switch();
    return err;
}

static w_err_t del_netdev(w_netdev_s *netdev)
{
    w_int32_t i;
    w_err_t err = W_ERR_FAIL;
    wind_disable_switch();
    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
    {
        if(netdev_list[i] != netdev)
            continue;
        netdev_list[i] = netdev;
        err = W_ERR_OK;
        break;
    }
    wind_enable_switch();
    return err;
}

w_err_t _wind_netdev_mod_init(void)
{
    w_int32_t i;
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
    w_err_t err;
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    err = add_netdev(netdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return wind_netnode_register(&netdev->netnode);
}

w_err_t wind_netdev_unregister(w_netdev_s *netdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    err = del_netdev(netdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
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

w_err_t wind_netdev_set_ip(w_netdev_s *netdev,w_uint32_t ip,w_uint32_t mask,w_uint32_t gw)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    netdev->param.ip = ip;
    netdev->param.mask = mask;
    netdev->param.gw = gw;
    return W_ERR_OK;
}
w_err_t wind_netdev_set_vlan(w_netdev_s *netdev,w_vlan_s *vlan)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    wind_memcpy(&netdev->param.vlan,vlan,sizeof(w_vlan_s));
    SET_F_NETDEV_VLAN_EN(netdev);
    return W_ERR_OK;
}

#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

