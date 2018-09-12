/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_dev.c
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: �ַ��豸��׼��API�ӿ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
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

w_err_t wind_dev_register(w_chdev_s *dev,w_int32_t count)
{
    w_int32_t i;
    w_chdev_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(dev[i].magic == WIND_DEV_MAGIC,W_ERR_INVALID);
        wind_notice("register dev:%s",dev[i].name);
        devi = wind_dev_get(dev[i].name);
        if(devi != NULL)
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
        dlist_insert_tail(&g_core.devlist,&dev[i].devnode);
        wind_enable_switch();
    }
    
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_dev_unregister(w_chdev_s *dev)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    wind_notice("unregister dev:%s",dev->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.devlist,dnode);
    wind_enable_switch();
    if(dnode == NULL)
    {
        wind_error("device has NOT been registered.\r\n");
        return W_ERR_FAIL;
    }
    wind_mutex_destroy(dev->mutex);
    dev->mutex = NULL;
    return W_ERR_OK;
}


w_err_t _wind_dev_mod_init(void)
{
    _register_devs();
    return W_ERR_OK;
}

w_chdev_s *wind_dev_get(char *name)
{
    w_chdev_s *dev = NULL;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.devlist)
    {
        dev = DLIST_OBJ(dnode,w_chdev_s,devnode);
        if(wind_strcmp(dev->name,name) == 0)
        {
            wind_enable_switch();
            return dev;
        }
    }
    wind_enable_switch();
    return NULL;
}

w_err_t wind_dev_open(w_chdev_s *dev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != NULL,W_ERR_INVALID);
    if(dev->opened)
        return W_ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != NULL)
    {
        err = dev->ops->open(dev);
        dev->opened = (err == W_ERR_OK)?W_TRUE:W_FALSE;
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_dev_ioctl(w_chdev_s *dev,w_int32_t cmd,void *param)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != NULL)
        err = dev->ops->ioctl(dev,cmd,param);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->read != NULL)
        err = dev->ops->read(dev,buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->write != NULL)
        err = dev->ops->write(dev,buf,len);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_dev_close(w_chdev_s *dev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dev != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != NULL,W_ERR_INVALID);
    if(!dev->opened)
        return W_ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->close != NULL)
    {
        err = dev->ops->close(dev);
        dev->opened = (err != W_ERR_OK)?W_TRUE:W_FALSE;
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_dev_print(w_dlist_s *list)
{
    w_dnode_s *dnode;
    w_chdev_s *dev;
    int cnt = 0;
    WIND_ASSERT_RETURN(list != NULL,W_ERR_NULL);
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


