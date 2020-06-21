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


static w_err_t wind_netnode_input_hook(w_netnode_s * netnode,w_skb_s *skb)
{
    return W_ERR_OK;
}

static w_err_t wind_netnode_output_hook(w_netnode_s * netnode,w_skb_s *skb)
{
    return W_ERR_OK;
}
w_err_t _wind_netnode_mod_init(void)
{
    DLIST_INIT(netnode_list);
    return W_ERR_OK;
}

w_netnode_s* wind_netnode_get(char *name)
{
    return (w_netnode_s*)wind_obj_get(name,&netnode_list);
}

w_err_t wind_netnode_register(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    wind_notice("net:register netnode %s",netnode->obj.name);
    wind_netnode_disable(netnode);
    return wind_obj_register(&netnode->obj,&netnode_list);
}

w_err_t wind_netnode_unregister(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    wind_notice("net:unregister netnode %s",netnode->obj.name);
    return wind_obj_unregister(&netnode->obj,&netnode_list);
}

w_err_t wind_netnode_enable(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(netnode->obj.magic == WIND_NETNODE_MAGIC,W_ERR_INVALID);
    SET_F_OBJ_ENABLE(netnode->obj);
    return W_ERR_OK;
}

w_err_t wind_netnode_disable(w_netnode_s *netnode)
{
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(netnode->obj.magic == WIND_NETNODE_MAGIC,W_ERR_INVALID);
    CLR_F_OBJ_ENABLE(netnode->obj);
    return W_ERR_OK;
}

w_err_t wind_netnode_recv(w_netnode_s * netnode,w_skb_s *skb)
{
    w_err_t err;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(skb != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(IS_F_OBJ_ENABLE(netnode->obj),W_ERR_NOT_SUPPORT);
    do
    {
        err = W_ERR_OK;
        RECV_PACK_CNT_INC(netnode->stati);
        RECV_BYTES_CNT_INC(netnode->stati,skb->packlen);
        err = wind_netnode_input_hook(netnode,skb);
        WIND_CHECK_BREAK(err == W_ERR_OK,W_ERR_DROP_PACK);
        err = netnode->input(netnode,skb);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
    }while(0);
    if(err != W_ERR_OK)
    {
        ERROR_RECV_PACK_CNT_INC(netnode->stati);
        DROP_RECV_PACK_CNT_INC(netnode->stati);
        wind_skb_free(skb);
    }
    return err;
}

w_err_t wind_netnode_send(w_netnode_s * netnode,w_skb_s *skb)
{
    w_err_t err;
    WIND_ASSERT_RETURN(netnode != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(skb != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(IS_F_OBJ_ENABLE(netnode->obj),W_ERR_NOT_SUPPORT);
    do
    {
        err = W_ERR_OK;
        SEND_PACK_CNT_INC(netnode->stati);
        SEND_BYTES_CNT_INC(netnode->stati,skb->packlen);
        err = wind_netnode_output_hook(netnode,skb);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
        err = netnode->output(netnode,skb);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
    }while(0);
    if(err != W_ERR_OK)
    {
        DROP_SEND_PACK_CNT_INC(netnode->stati);
        wind_skb_free(skb);
    }
    return err;
}


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

