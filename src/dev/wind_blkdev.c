/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_blkdev.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统的驱动的相关的源文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_blkdev.h"
#include "wind_mutex.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_var.h"

w_err_t wind_register_blkdev(blkdev_s *blkdev,w_int32_t count)
{
    blkdev_s *blkdevi;    
    dnode_s *dnode;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    
    wind_notice("register blkdev:%s",blkdev->name);
    wind_disable_switch();
    foreach_node(dnode,&g_core.blkdevlist)
    {
        blkdevi = DLIST_OBJ(dnode,blkdev_s,blkdevnode);
        if(wind_strcmp(blkdev->name,blkdevi->name) == 0)
        {
            wind_error("blkdevice has been registered.\r\n");
            wind_open_interrupt();
            return ERR_COMMAN;
        }
    }
    blkdev->mutex = wind_mutex_create(blkdev->name);
    dlist_insert_tail(&g_core.blkdevlist,&blkdev->blkdevnode);
    wind_enable_switch();
    return ERR_OK;
}

w_err_t wind_unregister_blkdev(blkdev_s *blkdev)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    wind_notice("unregister blkdev:%s",blkdev->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.blkdevlist,dnode);
    if(dnode == NULL)
    {
        wind_error("blkdevice has NOT been registered.\r\n");
        wind_open_interrupt();
        return ERR_COMMAN;
    }
    wind_mutex_destroy(blkdev->mutex);
    blkdev->mutex = NULL;
    wind_enable_switch();
    return ERR_OK;
}


w_err_t _wind_blkdev_init(void)
{
    _register_blkdevs();
    return ERR_OK;
}

blkdev_s *wind_blkdev_get(char *name)
{
    blkdev_s *blkdev = NULL;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.blkdevlist)
    {
        blkdev = DLIST_OBJ(dnode,blkdev_s,blkdevnode);
        if(wind_strcmp(blkdev->name,name) == 0)
            break;
    }
    wind_enable_switch();
    return blkdev;
}

w_err_t wind_blkdev_open(blkdev_s *blkdev)
{
    w_err_t err = ERR_COMMAN;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    if(blkdev->opened)
        return ERR_OK;
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->open != NULL)
    {
        err = blkdev->ops->open(blkdev);
        blkdev->opened = (err == ERR_OK)?B_TRUE:B_FALSE;
    }
    wind_mutex_unlock(blkdev->mutex);
    return err;
}


w_int32_t wind_blkdev_read(blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->read != NULL)
        cnt = blkdev->ops->read(blkdev,addr,buf,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return cnt;
}

w_int32_t wind_blkdev_write(blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->write != NULL)
        cnt = blkdev->ops->write(blkdev,addr,buf,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return cnt;
}

w_err_t wind_blkdev_erase(blkdev_s *blkdev,w_addr_t addr,w_int32_t blkcnt)
{
    w_err_t err = ERR_COMMAN;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->erase!= NULL)
        err = blkdev->ops->erase(blkdev,addr,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return err;
}

w_err_t wind_blkdev_eraseall(blkdev_s *blkdev)
{
    w_err_t err = ERR_COMMAN;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->eraseall!= NULL)
        err = blkdev->ops->eraseall(blkdev);
    wind_mutex_unlock(blkdev->mutex);
    return err;
}

w_err_t wind_blkdev_close(blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    if(!blkdev->opened)
        return ERR_OK;
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->close != NULL)
    {
        err = blkdev->ops->close(blkdev);
        blkdev->opened = (err != ERR_OK)?B_TRUE:B_FALSE;
    }
    wind_mutex_unlock(blkdev->mutex);
    return err;
}


