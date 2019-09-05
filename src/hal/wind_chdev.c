/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_chdev.c
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
#if WIND_CHDEV_SUPPORT
#define NODE_TO_CHDEV(node) (w_chdev_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_chdev_s*)0)->obj.objnode)))
static w_dlist_s blkdevlist;

w_err_t wind_chdev_register(w_chdev_s *chdev,w_int32_t count)
{
    w_int32_t i;
    w_chdev_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(chdev[i].obj.magic == (~WIND_CHDEV_MAGIC),W_ERR_INVALID);
        WIND_ASSERT_RETURN(chdev[i].obj.name != W_NULL,W_ERR_PTR_NULL);
        wind_notice("register chdev:%s",wind_obj_name(&chdev[i].obj));
        devi = wind_chdev_get(chdev[i].obj.name);
        if(devi != W_NULL)
        {
            wind_notice("device has been registered.\r\n");
            continue;
        }
        if(chdev[i].ops->init)
        {
            err = chdev[i].ops->init(&chdev[i]);
            if(err != W_ERR_OK)
            {
                wind_error("blkdev:%s init failed:%d.",chdev[i].obj.name,err);
                continue;
            }
        }
        chdev[i].mutex = wind_mutex_create(chdev[i].obj.name);
        wind_obj_init(&chdev[i].obj,WIND_CHDEV_MAGIC,chdev[i].obj.name,&blkdevlist);
    }
    return W_ERR_OK;
}

w_err_t wind_chdev_unregister(w_chdev_s *chdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(chdev->obj.magic == WIND_CHDEV_MAGIC,W_ERR_INVALID);
    wind_notice("unregister chdev:%s",chdev->obj.name);
    err = wind_obj_deinit(&chdev->obj,WIND_CHDEV_MAGIC,&blkdevlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    if(chdev->ops->deinit)
        chdev->ops->deinit(chdev);
    wind_mutex_destroy(chdev->mutex);
    chdev->mutex = W_NULL;
    return W_ERR_OK;
}

extern w_chdev_s stddev_dev[1];

w_err_t _wind_chdev_mod_init(void)
{
    DLIST_INIT(blkdevlist);
    wind_chdev_register(stddev_dev,1);
    _wind_register_chdevs();
    return W_ERR_OK;
}

w_chdev_s *wind_chdev_get(const char *name)
{
    return (w_chdev_s*)wind_obj_get(name,&blkdevlist);
}

w_err_t wind_chdev_open(w_chdev_s *chdev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(chdev->obj.magic == WIND_CHDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(chdev->ops != W_NULL,W_ERR_INVALID);
    if(IS_F_CHDEV_OPEN(chdev))
        return W_ERR_OK;
    wind_mutex_lock(chdev->mutex);
    if(chdev->ops->open != W_NULL)
    {
        err = chdev->ops->open(chdev);
        err == W_ERR_OK ? SET_F_CHDEV_OPEN(chdev) : CLR_F_CHDEV_OPEN(chdev);
    }
    wind_mutex_unlock(chdev->mutex);
    return err;
}

w_err_t wind_chdev_ioctl(w_chdev_s *chdev,w_int32_t cmd,void *param)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(chdev->obj.magic == WIND_CHDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(chdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_CHDEV_OPEN(chdev),W_ERR_STATUS);
    wind_mutex_lock(chdev->mutex);
    if(chdev->ops->open != W_NULL)
        err = chdev->ops->ioctl(chdev,cmd,param);
    wind_mutex_unlock(chdev->mutex);
    return err;
}

w_int32_t wind_chdev_read(w_chdev_s *chdev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(chdev->obj.magic == WIND_CHDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(chdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_CHDEV_OPEN(chdev),W_ERR_STATUS);
    wind_mutex_lock(chdev->mutex);
    if(chdev->ops->read != W_NULL)
        err = chdev->ops->read(chdev,buf,len);
    wind_mutex_unlock(chdev->mutex);
    return err;
}

w_int32_t wind_chdev_write(w_chdev_s *chdev,w_uint8_t *buf,w_int32_t len)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(chdev->obj.magic == WIND_CHDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(chdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_CHDEV_OPEN(chdev),W_ERR_STATUS);
    wind_mutex_lock(chdev->mutex);
    if(chdev->ops->write != W_NULL)
        err = chdev->ops->write(chdev,buf,len);
    wind_mutex_unlock(chdev->mutex);
    return err;
}

w_err_t wind_chdev_close(w_chdev_s *chdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(chdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(chdev->obj.magic == WIND_CHDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(chdev->ops != W_NULL,W_ERR_INVALID);
    if(!IS_F_CHDEV_OPEN(chdev))
        return W_ERR_OK;
    wind_mutex_lock(chdev->mutex);
    if(chdev->ops->close != W_NULL)
    {
        err = chdev->ops->close(chdev);
        err == W_ERR_OK ? CLR_F_CHDEV_OPEN(chdev) : SET_F_CHDEV_OPEN(chdev);
    }
    wind_mutex_unlock(chdev->mutex);
    return err;
}

w_err_t wind_chdev_print(void)
{
    w_dnode_s *dnode;
    w_chdev_s *chdev;
    int cnt = 0;
    w_dlist_s *list = &blkdevlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nchdev list:\r\n");
    
    foreach_node(dnode,list)
    {
        chdev = NODE_TO_CHDEV(dnode);
        wind_printf("%-12s ",chdev->obj.name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#endif


