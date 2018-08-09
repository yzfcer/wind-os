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

w_err_t wind_blkdev_register(blkdev_s *blkdev,w_int32_t count)
{
    w_int32_t i;
    blkdev_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(blkdev[i].magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
        wind_notice("register blkdev:%s",blkdev[i].name);
        devi = wind_blkdev_get(blkdev[i].name);
        if(devi != NULL)
        {
            wind_error("device has been registered.\r\n");
            continue;
        }
        if(blkdev[i].ops->init)
        {
            err = blkdev[i].ops->init(&blkdev[i]);
            if(err != ERR_OK)
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
    return ERR_OK;
}

w_err_t wind_blkdev_unregister(blkdev_s *blkdev)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    wind_notice("unregister blkdev:%s",blkdev->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.blkdevlist,dnode);
    wind_enable_switch();
    if(dnode == NULL)
    {
        wind_error("blkdevice has NOT been registered.\r\n");
        return ERR_FAIL;
    }
    wind_mutex_destroy(blkdev->mutex);
    blkdev->mutex = NULL;
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
        {
            wind_enable_switch();
            return blkdev;
        }
    }
    wind_enable_switch();
    return NULL;
}

w_err_t wind_blkdev_open(blkdev_s *blkdev)
{
    w_err_t err = ERR_FAIL;
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


w_int32_t wind_blkdev_read(blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkcnt > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr < blkdev->blkcnt,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr + blkcnt <= blkdev->blkcnt,ERR_INVALID_PARAM);
    
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->read != NULL)
        cnt = blkdev->ops->read(blkdev,blkaddr,buf,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return cnt;
}

w_int32_t wind_blkdev_write(blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt = -1;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(buf != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkcnt > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr < blkdev->blkcnt,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkaddr + blkcnt <= blkdev->blkcnt,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->write != NULL)
        cnt = blkdev->ops->write(blkdev,blkaddr,buf,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return cnt;
}

w_err_t wind_blkdev_erase(blkdev_s *blkdev,w_addr_t blkaddr,w_int32_t blkcnt)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(blkdev != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkcnt > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->magic == WIND_BLKDEV_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->ops != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(blkdev->opened == B_TRUE,ERR_STATUS);
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->erase!= NULL)
        err = blkdev->ops->erase(blkdev,blkaddr,blkcnt);
    wind_mutex_unlock(blkdev->mutex);
    return err;
}

w_err_t wind_blkdev_eraseall(blkdev_s *blkdev)
{
    w_err_t err = ERR_FAIL;
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


w_err_t wind_blkdev_print(dlist_s *list)
{
    dnode_s *dnode;
    blkdev_s *blkdev;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nblock device list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-12s %-10s\r\n","blkdev","blockcnt","blocksize");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        blkdev = (blkdev_s *)DLIST_OBJ(dnode,blkdev_s,blkdevnode);
        wind_printf("%-16s %-12d %-10d\r\n",
            blkdev->name,blkdev->blkcnt,blkdev->blksize);
    }
    wind_print_space(5);
    return ERR_OK;
}

#endif


