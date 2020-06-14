/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_netnode.c
** Author      : Jason Zhou
** Last Date   : 2020-06-14
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-14
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-14
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_netnode.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
static w_dlist_s netnode_list;
w_err_t _wind_netnode_mod_init(void)
{
    DLIST_INIT(netnode_list);
    return W_ERR_OK;
}

w_netnode_s* wind_netnode_get(char *name)
{
    return wind_obj_get(name,&netnode_list)
}

w_err_t wind_netnode_register(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    return wind_obj_register(&netnode->obj,&netnode_list);
}

w_err_t wind_netnode_unregister(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    return wind_obj_unregister(&netnode->obj,&netnode_list);
}

w_err_t wind_netnode_enable(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(netnode->obj.magic == WIND_NETNODE_MAGIC,W_ERR_INVALID);
    SET_F_NETNODE_ENABLE(netnode);
    return W_ERR_OK;
}

w_err_t wind_netnode_disable(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(netnode->obj.magic == WIND_NETNODE_MAGIC,W_ERR_INVALID);
    CLR_F_NETNODE_ENABLE(netnode);
    return W_ERR_OK;
}


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

