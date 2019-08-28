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
//#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_MODULE_VFS_SUPPORT

static w_int32_t bm_get_free_byteidx(w_uint8_t *blk,w_int32_t offset,w_int32_t blksize)
{
    w_int32_t i;
    for(i = offset;i < blksize;i ++)
    {
        if(blk[i] == 0)
            return i;
    }
    return -1;
}

static w_int32_t bm_alloc_blk(lfs_bitmap_s *bm,w_addr_t *addr,w_int32_t cnt)
{
    w_int32_t i;
    w_int32_t alloc_cnt = 0;
    
    for(i = bm->cur_byteidx;i < bm->blkdev->blksize;i ++)
    {
        if(bm->blk[i] == 0)
        {
            addr[alloc_cnt] = bm->data_addr + (bm->cur_blkidx*bm->blkdev->blksize) + bm->cur_byteidx;
            wind_notice("bitmap:alloc addr 0x%08x",addr[alloc_cnt]);
            bm->blk[i] = BITMAP_USED;
            bm->cur_byteidx ++;
            alloc_cnt ++;
            if(alloc_cnt >= cnt)
                break;
        }
    }
    return alloc_cnt;
}

static w_int32_t bm_free_blk(lfs_bitmap_s *bm,w_addr_t *addr,w_int32_t cnt)
{
    w_int32_t i,free_cnt = 0;
    w_int32_t byteidx;
    
    for(i = 0;i < cnt;i ++)
    {
        byteidx = addr[i] - bm->data_addr - (bm->cur_blkidx*bm->blkdev->blksize);
        if((byteidx >= 0)&&(byteidx < bm->blkdev->blksize))
        {
            wind_notice("bitmap:free addr 0x%08x",addr[i]);
            addr[i] = 0;
            if(bm->blk[byteidx] == 0)
                wind_error("bitmap free error");
            bm->blk[byteidx] = 0;
            free_cnt ++;
        }
    }
    return free_cnt;
}

static w_err_t bm_read_blk(lfs_bitmap_s *bm)
{
    w_int32_t cnt;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->cur_blkidx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->cur_blkidx < bm->bmblk_cnt,W_ERR_INVALID);
    
    cnt = wind_blkdev_read(bm->blkdev,bm->addr1 + bm->cur_blkidx,bm->blk,1);
    if(cnt != 1)
        cnt = wind_blkdev_read(bm->blkdev,bm->addr2 + bm->cur_blkidx,bm->blk,1);
    WIND_ASSERT_RETURN(cnt == 1,W_ERR_HARDFAULT);
    return W_ERR_OK;
}

static w_err_t bm_write_blk(lfs_bitmap_s *bm)
{
    w_int32_t cnt;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->cur_blkidx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->cur_blkidx < bm->bmblk_cnt,W_ERR_INVALID);
    
    cnt = wind_blkdev_write(bm->blkdev,bm->addr1 + bm->cur_blkidx,bm->blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_HARDFAULT);
    cnt = wind_blkdev_write(bm->blkdev,bm->addr2 + bm->cur_blkidx,bm->blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_HARDFAULT);
    return W_ERR_OK;
}


static w_err_t bm_check_free_bitmap(lfs_bitmap_s *bm)
{
    w_int32_t byteidx;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->blkdev != W_NULL,W_ERR_PTR_NULL);
    byteidx = bm_get_free_byteidx(bm->blk,bm->cur_byteidx,bm->blkdev->blksize);
    if(byteidx >= 0)
    {
        bm->cur_byteidx = byteidx;
        wind_notice("set free bitmap:%d,%d",bm->cur_blkidx,bm->cur_byteidx);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

static w_err_t bm_update_freeidx(lfs_bitmap_s *bm)
{
    w_uint32_t i;
    w_err_t err;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->blkdev != W_NULL,W_ERR_INVALID);
    wind_notice("bm_update_freeidx");
    for(i = 0;i < bm->bmblk_cnt;i ++)
    {
        err = bm_check_free_bitmap(bm);
        if(err == W_ERR_OK)
            return err;
        bm->cur_blkidx ++;
        if(bm->cur_blkidx >= bm->bmblk_cnt)
            bm->cur_blkidx = 0;
        err = bm_read_blk(bm);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_HARDFAULT,"read blk failed");
    }

    bm->cur_blkidx = 0;
    bm->cur_byteidx = 0;
    wind_notice("set free bitmap failed");
    return W_ERR_FAIL;
}



w_err_t listfs_bitmap_init(lfs_bitmap_s *bm,w_addr_t start_addr,w_int32_t blk_cnt,w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_trace("listfs_bitmap_init,blkdev:%s",wind_obj_name(&blkdev->obj));
    do
    {
        err = W_ERR_OK;
        bm->magic = LISTFS_BITMAP_MAGIC;
        bm->addr1 = start_addr;
        bm->addr2 = start_addr + blk_cnt;
        bm->bmblk_cnt = blk_cnt;
        bm->data_addr = bm->addr2 + blk_cnt;
        bm->cur_blkidx = 0;
        bm->cur_byteidx = 0;
        bm->blkdev = blkdev;
        bm->blk = listfs_mem_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(bm->blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        err = bm_read_blk(bm);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blk failed");
        err = bm_update_freeidx(bm);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"update listfs bitmap failed");
    }while(0);
    
    if(err != W_ERR_OK)
    {
        bm->magic = 0;
        if(bm->blk != W_NULL)
            listfs_mem_free(bm->blk);
    }
    return err;
}

