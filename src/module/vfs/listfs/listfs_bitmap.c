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

//����addr��������ݿ鿪ʼλ�õ�ƫ����
#define BLKADDR_TO_OFFSET(bp,addr) (addr - bp->addr2 - bp->addr_cnt)

//����ַ�±�ת���ɿ���ֽ��±�
#define ADDR_TO_BPIDX(blk_addridx,blksize,bp_addridx,bp_byteidx) \
    do{bp_addridx = blk_addridx / blksize;\
    bp_byteidx = blk_addridx % blksize;}while(0)

//������ֽ��±�ת���ɵ�ַ�±�
#define BPIDX_TO_ADDR(blk_addridx,blksize,bp_addridx,bp_byteidx) \
	do {blk_addridx = bp_addridx * blksize + bp_byteidx;}while(0)



static w_int32_t bp_get_free_byteidx(w_uint8_t *blk,w_int32_t blksize)
{
    w_int32_t i,j,cnt;
    w_uint32_t *ptr;
    ptr = (w_uint32_t *)blk;
    cnt = (blksize >> 2);
    for(i = 0;i < cnt;i ++)
    {
        if(ptr[i] != 0x01010101)
        {
            blk = (w_uint8_t *)&ptr[i];
            for(j = 0;j < 4;j ++)
                if(blk[i] == 0)
                    return (i << 4);
        }
    }
    return -1;
}

static w_int32_t bp_malloc_blk(w_uint8_t *blk,w_addr_t addridx,w_int32_t blksize,w_addr_t *addr,w_int32_t cnt)
{
    w_int32_t i,idx;
    w_int32_t alloc_cnt = 0;
    
    idx = bp_get_free_byteidx(blk,blksize);
    WIND_ASSERT_RETURN(idx >= 0,0);
    for(i = idx;i < blksize;i ++)
    {
        if(blk[i] == 0)
        {
            BPIDX_TO_ADDR(addr[alloc_cnt],blksize,addridx,i);
            blk[i] = BITMAP_USED;
            alloc_cnt ++;
            if(alloc_cnt >= cnt)
                break;
        }
    }
    return alloc_cnt;
}

static w_int32_t bp_free_blk(w_uint8_t *blk,w_addr_t addridx,w_int32_t blksize,w_addr_t *addr,w_int32_t cnt)
{
    w_int32_t i,free_cnt = 0;
    w_int32_t bp_addridx,bp_byteidx;
    for(i = 0;i < cnt;i ++)
    {
        ADDR_TO_BPIDX(addr[addridx],blksize,bp_addridx,bp_byteidx);
        if(bp_addridx == addridx)
        {
            addr[i] = 0;
            blk[bp_byteidx] = 0;
            free_cnt ++;
        }
    }
    return free_cnt;
}


static w_err_t bp_set_free_bitmap(lfs_bitmap_s *bp,w_addr_t blkidx,w_uint8_t *blk)
{
    w_int32_t byteidx;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_blkdev_read(bp->blkdev,blkidx,blk,1);
    byteidx = bp_get_free_byteidx(blk,bp->blkdev->blksize);
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
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_INVALID);
    blk = lfs_malloc(bp->blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    for(i = bp->free_blkidx;i < bp->addr_cnt;i ++)
    {
        err = bp_set_free_bitmap(bp,i,blk);
        if(err == W_ERR_OK)
        {
            lfs_free(blk);
            return err;
        }
    }
    for(i = 0;i < bp->free_blkidx;i ++)
    {
        err = bp_set_free_bitmap(bp,i,blk);
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
    w_int32_t byte_idx;
    w_uint8_t *blk = W_NULL;
    w_addr_t addr_idx,addr_start;
    
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bp->blkdev != W_NULL,W_ERR_PTR_NULL);
    addr_start = bp->addr2 + bp->addr_cnt;
    WIND_ASSERT_RETURN(addr >= addr_start,W_ERR_INVALID);
    addr_idx = (addr - addr_start) / bp->blkdev->blksize;
    byte_idx = addr % bp->blkdev->blksize;
    blk = lfs_malloc(bp->blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    
    wind_blkdev_read(bp->blkdev,bp->addr1+addr_idx,blk,1);
    blk[byte_idx] = bitflag;
    wind_blkdev_write(bp->blkdev,bp->addr1+addr_idx,blk,1);
    wind_blkdev_write(bp->blkdev,bp->addr2+addr_idx,blk,1);
    lfs_free(blk);
    return W_ERR_OK;
}



static w_err_t bp_read_free(lfs_bitmap_s *bp,w_uint8_t *blk)
{
    w_int32_t cnt;
    WIND_ASSERT_RETURN(bp != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    listfs_bitmap_update(bp);
    cnt = wind_blkdev_read(bp->blkdev,bp->addr1 + bp->free_blkidx,blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_HARDFAULT);
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
        listfs_bitmap_set(bp,addr[i],BITMAP_USED);
        
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


w_err_t listfs_bitmap_clear(lfs_bitmap_s *bp)
{
    w_int32_t i;
    w_uint8_t *blk = W_NULL;
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
    w_int32_t i,cnt;
    w_uint8_t *blk = W_NULL;
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
        cnt = wind_blkdev_read(blkdev,bp->addr1+1,blk,1);
        WIND_ASSERT_TODO_RETURN(cnt == 1,lfs_free(blk),0xffffffff);
        blkused += calc_blkused(blk,blkdev->blksize);
    }
    return blkused;    
}

#endif

