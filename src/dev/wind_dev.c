/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_driver.c
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
#include "wind_dev.h"
#include "wind_mutex.h"
#include "wind_os_hwif.h"
#include "wind_string.h"
dlist_s dev_list;
w_err_t wind_register_dev(dev_s *dev,w_int32_t count)
{
    dev_s *devi;    
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    
    wind_close_interrupt();
    foreach_node(dnode,&dev_list)
    {
        dev = DLIST_OBJ(dnode,dev_s,devnode);
        if(wind_strcmp(dev->name,devi->name) == 0)
        {
            wind_error("device has been registered.\r\n");
            wind_open_interrupt();
            return ERR_COMMAN;
        }
    }
    dev->mutex = wind_mutex_create(dev->name);
    dlist_insert_tail(&dev_list,&dev->devnode);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_unregister_dev(dev_s *dev)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    wind_close_interrupt();
    dnode = dlist_remove(&dev_list,dnode);
    if(dnode == NULL)
    {
        wind_error("device has NOT been registered.\r\n");
        wind_open_interrupt();
        return ERR_COMMAN;
    }
    wind_mutex_free(dev->mutex);
    dev->mutex = NULL;
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_devices_init(void)
{
    DLIST_INIT(dev_list);
    _register_devs();
    return ERR_OK;
}

dev_s *wind_dev_get(char *name)
{
    dev_s *dev;
    dnode_s *dnode;
    foreach_node(dnode,&dev_list)
    {
        dev = DLIST_OBJ(dnode,dev_s,devnode);
        if(wind_strcmp(dev->name,name) == 0)
            return dev;
    }
    return NULL;
}

w_err_t wind_dev_open(dev_s *dev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops->open != NULL,ERR_INVALID_PARAM);
    wind_mutex_lock(dev->mutex);
    err = dev->ops->open();
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_dev_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops->ioctl != NULL,ERR_INVALID_PARAM);
    wind_mutex_lock(dev->mutex);
    err = dev->ops->ioctl(ctrlpoint,param);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_read(dev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops->read != NULL,ERR_INVALID_PARAM);
    wind_mutex_lock(dev->mutex);
    err = dev->ops->read(buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_write(dev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops->write != NULL,ERR_INVALID_PARAM);
    wind_mutex_lock(dev->mutex);
    err = dev->ops->write(buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_dev_close(dev_s *dev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops->close != NULL,ERR_INVALID_PARAM);
    wind_mutex_lock(dev->mutex);
    err = dev->ops->close();
    wind_mutex_unlock(dev->mutex);
    return err;
}


