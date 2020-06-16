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
static w_dlist_s netdev_list;
w_err_t _wind_netdev_mod_init(void)
{
    DLIST_INIT(netdev_list);
    return W_ERR_OK;
}

w_netdev_s* wind_netdev_get(char *name)
{
    return (w_netdev_s*)wind_obj_get(name,&netdev_list);
}

w_err_t wind_netdev_register(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_PTR_NULL);
    return wind_obj_register(&netdev->obj,&netdev_list);
}

w_err_t wind_netdev_unregister(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_PTR_NULL);
    return wind_obj_unregister(&netdev->obj,&netdev_list);
}

w_err_t wind_netdev_enable(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(netdev->obj.magic == WIND_NETDEV_MAGIC,W_ERR_INVALID);
    SET_F_OBJ_ENABLE(netdev->obj);
    return W_ERR_OK;
}

w_err_t wind_netdev_disable(w_netdev_s *netdev)
{
    WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(netdev->obj.magic == WIND_NETDEV_MAGIC,W_ERR_INVALID);
    CLR_F_OBJ_ENABLE(netdev->obj);
    return W_ERR_OK;
}




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

