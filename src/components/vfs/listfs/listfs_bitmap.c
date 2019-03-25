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
        if(blk[i] == 0)
            return i;
    return -1;
}

static w_err_t do_set_free_bitmap(listfs_s *listfs,w_int32_t blkidx,w_uint8_t *blk,w_int32_t size)
{
    w_int32_t byteidx;
    lfs_info_s *info = &listfs->lfs_info;
    wind_blkdev_read(listfs->blkdev,info->bitmap1+blkidx,blk,1);
    byteidx = get_blk_free_byteidx(blk,listfs->blkdev->blksize);
    if(byteidx >= 0)
    {
        listfs->free_blkidx = blkidx * listfs->blkdev->blksize;
        listfs->free_byteidx = byteidx;
        wind_notice("set free bitmap:%d,%d",listfs->free_blkidx,listfs->free_byteidx);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

static w_err_t set_free_bitmap_start(listfs_s *listfs,w_int32_t blkidx,w_int32_t byteidx)
{
    w_int32_t i;
    w_err_t err;
    lfs_info_s *info = &listfs->lfs_info;
    w_uint8_t *blk = listfs_malloc(listfs->blkdev->blksize);
    for(i = blkidx;i < info->bitmap_cnt;i ++)
    {
        err = do_set_free_bitmap(listfs,i,blk,listfs->blkdev->blksize);
        if(err = W_ERR_OK)
        {
            listfs_free(blk);
            return err;
        }
    }
    for(i = 0;i < blkidx;i ++)
    {
        err = do_set_free_bitmap(listfs,i,blk,listfs->blkdev->blksize);
        if(err = W_ERR_OK)
        {
            listfs_free(blk);
            return err;
        }
    }

    listfs->free_blkidx = -1;
    listfs->free_byteidx = -1;
    wind_notice("set free bitmap failed");
    listfs_free(blk);
    return W_ERR_FAIL;
}

w_err_t listfs_bitmap_init(listfs_s *listfs)
{
    w_err_t err;
    err = set_free_bitmap_start(listfs,0,0);
    return err;
}

w_err_t listfs_bitmap_set(listfs_s *listfs, w_int32_t unit_idx, w_uint8_t flag)
{
    w_int32_t bmidx,byteidx;
    lfs_info_s *info = &listfs->lfs_info;
    w_uint8_t *blk = listfs_malloc(listfs->blkdev->blksize);
    bmidx = unit_idx / listfs->lfs_info.unit_size;
    byteidx = unit_idx % listfs->lfs_info.unit_size;
    wind_blkdev_read(listfs->blkdev,info->bitmap1+bmidx,blk,1);
    blk[byteidx] = flag;
    wind_blkdev_write(listfs->blkdev,info->bitmap1+bmidx,blk,1);
    wind_blkdev_write(listfs->blkdev,info->bitmap2+bmidx,blk,1);
    listfs_free(blk);
    return W_ERR_OK;
}


w_err_t listfs_bitmap_find_free(listfs_s *listfs,w_int32_t *freeidx)
{
    w_int32_t bmidx,byteidx;
    WIND_ASSERT_RETURN(listfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(listfs->free_blkidx != -1,W_ERR_INVALID);
    WIND_ASSERT_RETURN(listfs->free_byteidx != -1,W_ERR_INVALID);
    *freeidx = listfs->free_blkidx * listfs->lfs_info.unit_size + listfs->free_byteidx;
    listfs_bitmap_set(listfs,*freeidx,BITMAP_USED);
    set_free_bitmap_start(listfs,listfs->free_blkidx,listfs->free_byteidx);
    return W_ERR_OK;
}


#endif