w_err_t listfs_bitmap_deinit(lfs_bitmap_s *bm)
{
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    
    wind_trace("listfs_bitmap_deinit");
    bm->magic = 0;
    bm->bmblk_cnt = 0;
    if(bm->blk != W_NULL)
    {
        listfs_mem_free(bm->blk);
        bm->blk = W_NULL;
    }
    return W_ERR_OK;
}






w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bm,w_addr_t *addr,w_int32_t addr_cnt)
{
    w_int32_t i,cnt,alloc_cnt;
    w_err_t err;
    wind_trace("listfs_bitmap_alloc_blk");
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr_cnt > 0,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(bm->blkdev != W_NULL,W_ERR_PTR_NULL);
    if((bm->cur_blkidx == 0) && (bm->cur_byteidx == 0))
        bm_update_freeidx(bm);
    wind_memset(addr,0,sizeof(w_addr_t)*addr_cnt);
    alloc_cnt = 0;
    err = W_ERR_OK;
    //�˴�Ӧ���Ż������Լ�¼��ǰ�Ѿ���������ݿ飬�����Ϳ�����ǰ�����Ƿ����㹻�����ݿ鹩����
    for(i = 0;i < bm->bmblk_cnt;i ++)
    {
        err = bm_read_blk(bm);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read bitmap failed");
        cnt = bm_alloc_blk(bm,&addr[alloc_cnt],addr_cnt - alloc_cnt);
        if(cnt > 0)
        {
            bm_write_blk(bm);
            alloc_cnt += cnt;
            if(alloc_cnt >= addr_cnt)
                break;
        }
        bm->cur_blkidx ++;
        if(bm->cur_blkidx >= bm->bmblk_cnt)
            bm->cur_blkidx = 0;
        bm->cur_byteidx = 0;
    }
    
    if((err != W_ERR_OK) || (alloc_cnt < addr_cnt))
    {
        listfs_bitmap_free_blk(bm,addr,addr_cnt);
        return W_ERR_FAIL;
    }
    return W_ERR_OK;
}

w_err_t listfs_bitmap_free_blk(lfs_bitmap_s *bm,w_addr_t *addr,w_int32_t addr_cnt)
{
    w_int32_t i,cnt,free_cnt;
    w_err_t err;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr_cnt > 0,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(bm->blkdev != W_NULL,W_ERR_PTR_NULL);
    
    wind_trace("listfs_bitmap_free_blk");
    err = W_ERR_OK;
    free_cnt = 0;
    for(i = 0;i < addr_cnt;i ++)
    {
        if(addr[i] != 0)
        {
            err = bm_read_blk(bm);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read bitmap failed");
            cnt = bm_free_blk(bm,&addr[i],addr_cnt - i);
            if(cnt > 0)
            {
                free_cnt += cnt;
                bm_write_blk(bm);
                if(free_cnt >= addr_cnt)
                    break;
            }
        }
    }
    return err;
}


w_err_t listfs_bitmap_clear(lfs_bitmap_s *bm)
{
    w_int32_t i;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_trace("listfs_bitmap_clear");
    blkdev = bm->blkdev;
    wind_memset(bm->blk,0,blkdev->blksize);
    for(i = 1;i < bm->bmblk_cnt; i ++)
        wind_blkdev_write(blkdev,bm->addr1+i,bm->blk,1);
    for(i = 1;i < bm->bmblk_cnt; i ++)
        wind_blkdev_write(blkdev,bm->addr2+i,bm->blk,1);
    bm->cur_blkidx = 0;
    bm->cur_byteidx = 0;
    return W_ERR_OK;
}

static w_uint32_t bm_calc_blkused(w_uint8_t *blk,w_int32_t blksize)
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

w_int32_t listfs_bitmap_calc_usedblk(lfs_bitmap_s *bm)
{
    w_int32_t i,cnt;
    w_uint8_t *blk = W_NULL;
    w_blkdev_s *blkdev;
    w_uint32_t blkused = 0;
    WIND_ASSERT_RETURN(bm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(bm->magic == LISTFS_BITMAP_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(bm->blkdev != W_NULL,W_ERR_PTR_NULL);
    wind_trace("listfs_bitmap_calc_usedblk");
    blkdev = bm->blkdev;
    blk = listfs_mem_malloc(blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    for(i = 1;i < bm->bmblk_cnt; i ++)
    {
        cnt = wind_blkdev_read(blkdev,bm->addr1+1,blk,1);
        WIND_ASSERT_TODO_RETURN(cnt == 1,listfs_mem_free(blk),0xffffffff);
        blkused += bm_calc_blkused(blk,blkdev->blksize);
    }
    if(blk != W_NULL)
        listfs_mem_free(blk);
    return blkused;    
}

#endif

