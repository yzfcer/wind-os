/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_blkdev.c
**创   建   人: Jason Zhou
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
#if WIND_BLKDEV_SUPPORT
#define NODE_TO_BLKDEV(node) (w_blkdev_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_blkdev_s*)0)->obj.objnode)))
static w_dlist_s blkdevlist;

w_err_t wind_blkdev_register(w_blkdev_s *blkdev,w_int32_t count)
{
    w_int32_t i;
    w_blkdev_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(blkdev[i].obj.magic == (~WIND_BLKDEV_MAGIC),W_ERR_INVALID);
        WIND_ASSERT_RETURN(blkdev[i].obj.name != W_NULL,W_ERR_PTR_NULL);
        wind_notice("register blkdev:%s",wind_obj_name(&blkdev[i].obj));
        devi = wind_blkdev_get(blkdev[i].obj.name);
        if(devi != W_NULL)
        {
            wind_notice("device has been registered.\r\n");
            continue;
        }
        if(blkdev[i].ops->init)
        {
            err = blkdev[i].ops->init(&blkdev[i]);
            if(err != W_ERR_OK)
            {
                wind_error("blkdev:%s init failed:%d.",blkdev[i].obj.name,err);
                continue;
            }
        }
        blkdev[i].mutex = wind_mutex_create(blkdev[i].obj.name);
        wind_obj_init(&blkdev[i].obj,WIND_BLKDEV_MAGIC,blkdev[i].obj.name,&blkdevlist);
    }
    return W_ERR_OK;
}

w_err_t wind_blkdev_unregister(w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    wind_notice("unregister blkdev:%s",blkdev->obj.name);
    err = wind_obj_deinit(&blkdev->obj,WIND_BLKDEV_MAGIC,&blkdevlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    if(blkdev->ops->deinit)
        blkdev->ops->deinit(blkdev);
    wind_mutex_destroy(blkdev->mutex);
    blkdev->mutex = W_NULL;
    return W_ERR_OK;
}


w_err_t _wind_blkdev_mod_init(void)
{
    DLIST_INIT(blkdevlist);
    _register_blkdevs();
    return W_ERR_OK;
}

w_blkdev_s *wind_blkdev_get(const char *name)
{
    return (w_blkdev_s*)wind_obj_get(name,&blkdevlist);
}

w_err_t wind_blkdev_open(w_blkdev_s *blkdev)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    if(IS_F_BLKDEV_OPEN(blkdev))
        return W_ERR_OK;
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->open != W_NULL)
    {
        err = blkdev->ops->open(blkdev);
        (err == W_ERR_OK)?SET_F_BLKDEV_OPEN(blkdev):CLR_F_BLKDEV_OPEN(blkdev);
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
    
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_BLKDEV_OPEN(blkdev),W_ERR_STATUS);
    
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
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_BLKDEV_OPEN(blkdev),W_ERR_STATUS);
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
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_BLKDEV_OPEN(blkdev),W_ERR_STATUS);
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
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_BLKDEV_OPEN(blkdev),W_ERR_STATUS);
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
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->ops != W_NULL,W_ERR_INVALID);
    if(!IS_F_BLKDEV_OPEN(blkdev))
        return W_ERR_OK;
    wind_mutex_lock(blkdev->mutex);
    if(blkdev->ops->close != W_NULL)
    {
        err = blkdev->ops->close(blkdev);
        (err == W_ERR_OK)?CLR_F_BLKDEV_OPEN(blkdev):SET_F_BLKDEV_OPEN(blkdev);
    }
    wind_mutex_unlock(blkdev->mutex);
    return err;
}


w_err_t wind_blkdev_print(void)
{
    w_dnode_s *dnode;
    w_blkdev_s *blkdev;
    w_dlist_s *list = &blkdevlist;
    wind_printf("\r\n\r\nblock device list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-12s %-10s\r\n","blkdev","blockcnt","blocksize");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        blkdev = NODE_TO_BLKDEV(dnode);
        wind_printf("%-16s %-12d %-10d\r\n",
            blkdev->obj.name,blkdev->blkcnt,blkdev->blksize);
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif


