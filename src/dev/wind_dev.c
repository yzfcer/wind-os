/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_driver.c
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ϵͳ����������ص�Դ�ļ�
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
#include "wind_os_hwif.h"
#include "wind_string.h"
#include "wind_var.h"
//dlist_s dev_list;
w_err_t wind_register_dev(dev_s *dev,w_int32_t count)
{
    dev_s *devi;    
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    
    wind_notice("register dev:%s",dev->name);
    wind_close_interrupt();
    foreach_node(dnode,&g_core.devlist)
    {
        devi = DLIST_OBJ(dnode,dev_s,devnode);
        if(wind_strcmp(dev->name,devi->name) == 0)
        {
            wind_error("device has been registered.\r\n");
            wind_open_interrupt();
            return ERR_COMMAN;
        }
    }
    dev->mutex = wind_mutex_create(dev->name);
    dlist_insert_tail(&g_core.devlist,&dev->devnode);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_unregister_dev(dev_s *dev)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    wind_notice("unregister dev:%s",dev->name);
    wind_close_interrupt();
    dnode = dlist_remove(&g_core.devlist,dnode);
    if(dnode == NULL)
    {
        wind_error("device has NOT been registered.\r\n");
        wind_open_interrupt();
        return ERR_COMMAN;
    }
    wind_mutex_destroy(dev->mutex);
    dev->mutex = NULL;
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_devices_init(void)
{
    //DLIST_INIT(dev_list);
    _register_devs();
    return ERR_OK;
}

dev_s *wind_dev_get(char *name)
{
    dev_s *dev;
    dnode_s *dnode;
    foreach_node(dnode,&g_core.devlist)
    {
        dev = DLIST_OBJ(dnode,dev_s,devnode);
        if(wind_strcmp(dev->name,name) == 0)
            return dev;
    }
    return NULL;
}

w_err_t wind_dev_open(dev_s *dev)
{
    w_err_t err = ERR_COMMAN;
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

w_err_t wind_dev_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    w_err_t err = ERR_COMMAN;
    WIND_ASSERT_RETURN(dev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(dev->magic == WIND_DEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(dev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(dev->mutex);
    if(dev->ops->open != NULL)
        err = dev->ops->ioctl(dev,ctrlpoint,param);
    wind_mutex_unlock(dev->mutex);
    return err;
}

w_int32_t wind_dev_read(dev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = ERR_COMMAN;
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
    w_err_t err = ERR_COMMAN;
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


