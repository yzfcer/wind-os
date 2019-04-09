/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_bitmap.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: �ļ�ϵͳ�Կ�λͼ�Ĳ����ӿ�
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


w_err_t listfs_bitmap_init(lfs_bitmap_s *bp,w_addr_t addr,w_int32_t blk_cnt,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    bp->addr1 = addr;
    bp->addr2 = addr+blk_cnt;
    bp->addr_cnt = blk_cnt;
    bp->free_blkidx = 0;
    bp->free_byteidx = 0;
    bp->blkdev = blkdev;
    return W_ERR_OK;
}

w_err_t listfs_bitmap_update(lfs_bitmap_s *bp)
{
    w_int32_t i;
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


w_err_t listfs_bitmap_find_free(lfs_bitmap_s *bp,w_addr_t *addr)
{
    w_err_t err;
    w_uint8_t *blk;
    w_int32_t idx;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    if((bp->free_blkidx == 0) && (bp->free_byteidx == 0))
        listfs_bitmap_update(bp);
    idx = bp->free_blkidx * bp->blkdev->blksize;
    idx += bp->free_byteidx;
    *addr = bp->addr2 + bp->addr_cnt + idx;

    bp->free_byteidx ++;
    if(bp->free_byteidx >= bp->blkdev->blksize)
    {
        bp->free_blkidx ++;
        bp->free_byteidx = 0;
    }
    listfs_bitmap_update(bp);
    return W_ERR_OK;
}


w_err_t listfs_bitmap_clear(lfs_bitmap_s *bp)
{
    w_int32_t i;
    w_uint8_t *blk;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
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


#endif

