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
#if WIND_BLK_DRVFRAME_SUPPORT

w_err_t wind_blkdev_register(blkdev_s *dev,w_int32_t count)
{
    w_int32_t i;
    blkdev_s *devi;    
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(dev[i].magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
        wind_notice("register dev:%s",dev[i].name);
        wind_disable_switch();
        foreach_node(dnode,&g_core.devlist)
        {
            devi = DLIST_OBJ(dnode,blkdev_s,blkdevnode);
            if(wind_strcmp(dev[i].name,devi->name) == 0)
            {
                wind_error("device has been registered.\r\n");
                wind_enable_interrupt();
                return ERR_FAIL;
            }
        }
        dev[i].mutex = wind_mutex_create(dev[i].name);
        dlist_insert_tail(&g_core.blkdevlist,&dev[i].blkdevnode);
        wind_enable_switch();    
    }
    
    wind_enable_switch();
    return ERR_OK;
}

w_err_t wind_blkdev_unregister(blkdev_s *dev)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    wind_notice("unregister dev:%s",dev->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.blkdevlist,dnode);
    if(dnode == NULL)
    {
        wind_error("blkdevice has NOT been registered.\r\n");
        wind_enable_interrupt();
        return ERR_FAIL;
    }
    wind_mutex_destroy(dev->mutex);
    dev->mutex = NULL;
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
    blkdev_s *dev = NULL;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.blkdevlist)
    {
        dev = DLIST_OBJ(dnode,blkdev_s,blkdevnode);
        if(wind_strcmp(dev->name,name) == 0)
        {
            wind_enable_switch();
            return dev;
        }
    }
    wind_enable_switch();
    return NULL;
}

w_err_t wind_blkdev_open(blkdev_s *dev)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    if(dev->opened)
        return ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != NULL)
    {
        err = dev->ops->open(dev);
        dev->opened = (err == ERR_OK)?B_TRUE:B_FALSE;
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}


w_int32_t wind_blkdev_read(blkdev_s *dev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkcnt > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr < dev->blkcnt,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr + blkcnt <= dev->blkcnt,ERR_INVALID_PARAM);
    
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    
    wind_mutex_lock(dev->mutex);
    if(dev->ops->read != NULL)
        cnt = dev->ops->read(dev,blkaddr,buf,blkcnt);
    wind_mutex_unlock(dev->mutex);
    return cnt;
}

w_int32_t wind_blkdev_write(blkdev_s *dev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkcnt > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr < dev->blkcnt,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr + blkcnt <= dev->blkcnt,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->write != NULL)
        cnt = dev->ops->write(dev,blkaddr,buf,blkcnt);
    wind_mutex_unlock(dev->mutex);
    return cnt;
}

w_err_t wind_blkdev_erase(blkdev_s *dev,w_addr_t blkaddr,w_int32_t blkcnt)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkcnt > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->erase!= NULL)
        err = dev->ops->erase(dev,blkaddr,blkcnt);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_blkdev_eraseall(blkdev_s *dev)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->eraseall!= NULL)
        err = dev->ops->eraseall(dev);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_blkdev_close(blkdev_s *dev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    if(!dev->opened)
        return ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->close != NULL)
    {
        err = dev->ops->close(dev);
        dev->opened = (err != ERR_OK)?B_TRUE:B_FALSE;
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}


w_err_t wind_blkdev_print(dlist_s *list)
{
    dnode_s *dnode;
    blkdev_s *dev;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nblock device list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-12s %-10s\r\n","dev","blockcnt","blocksize");
    wind_printf("----------------------------------------------\r\n");

    foreach_node(dnode,list)
    {
        dev = (blkdev_s *)DLIST_OBJ(dnode,blkdev_s,blkdevnode);
        wind_printf("%-16s %-12d %-10d\r\n",
            dev->name,dev->blkcnt,dev->blksize);
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}

#endif


