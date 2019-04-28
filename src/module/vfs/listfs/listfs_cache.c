/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_cache.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: �ļ�ϵͳ�Կ��豸�Ĳ����ӿ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.04.05
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.04.05
** �衡��: 
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




