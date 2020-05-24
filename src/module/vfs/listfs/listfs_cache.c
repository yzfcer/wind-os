/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: listfs_cache.c
**Author: Jason Zhou
**Last Date: 2019.04.05
**Description: 文件系统对块设备的操作接口
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2019.04.05
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2019.04.05
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "listfs_cache.h"
#include "listfs.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_obj.h"
#include "wind_heap.h"
#include "wind_core.h"
#if WIND_LISTFS_SUPPORT

#define NODE_TO_CACHEITEM(node) (lcache_item_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((lcache_item_s*)0)->itemnode)))
w_dlist_s lfscache_list = {W_NULL,W_NULL};

w_err_t lfs_cache_init(lfs_cache_s *cache,w_uint32_t itemcount,w_uint32_t blksize)
{
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(itemcount > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blksize > 0,W_ERR_INVALID);
    wind_memset(cache,0,sizeof(lfs_cache_s));
    cache->itemcount = itemcount;
    //cache->itemsize = sizeof(lcache_item_s) + blksize;
    wind_obj_init(&cache->obj,LISTFS_CACHE_MAGIC,W_NULL,&lfscache_list);
    return W_ERR_OK;
}

static lcache_item_s *hit_cacheitem(lfs_cache_s *cache,w_uint32_t addr)
{
    w_dnode_s *dnode;
    lcache_item_s *cacheitem;
    foreach_node(dnode,&cache->itemlist)
    {
        cacheitem = NODE_TO_CACHEITEM(dnode);
        if(cacheitem->addr == addr)
        {
            SET_F_LFSCACHE_HIT(cacheitem);
            return cacheitem;
        }
    }
    return W_NULL;
}
static lcache_item_s *alloc_cacheitem(lfs_cache_s *cache,w_int32_t blksize)
{
    w_dnode_s *dnode;
    w_uint32_t itemsize;
    lcache_item_s *cacheitem;
    
    if(cache->itemcount < LFS_CACHEITEM_MAX_CNT)
    {
        itemsize = sizeof(lcache_item_s) + blksize;
        cacheitem = listfs_mem_malloc(itemsize);
        cacheitem->flag = 0;
        WIND_ASSERT_TODO_RETURN(cacheitem == W_NULL,wind_enable_switch(),W_NULL);
        cache->itemcount ++;
    }
    else
    {
        dnode = dlist_remove_tail(&cache->itemlist);
        cacheitem = NODE_TO_CACHEITEM(dnode);
        SET_F_LFSCACHE_SW(cacheitem);
    }
    return cacheitem;
}

static w_err_t cacheitem_init(lcache_item_s *cacheitem,w_uint32_t addr,w_int32_t blksize)
{
    cacheitem->addr = addr;
    cacheitem->blksize = (w_int16_t)blksize;
    cacheitem->blk = (w_uint8_t*)&cacheitem[1];
    DNODE_INIT(cacheitem->itemnode);
    return W_ERR_OK;
}

w_err_t lfs_cache_read(lfs_cache_s *cache,w_blkdev_s *blkdev,w_uint32_t addr,w_uint8_t *blk)
{
    lcache_item_s *cacheitem;
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr > 0,W_ERR_INVALID);
    wind_disable_switch();
    cacheitem = hit_cacheitem(cache,addr);
    if(cacheitem != W_NULL)
    {
        cache->r_hit ++;
        wind_memcpy(blk,cacheitem->blk,blkdev->blksize);
        dlist_remove(&cache->itemlist,&cacheitem->itemnode);
        dlist_insert_head(&cache->itemlist,&cacheitem->itemnode);
        wind_enable_switch();
        return W_ERR_OK;
    }

    cache->r_miss ++;
    cacheitem = alloc_cacheitem(cache,blkdev->blksize);
    WIND_ASSERT_TODO_RETURN(cacheitem != W_NULL,wind_enable_switch(),W_ERR_MEM);
    if(IS_F_LFSCACHE_DIRTY(cache))
    cacheitem_init(cacheitem,addr,blkdev->blksize);
    wind_memcmp(cacheitem->blk,blk,blkdev->blksize);
    dlist_insert_head(&cache->itemlist,&cacheitem->itemnode);
    
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t lfs_cache_write(lfs_cache_s *cache,w_blkdev_s *blkdev,w_uint32_t addr,w_uint8_t *blk)
{
    lcache_item_s *cacheitem;
    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr > 0,W_ERR_INVALID);
    wind_disable_switch();
    cacheitem = hit_cacheitem(cache,addr);
    if(cacheitem != W_NULL)
    {
        cache->w_hit ++;
        wind_memcpy(cacheitem->blk,blk,blkdev->blksize);
        dlist_remove(&cache->itemlist,&cacheitem->itemnode);
        dlist_insert_head(&cache->itemlist,&cacheitem->itemnode);
        wind_enable_switch();
        return W_ERR_OK;
    }
    
    cache->w_miss ++;
    
    cacheitem = alloc_cacheitem(cache,blkdev->blksize);
    WIND_ASSERT_TODO_RETURN(cacheitem != W_NULL,wind_enable_switch(),W_ERR_MEM);
    cacheitem_init(cacheitem,addr,blkdev->blksize);
    wind_memcmp(cacheitem->blk,blk,blkdev->blksize);
    dlist_insert_head(&cache->itemlist,&cacheitem->itemnode);
    wind_enable_switch();
    return W_ERR_OK;
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
    return W_ERR_OK;
}

#endif


