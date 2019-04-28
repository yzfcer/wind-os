/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_cache.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统对块设备的操作接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.04.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.04.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "listfs_cache.h"
#include "wind_debug.h"
#include "wind_string.h"
#define NODE_TO_CACHEITEM(node) (lcache_item_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((lcache_item_s*)0)->itemnode)))
w_err_t lfs_cache_init(lfs_cache_s *cache)
{
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    cache->itemcount = 0;
    wind_obj_init(&cache->obj,LISTFS_CACHE_MAGIC,W_NULL,w_dlist_s * list)
    cache->magic = LISTFS_CACHE_MAGIC;
    DLIST_INIT(cache->itemlist);
    return W_ERR_FAIL;
}

w_err_t lfs_cache_read(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk)
{
    w_dnode_s *dnode;
    lcache_item_s *cacheitem;
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr > 0,W_ERR_INVALID);
    foreach_node(dnode,&cache->itemlist)
    {
        cacheitem = NODE_TO_CACHEITEM(dnode);
        if(cacheitem->addr == addr)
        {
            wind_memcpy(blk,cacheitem->blk,blkdev->blksize);
            return 
        }
    }
    
    return W_ERR_FAIL;
}

w_err_t lfs_cache_write(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk)
{
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}

w_err_t lfs_cache_flush(lfs_cache_s *cache,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    return W_ERR_FAIL;
}

w_err_t lfs_cache_clear(lfs_cache_s *cache,w_blkdev_s *blkdev)
{
    w_err_t err;
    err = lfs_cache_flush(cache,blkdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
}




