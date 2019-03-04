/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_dev.c
**��   ��   ��: Jason Zhou
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
        WIND_ASSERT_RETURN(dev[i].obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
        wind_notice("register dev:%s",dev[i].obj.name);
        devi = wind_chdev_get(dev[i].obj.name);
        if(devi != W_NULL)
        {
            wind_notice("device has been registered.\r\n");
            continue;
        }
        if(dev[i].ops->init)
        {
            err = dev[i].ops->init(&dev[i]);
            if(err != W_ERR_OK)
            {
                wind_error("blkdev:%s init failed:%d.",dev[i].obj.name,err);
                continue;
            }
        }
        dev[i].mutex = wind_mutex_create(dev[i].obj.name);
        wind_disable_switch();
        dlist_insert_tail(&devlist,&dev[i].obj.objnode);
        wind_enable_switch();
    }
    return W_ERR_OK;
}

w_err_t wind_chdev_unregister(w_chdev_s *dev)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    wind_notice("unregister dev:%s",dev->obj.name);
    wind_disable_switch();
    dnode = dlist_remove(&devlist,&dev->obj.objnode);
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

w_chdev_s *wind_chdev_get(const char *name)
{
    return (w_chdev_s*)wind_obj_get(name,&devlist);
}

w_err_t wind_chdev_open(w_chdev_s *dev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    if(IS_F_CHDEV_OPEN(dev))
        return W_ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != W_NULL)
    {
        err = dev->ops->open(dev);
        err == W_ERR_OK ? SET_F_CHDEV_OPEN(dev) : CLR_F_CHDEV_OPEN(dev);
    }
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_err_t wind_chdev_ioctl(w_chdev_s *dev,w_int32_t cmd,void *param)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dev->obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_CHDEV_OPEN(dev) == W_TRUE,W_ERR_STATUS);
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
    WIND_ASSERT_RETURN(dev->obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_CHDEV_OPEN(dev) == W_TRUE,W_ERR_STATUS);
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
    WIND_ASSERT_RETURN(dev->obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_CHDEV_OPEN(dev) == W_TRUE,W_ERR_STATUS);
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
    WIND_ASSERT_RETURN(dev->obj.magic == WIND_DEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dev->ops != W_NULL,W_ERR_INVALID);
    if(!IS_F_CHDEV_OPEN(dev))
        return W_ERR_OK;
    wind_mutex_lock(dev->mutex);
    if(dev->ops->close != W_NULL)
    {
        err = dev->ops->close(dev);
        err == W_ERR_OK ? CLR_F_CHDEV_OPEN(dev) : SET_F_CHDEV_OPEN(dev);
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
    wind_printf("\r\n\r\nchdev list:\r\n");
    
    foreach_node(dnode,list)
    {
        dev = (w_chdev_s *)DLIST_OBJ(dnode,w_chdev_s,obj.objnode);
        wind_printf("%-12s ",dev->obj.name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#endif


