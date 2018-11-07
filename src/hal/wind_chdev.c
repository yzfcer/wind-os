/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_dev.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 字符设备标准化API接口
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
#include "wind_chdev.h"
#include "wind_mutex.h"
#include "wind_core.h"
#include "wind_string.h"
#if WIND_DRVFRAME_SUPPORT
static w_dlist_s devlist;

w_err_t wind_chdev_register(w_chdev_s *dev,w_int32_t count)
{
    w_int32_t i;
    w_chdev_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(dev[i].magic == WIND_DEV_MAGIC,W_ERR_INVALID);
        wind_notice("register dev:%s",dev[i].name);
        devi = wind_chdev_get(dev[i].name);
        if(devi != W_NULL)
        {
            wind_error("device has been registered.\r\n");
            continue;
        }
        if(dev[i].ops->init)
        {
            err = dev[i].ops->init(&dev[i]);
            if(err != W_ERR_OK)
            {
                wind_error("blkdev:%s init failed:%d.",dev[i].name,err);
                continue;
            }
        }
        dev[i].mutex = wind_mutex_create(dev[i].name);
        wind_disable_switch();
        dlist_insert_tail(&devlist,&dev[i].devnode);
        wind_enable_switch();
    }
    
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_chdev_unregister(w_chdev_s *dev)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    wind_notice("unregister dev:%s",dev->name);
    wind_disable_switch();
    dnode = dlist_remove(&devlist,&dev->devnode);
    wind_enable_switch();
    if(dnode == W_NULL)
    {
        wind_error("device has NOT been registered.\r\n");
        return W_ERR_FAIL;
    }
    if(dev->ops->deinit)
        dev->ops->deinit(dev);
    wind_mutex_destroy(dev->mutex);
    dev->mutex = W_NULL;
    return W_ERR_OK;
}


w_err_t _wind_chdev_mod_init(void)
{
    DLIST_INIT(devlist);
    _register_chdevs();
    return W_ERR_OK;
}

w_chdev_s *wind_chdev_get(char *name)
{
    w_chdev_s *dev = W_NULL;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&devlist)
    {
        dev = DLIST_OBJ(dnode,w_chdev_s,devnode);
        if(wind_strcmp(dev->name,name) == 0)
        {
            wind_enable_switch();
            return dev;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_chdev_open(w_chdev_s *dev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    if(dev->opened)
        return W_ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != W_NULL)
    {
        err = dev->ops->open(dev);
        dev->opened = (err == W_ERR_OK)?W_TRUE:W_FALSE;
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_chdev_ioctl(w_chdev_s *dev,w_int32_t cmd,void *param)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != W_NULL)
        err = dev->ops->ioctl(dev,cmd,param);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_chdev_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->read != W_NULL)
        err = dev->ops->read(dev,buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_chdev_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->write != W_NULL)
        err = dev->ops->write(dev,buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_chdev_close(w_chdev_s *dev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    if(!dev->opened)
        return W_ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->close != W_NULL)
    {
        err = dev->ops->close(dev);
        dev->opened = (err != W_ERR_OK)?W_TRUE:W_FALSE;
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_chdev_print(void)
{
    w_dnode_s *dnode;
    w_chdev_s *dev;
    int cnt = 0;
    w_dlist_s *list = &devlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\ndev list as following:\r\n");
    
    foreach_node(dnode,list)
    {
        dev = (w_chdev_s *)DLIST_OBJ(dnode,w_chdev_s,devnode);
        wind_printf("%-12s ",dev->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#endif


