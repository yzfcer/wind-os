/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dbgpoint.c
** Author      : Jason Zhou
** Last Date   : 2019.01.28
** Description : 新增加一种调试方法，该方法可以方便的通过命令来实现变量查看与改写
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.01.28
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.01.28
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_dbgpoint.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_mutex.h"
#include "wind_board_port.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_DBGPOINT_SUPPORT
#define NODE_TO_DBGPOINT(node) (w_dbgpoint_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_dbgpoint_s*)0)->obj.objnode)))
static w_dlist_s dbgpointlist;


w_err_t _wind_dbgpoint_mod_init(void)
{
    DLIST_INIT(dbgpointlist);
    return W_ERR_OK;
}

w_dbgpoint_s *wind_dbgpoint_get(const char *name)
{
    return (w_dbgpoint_s*)wind_obj_get(name,&dbgpointlist);
}

w_err_t wind_dbgpoint_register(w_dbgpoint_s *dbgpoint)
{
    w_err_t err;
    w_dbgpoint_s *dbgp;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dbgpoint->obj.magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        wind_notice("register dbgpoint:%s",wind_obj_name(&dbgpoint->obj));
        dbgp = wind_dbgpoint_get(dbgpoint->obj.name);
        WIND_CHECK_BREAK(dbgp == W_NULL,W_ERR_OK);

        err = wind_mutex_init(&dbgpoint->mutex,dbgpoint->obj.name);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init mutex failed");
        err = wind_obj_init(&dbgpoint->obj,WIND_DBGPOINT_MAGIC,wind_obj_name(&dbgpoint->obj),&dbgpointlist);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init obj failed");
    }while(0);

    return err;
}

w_err_t wind_dbgpoint_unregister(w_dbgpoint_s *dbgpoint)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dbgpoint->obj.magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        wind_notice("unregister dbgpoint:%s",wind_obj_name(&dbgpoint->obj));
        err = wind_obj_deinit(&dbgpoint->obj,WIND_DBGPOINT_MAGIC,&dbgpointlist);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"deinit obj failed");
        err = wind_mutex_destroy(&dbgpoint->mutex);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"deinit mutex failed");
    }while(0);
    return err;
}

w_int32_t wind_dbgpoint_read(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t lenth = -1;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len >= dbgpoint->lenth,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dbgpoint->obj.magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&dbgpoint->mutex);
    if(dbgpoint->read)
        lenth = dbgpoint->read(buff,len);
    wind_mutex_unlock(&dbgpoint->mutex);
    return lenth;
}

w_int32_t wind_dbgpoint_write(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t lenth = -1;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len <= dbgpoint->lenth,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dbgpoint->obj.magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&dbgpoint->mutex);
    if(dbgpoint->write)
        lenth = dbgpoint->write(buff,len);
    wind_mutex_unlock(&dbgpoint->mutex);
    return lenth;
}

w_err_t wind_dbgpoint_print(void)
{
    w_dnode_s *dnode;
    w_dbgpoint_s *dbgpoint;
    int cnt = 0;
    w_dlist_s *list = &dbgpointlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\ndebug point list:\r\n");
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        dbgpoint = NODE_TO_DBGPOINT(dnode);
        wind_printf("%-12s ",dbgpoint->obj.magic);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    wind_enable_switch();
    return W_ERR_OK;
}

#endif // #if WIND_DBGPOINT_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

