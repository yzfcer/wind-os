/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_blkdev.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 块设备标准化API接口
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

w_err_t wind_blkdev_register(w_blkdev_s *blkdev,w_int32_t count)
{
    w_int32_t i;
    w_blkdev_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(blkdev[i].magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
        wind_notice("register blkdev:%s",blkdev[i].name);
        devi = wind_blkdev_get(blkdev[i].name);
        if(devi != W_NULL)
        {
            wind_error("device has been registered.\r\n");
            continue;
        }
        if(blkdev[i].ops->init)
        {
            err = blkdev[i].ops->init(&blkdev[i]);
            if(err != W_ERR_OK)
            {
                wind_error("blkdev:%s init failed:%d.",blkdev[i].name,err);
                continue;
            }
        }
        blkdev[i].mutex = wind_mutex_create(blkdev[i].name);
        wind_disable_switch();
        dlist_insert_tail(&g_core.blkdevlist,&blkdev[i].blkdevnode);
        wind_enable_switch();    
    }
    return W_ERR_OK;
}

w_err_t wind_blkdev_unregister(w_blkdev_s *blkdev)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    wind_notice("unregister blkdev:%s",blkdev->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.blkdevlist,dnode);
    wind_enable_switch();
    if(dnode == W_NULL)
    {
        wind_error("blkdevice has NOT been registered.\r\n");
        return W_ERR_FAIL;
    }
    wind_mutex_destroy(blkdev->mutex);
    blkdev->mutex = W_NULL;
    return W_ERR_OK;
}


w_err_t _wind_blkdev_mod_init(void)
{
    _register_blkdevs();
    return W_ERR_OK;
}

w_blkdev_s *wind_blkdev_get(char *name)
{
    w_blkdev_s *blkdev = W_NULL;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.blkdevlist)
    {
        blkdev = DLIST_OBJ(dnode,w_blkdev_s,blkdevnode);
        if(wind_strcmp(blkdev->name,name) == 0)
        {
            wind_enable_switch();
            return blkdev;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_blkdev_open(w_blkdev_s *blkdev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    if(blkdev->opened)
        return W_ERR_OK;
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->open != W_NULL)
    {
        err = blkdev->ops->open(blkdev);
        blkdev->opened = (err == W_ERR_OK)?W_TRUE:W_FALSE;
    }
    wind_mutex_unlock(blkdev->mutex);
    return err;
}


w_int32_t wind_blkdev_read(w_blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buf != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkaddr < blkdev->blkcnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkaddr + blkcnt <= blkdev->blkcnt,W_ERR_INVALID);
    
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->opened == W_TRUE,W_ERR_STATUS);
    
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->read != W_NULL)
        cnt = blkdev->ops->read(blkdev,blkaddr,buf,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return cnt;
}

w_int32_t wind_blkdev_write(w_blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buf != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkaddr < blkdev->blkcnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkaddr + blkcnt <= blkdev->blkcnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->write != W_NULL)
        cnt = blkdev->ops->write(blkdev,blkaddr,buf,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return cnt;
}

w_err_t wind_blkdev_erase(w_blkdev_s *blkdev,w_addr_t blkaddr,w_int32_t blkcnt)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->erase!= W_NULL)
        err = blkdev->ops->erase(blkdev,blkaddr,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return err;
}

w_err_t wind_blkdev_eraseall(w_blkdev_s *blkdev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->opened == W_TRUE,W_ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->eraseall!= W_NULL)
        err = blkdev->ops->eraseall(blkdev);
    wind_mutex_unlock(blkdev->mutex);
    return err;
}

w_err_t wind_blkdev_close(w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    if(!blkdev->opened)
        return W_ERR_OK;
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->close != W_NULL)
    {
        err = blkdev->ops->close(blkdev);
        blkdev->opened = (err != W_ERR_OK)?W_TRUE:W_FALSE;
    }
    wind_mutex_unlock(blkdev->mutex);
    return err;
}


w_err_t wind_blkdev_print(w_dlist_s *list)
{
    w_dnode_s *dnode;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nblock device list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-12s %-10s\r\n","blkdev","blockcnt","blocksize");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        blkdev = (w_blkdev_s *)DLIST_OBJ(dnode,w_blkdev_s,blkdevnode);
        wind_printf("%-16s %-12d %-10d\r\n",
            blkdev->name,blkdev->blkcnt,blkdev->blksize);
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif


