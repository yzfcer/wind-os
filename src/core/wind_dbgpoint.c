/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_dbgpoint.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.01.28
**描        述: 新增加一种调试方法，该方法可以方便的通过命令来实现变量查看与改写
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.01.28
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.01.28
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_dbgpoint.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_mutex.h"
#include "wind_board_port.h"



#if WIND_DBGPOINT_SUPPORT
static w_dlist_s dbgpointlist;


w_err_t _wind_dbgpoint_mod_init(void)
{
    DLIST_INIT(dbgpointlist);
    return W_ERR_OK;
}

w_dbgpoint_s *wind_dbgpoint_get(const char *name)
{
    w_dbgpoint_s *dbgpoint;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&dbgpointlist)
    {
        dbgpoint = DLIST_OBJ(dnode,w_dbgpoint_s,dbgpointnode);
        if(dbgpoint->name && (wind_strcmp(name,dbgpoint->name) == 0))
        {
            wind_enable_switch();
            return dbgpoint;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_dbgpoint_register(w_dbgpoint_s *dbgpoint)
{
    w_dbgpoint_s *dbgp;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dbgpoint->magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    wind_notice("register dbgpoint:%s",dbgpoint->name != W_NULL?dbgpoint->name:"null");
    dbgp = wind_dbgpoint_get(dbgpoint->name);
    if(dbgp != W_NULL)
    {
        wind_notice("dbgpoint has been registered.\r\n");
        return W_ERR_OK;
    }

    dbgpoint->mutex = wind_mutex_create(dbgpoint->name);
    wind_disable_switch();
    dlist_insert_tail(&dbgpointlist,&dbgpoint->dbgpointnode);
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_dbgpoint_unregister(w_dbgpoint_s *dbgpoint)
{
    w_dnode_s *dnode;
    w_dbgpoint_s *dbgp;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dbgpoint->magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    wind_notice("unregister dbgpoint:%s",dbgpoint->name != W_NULL?dbgpoint->name:"null");
    wind_disable_switch();
    dnode = dlist_remove(&dbgpointlist,&dbgpoint->dbgpointnode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    dbgp = DLIST_OBJ(dnode,w_dbgpoint_s,dbgpointnode);
    if(dbgp->mutex)
        wind_mutex_destroy(dbgp->mutex);
    return W_ERR_OK;  
}

w_int32_t wind_dbgpoint_read(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t lenth = -1;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len >= dbgpoint->lenth,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dbgpoint->magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dbgpoint->mutex != W_NULL,W_ERR_FAIL);
    wind_mutex_lock(dbgpoint->mutex);
    if(dbgpoint->read)
        lenth = dbgpoint->read(buff,len);
    wind_mutex_unlock(dbgpoint->mutex);
    return lenth;
}

w_int32_t wind_dbgpoint_write(w_dbgpoint_s *dbgpoint,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t lenth = -1;
    WIND_ASSERT_RETURN(dbgpoint != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len >= dbgpoint->lenth,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dbgpoint->magic == WIND_DBGPOINT_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dbgpoint->mutex != W_NULL,W_ERR_FAIL);
    wind_mutex_lock(dbgpoint->mutex);
    if(dbgpoint->write)
        lenth = dbgpoint->write(buff,len);
    wind_mutex_unlock(dbgpoint->mutex);
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
    foreach_node(dnode,list)
    {
        dbgpoint = (w_dbgpoint_s *)DLIST_OBJ(dnode,w_dbgpoint_s,dbgpointnode);
        wind_printf("%-12s ",dbgpoint->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}

#endif

