/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_bitmap.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统对块位图的操作接口
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
#include "listfs.h"
#include "listfs_bitmap.h"
#include "wind_tree.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_FS_SUPPORT

static w_int32_t get_blk_free_byteidx(w_uint8_t *blk,w_int32_t size)
{
    w_int32_t i;
    for(i = 0;i < size;i ++)
    {
        if(blk[i] == 0)
            return i;
    }
    return -1;
}

static w_err_t do_set_free_bitmap(lfs_bitmap_s *bp,w_addr_t blkidx,w_uint8_t *blk)
{
    w_int32_t byteidx;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_blkdev_read(bp->blkdev,blkidx,blk,1);
    byteidx = get_blk_free_byteidx(blk,bp->blkdev->blksize);
    if(byteidx >= 0)
    {
        bp->free_blkidx = blkidx;
        bp->free_byteidx = byteidx;
        wind_notice("set free bitmap:%d,%d",bp->free_blkidx,bp->free_byteidx);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}


w_err_t listfs_bitmap_init(lfs_bitmap_s *bp,w_addr_t start_addr,w_int32_t blk_cnt,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_notice("listfs_bitmap_init,blkdev:%s",wind_obj_name(&blkdev->obj));
    bp->addr1 = start_addr;
    bp->addr2 = start_addr + blk_cnt;
    bp->addr_cnt = blk_cnt;
    bp->free_blkidx = 0;
    bp->free_byteidx = 0;
    bp->blkdev = blkdev;
    return W_ERR_OK;
}

w_err_t listfs_bitmap_update(lfs_bitmap_s *bp)
{
    w_uint32_t i;
    w_err_t err;
    w_uint8_t *blk;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_INVALID);
    blk = lfs_malloc(bp->blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    for(i = bp->free_blkidx;i < bp->addr_cnt;i ++)
    {
        err = do_set_free_bitmap(bp,i,blk);
        if(err == W_ERR_OK)
        {
            lfs_free(blk);
            return err;
        }
    }
    for(i = 0;i < bp->free_blkidx;i ++)
    {
        err = do_set_free_bitmap(bp,i,blk);
        if(err == W_ERR_OK)
        {
            lfs_free(blk);
            return err;
        }
    }

    bp->free_blkidx = 0;
    bp->free_byteidx = 0;
    wind_notice("set free bitmap failed");
    lfs_free(blk);
    return W_ERR_FAIL;
}


w_err_t listfs_bitmap_set(lfs_bitmap_s *bp,w_addr_t addr,w_uint8_t bitflag)
{
    w_int32_t byteidx;
    w_uint8_t *blk;
    w_addr_t addr_idx,addr_st;
    
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    addr_st = bp->addr2 + bp->addr_cnt;
    WIND_ASSERT_RETURN(addr >= addr_st,W_ERR_INVALID);
    addr_idx = (addr - addr_st) / bp->blkdev->blksize;
    byteidx = addr % bp->blkdev->blksize;
    blk = lfs_malloc(bp->blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    
    wind_blkdev_read(bp->blkdev,bp->addr1+addr_idx,blk,1);
    blk[byteidx] = bitflag;
    wind_blkdev_write(bp->blkdev,bp->addr1+addr_idx,blk,1);
    wind_blkdev_write(bp->blkdev,bp->addr2+addr_idx,blk,1);
    lfs_free(blk);
    return W_ERR_OK;
}


w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t cnt)
{
    w_int32_t i,idx;
    w_err_t err = W_ERR_OK;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    
    if((bp->free_blkidx == 0) && (bp->free_byteidx == 0))
        listfs_bitmap_update(bp);
    wind_memset(addr,0,sizeof(w_addr_t)*cnt);
    for(i = 0;i < cnt;i ++)
    {
        idx = bp->free_blkidx * bp->blkdev->blksize;
        idx += bp->free_byteidx;
        addr[i] = bp->addr2 + bp->addr_cnt + idx;
        WIND_ASSERT_BREAK(addr[i] < bp->blkdev->blkcnt,W_ERR_FAIL,"addr is overflow.");
        
        bp->free_byteidx ++;
        if(bp->free_byteidx >= bp->blkdev->blksize)
        {
            bp->free_blkidx ++;
            bp->free_byteidx = 0;
        }
        listfs_bitmap_update(bp);
    }
    if(err != W_ERR_OK)
    {
        for(i = 0;i < cnt;i ++)
        {
            if(addr[i] != 0)
                listfs_bitmap_set(bp,addr[i],0);
        }
    }
    return err;
}

w_err_t listfs_bitmap_free_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t count)
{
    w_int32_t i;
    //w_err_t err;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    for(i = 0;i < count;i ++)
    {
        if(addr[i] != 0)
            listfs_bitmap_set(bp,addr[i],0);
    }
    return W_ERR_OK;
}

#if 0
w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t count)
{
    w_int32_t i;
    w_err_t err;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    for(i = 0;i < count;i ++)
        addr[i] = 0;
    err = listfs_bitmap_alloc_blk(bp,&addr[i],count);
    WIND_ASSERT_BREAK(err == W_ERR_OK,err,"alloc bitmap blk failed.");
    if(i < count)
    {
        listfs_bitmap_free_blk(bp,addr,count);
        return W_ERR_FAIL;
    }
    return W_ERR_OK;
}
#endif

w_err_t listfs_bitmap_clear(lfs_bitmap_s *bp)
{
    w_int32_t i;
    w_uint8_t *blk;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_notice("listfs_bitmap_clear");
    blkdev = bp->blkdev;
    blk = lfs_malloc(blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    blk = lfs_malloc(blkdev->blksize);
    wind_memset(blk,0,blkdev->blksize);
    for(i = 1;i < bp->addr_cnt; i ++)
        wind_blkdev_write(blkdev,bp->addr1+1,blk,1);
    for(i = 1;i < bp->addr_cnt; i ++)
        wind_blkdev_write(blkdev,bp->addr1+1,blk,1);
    lfs_free(blk);
    return W_ERR_OK;
}

static w_uint32_t calc_blkused(w_uint8_t *blk,w_int32_t blksize)
{
    w_int32_t i;
    w_uint32_t blkused = 0;
    for(i = 0;i < blksize;i ++)
    {
        if(blk[i] != 0)
            blkused ++;
    }
    return blkused;
}

w_int32_t listfs_bitmap_get_usedblk(lfs_bitmap_s *bp)
{
    w_int32_t i;
    w_err_t err;
    w_uint8_t *blk;
    w_blkdev_s *blkdev;
    w_uint32_t blkused = 0;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    blkdev = bp->blkdev;
    blk = lfs_malloc(blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    blk = lfs_malloc(blkdev->blksize);
    for(i = 1;i < bp->addr_cnt; i ++)
    {
        err = wind_blkdev_read(blkdev,bp->addr1+1,blk,1);
        WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,lfs_free(blk),0xffffffff);
        blkused += calc_blkused(blk,blkdev->blksize);
    }
    return blkused;    
}

#endif

