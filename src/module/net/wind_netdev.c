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
#include "wind_ip.h"
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
        netdev->param.devid = i;
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
        netdev->param.devid = 255;
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
    w_int32_t i;
    w_netdev_s *netdev = (w_netdev_s *)W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
    {
        if(netdev_list[i] == W_NULL)
            continue;
        if(wind_strcmp(netdev_list[i]->netnode.obj.name,name) == 0)
        {
            netdev = netdev_list[i];
            break;
        }
    }
    wind_enable_switch();
    return netdev;
}


w_netdev_s* wind_netdev_get_byid(w_uint16_t devid)
{
    w_int32_t i;
    w_netdev_s *netdev = (w_netdev_s *)W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
    {
        if(netdev_list[i] == W_NULL)
            continue;
        if(netdev_list[i]->param.devid == devid)
        {
            netdev = netdev_list[i];
            break;
        }
    }
    wind_enable_switch();
    return netdev;
}

w_err_t wind_netdev_register(w_netdev_s *netdev)
{
    w_err_t err;
    w_int32_t len;
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    len = wind_strlen(netdev->netnode.obj.name);
    WIND_ASSERT_RETURN(len <= NETDEV_MAX_NAMELEN,W_ERR_INVALID);
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
    SET_F_NETDEV_IP_READY(netdev);
    return W_ERR_OK;
}

w_err_t wind_netdev_set_vlan(w_netdev_s *netdev,w_vlan_s *vlan)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    wind_memcpy(&netdev->param.vlan,vlan,sizeof(w_vlan_s));
    SET_F_NETDEV_VLAN_EN(netdev);
    return W_ERR_OK;
}

w_err_t wind_netdev_set_mac(w_netdev_s *netdev,w_uint8_t *mac)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(mac != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(!wind_mac_is_broad(mac),W_ERR_INVALID);
    WIND_ASSERT_RETURN(!wind_mac_is_mult(mac),W_ERR_INVALID);
    wind_memcpy(&netdev->param.mac,mac,MAC_ADDR_LEN);
    return W_ERR_OK;
}

w_err_t wind_netdev_set_iptype(w_netdev_s *netdev,ip_type_e iptype)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);
    netdev->param.iptype = iptype;
    return W_ERR_OK;
}


static w_err_t print_netdev_detail(w_netdev_s *netdev)
{
    char ipstr[16],gwstr[16],maskstr[16];
    char macstr[18];
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_NULL_PTR);

    wind_memset(ipstr,0,sizeof(ipstr));
    wind_memset(gwstr,0,sizeof(gwstr));
    wind_memset(macstr,0,sizeof(macstr));
    wind_memset(ipstr,0,sizeof(ipstr));
    wind_ip_to_str(netdev->param.ip,ipstr);
    wind_ip_to_str(netdev->param.gw,gwstr);
    wind_ip_to_str(netdev->param.mask,maskstr);
    wind_mac_to_str(netdev->param.mac,macstr,':');

    wind_printf("%-12s mac %s\r\n",netdev->netnode.obj.name,macstr);
    wind_printf("%-12s %s %s %s %s\r\n","",
        IS_F_NETDEV_ENABLE(netdev)?"ENABLED":"DISABLED",
        IS_F_NETDEV_UNICAST(netdev)?"UNICAST":"",
        IS_F_NETDEV_BROADCAST(netdev)?"BROADCAST":"",
        IS_F_NETDEV_MULTCAST(netdev)?"MULTCAST":"");
    wind_printf("%-12s ip %s gw %s mask %s\r\n","",ipstr,gwstr,maskstr);
    wind_printf("%-12s VLAN_EN %-5s vlanid %d prio %d\r\n","",
        IS_F_NETDEV_VLAN_EN(netdev)?"TRUE":"FALSE",netdev->param.vlan.vlanid,
        netdev->param.vlan.vlanpri);
    wind_printf("\r\n");
    return W_ERR_OK;
}

w_err_t wind_netdev_print_detail(char *devname)
{
    w_int32_t i;
    w_err_t err;
	w_netdev_s *netdev = (w_netdev_s *)W_NULL;
    wind_disable_switch();
    err = W_ERR_OK;
    for(i = 0;i < WIND_NETDEV_MAX_NUM;i ++)
    {
        if(netdev_list[i] == W_NULL)
            continue;
        if((devname == W_NULL) || (wind_strcmp(netdev_list[i]->netnode.obj.name,devname) == 0))
        {
            netdev = netdev_list[i];
            err = print_netdev_detail(netdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"print netdev fail");
        }
    }
    wind_enable_switch();
    return err;
    
}



#endif // #if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

