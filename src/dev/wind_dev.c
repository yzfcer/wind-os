/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_dev.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 字符设备API接口
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
#include "wind_core.h"
#include "wind_string.h"
#include "wind_var.h"
#if WIND_DRVFRAME_SUPPORT

w_err_t wind_dev_register(dev_s *dev,w_int32_t count)
{
    w_int32_t i;
    dev_s *devi;    
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(dev[i].magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
        wind_notice("register dev:%s",dev[i].name);
        wind_disable_switch();
        foreach_node(dnode,&g_core.devlist)
        {
            devi = DLIST_OBJ(dnode,dev_s,devnode);
            if(wind_strcmp(dev[i].name,devi->name) == 0)
            {
                wind_error("device has been registered.\r\n");
                wind_enable_interrupt();
                return ERR_FAIL;
            }
        }
        dev[i].mutex = wind_mutex_create(dev[i].name);
        dlist_insert_tail(&g_core.devlist,&dev[i].devnode);
        wind_enable_switch();    
    }
    
    wind_enable_switch();
    return ERR_OK;
}

w_err_t wind_dev_unregister(dev_s *dev)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    wind_notice("unregister dev:%s",dev->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.devlist,dnode);
    if(dnode == NULL)
    {
        wind_error("device has NOT been registered.\r\n");
        wind_enable_interrupt();
        return ERR_FAIL;
    }
    wind_mutex_destroy(dev->mutex);
    dev->mutex = NULL;
    wind_enable_switch();
    return ERR_OK;
}


w_err_t _wind_dev_init(void)
{
    _register_devs();
    return ERR_OK;
}

dev_s *wind_dev_get(char *name)
{
    dev_s *dev = NULL;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.devlist)
    {
        dev = DLIST_OBJ(dnode,dev_s,devnode);
        if(wind_strcmp(dev->name,name) == 0)
        {
            wind_enable_switch();
            return dev;
        }
    }
    wind_enable_switch();
    return NULL;
}

w_err_t wind_dev_open(dev_s *dev)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
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

w_err_t wind_dev_ioctl(dev_s *dev,w_int32_t cmd,void *param)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != NULL)
        err = dev->ops->ioctl(dev,cmd,param);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_read(dev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->read != NULL)
        err = dev->ops->read(dev,buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_write(dev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->write != NULL)
        err = dev->ops->write(dev,buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_dev_close(dev_s *dev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
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

w_err_t wind_dev_print(dlist_s *list)
{
    dnode_s *dnode;
    dev_s *dev;
    int cnt = 0;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\ndev list as following:\r\n");
    
    foreach_node(dnode,list)
    {
        dev = (dev_s *)DLIST_OBJ(dnode,dev_s,devnode);
        wind_printf("%-12s ",dev->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return ERR_OK;
}


#endif


