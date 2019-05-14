/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_fileinfo.c
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
#include "listfs_blkinfo.h"
#include "listfs_fileinfo.h"
#include "listfs.h"
#include "wind_debug.h"
#include "wind_string.h"

w_int32_t blkinfo_get_used(lfile_blkinfo_s *info)
{
    w_int32_t count;
    WIND_ASSERT_RETURN(info != W_NULL,-1);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,-1);
    count = info->offset + info->byteused;
    if(info->blkused > 1)
        count += ((info->blkused - 1)*info->blksize);
    return count;
}

w_int32_t blkinfo_get_space(lfile_blkinfo_s *info)
{
    w_int32_t count;
    WIND_ASSERT_RETURN(info != W_NULL,-1);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,-1);
    count = info->offset + info->blkused * info->blksize;
    return count;
}

w_err_t blkinfo_init(lfile_blkinfo_s *info,w_addr_t self_addr,w_addr_t prev_addr,w_int32_t offset,w_int32_t blksize)
{
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(self_addr != 0,W_ERR_INVALID);
    wind_memset(info,0,sizeof(lfile_blkinfo_s));
    info->magic = LISTFILE_BLK_MAGIC;
    info->self_addr = self_addr;
    info->offset = offset;
    info->blksize = blksize;
    return W_ERR_OK;
}

w_err_t blkinfo_read(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t addr)
{
    w_err_t err;
    w_int32_t cnt;
    lfile_blkinfo_s *tmpinfo;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != 0,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
        cnt = wind_blkdev_read(blkdev,addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_FAIL,"read blk failed");
        tmpinfo = (lfile_blkinfo_s*)blk;
        if(tmpinfo->magic == LISTFILE_MAGIC)
            tmpinfo = FILEINFO_BLKINFO(blk);
        WIND_ASSERT_BREAK(tmpinfo->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID,"invalid blkinfo");
        wind_memcpy(info,tmpinfo,sizeof(lfile_blkinfo_s));
    
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}

w_err_t blkinfo_write(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t cnt;
    lfile_blkinfo_s *tmpinfo;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);

    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        tmpinfo = (lfile_blkinfo_s*)blk;
        if(info->offset == 0)
        {
            cnt = wind_blkdev_read(blkdev,info->self_addr,blk,1);
            WIND_ASSERT_BREAK(cnt == 1,err,"read blkinfo failed");
            if(tmpinfo->magic == LISTFILE_MAGIC)
                tmpinfo = FILEINFO_BLKINFO(blk);          
            WIND_ASSERT_BREAK(tmpinfo->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID,"invalid blkinfo");
        }

        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
        cnt = wind_blkdev_write(blkdev,info->self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt == 1,err,"write blkinfo failed");
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}

w_err_t blkinfo_get_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(info->prevblk_addr != 0,W_ERR_FAIL);
    return blkinfo_read(info,blkdev,info->prevblk_addr);
}

w_err_t blkinfo_get_next(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(info->nextblk_addr != 0,W_ERR_FAIL);
    return blkinfo_read(info,blkdev,info->nextblk_addr);
}

w_err_t blkinfo_get_tail(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    if(info->nextblk_addr == 0)
        return W_ERR_OK;
    err = W_ERR_OK;
    while(1)
    {
        if(info->nextblk_addr == 0)
            break;
        err = blkinfo_get_next(info,blkdev);
        if(err != W_ERR_OK)
            break;
    }
    return err;
}


w_err_t blkinfo_get_byoffset(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t offset)
{
    w_err_t err;
    lfile_blkinfo_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    if((offset >= info->offset)&&(offset < info->offset + info->blkused * blkdev->blksize))
        return W_ERR_OK;
    do
    {
        err = W_ERR_OK;
        tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));

        while(info->offset > offset)
        {
            err = blkinfo_get_prev(tmpinfo,blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get prev blkinfo failed");
            if(BLKINFO_HAS_OFFSET(tmpinfo,offset,blkdev->blksize))
                break;
            err = W_ERR_FAIL;
        }
        if(err == W_ERR_OK)
            break;
        
        while(info->offset <= offset)
        {
            err = blkinfo_get_next(tmpinfo,blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get prev blkinfo failed");
            if(BLKINFO_HAS_OFFSET(tmpinfo,offset,blkdev->blksize))
                break;
            err = W_ERR_FAIL;
        }
        if(err == W_ERR_OK)
            break;
    }while(0);
    if(err == W_ERR_OK)
        wind_memcpy(info,tmpinfo,sizeof(lfile_blkinfo_s));
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);
    return err;
}

w_err_t blkinfo_update_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_blkinfo_s *tmpinfo = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
        err = blkinfo_read(tmpinfo,blkdev,tmpinfo->prevblk_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"read blkinfo failed");
        tmpinfo->nextblk_addr = info->self_addr;
        err = blkinfo_write(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write blkinfo failed");
    }while(0);
    
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);
    return err;
}

w_err_t blkinfo_link(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t *addr,w_int32_t count)
{
    w_err_t err;
    w_int32_t i;
    lfile_blkinfo_s *info1 = W_NULL,*info2 = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        info1 = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(info1 != W_NULL,W_ERR_MEM,"malloc info1 failed");
        info2 = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(info2 != W_NULL,W_ERR_MEM,"malloc info2 failed");
        wind_memcpy(info1,info,sizeof(lfile_blkinfo_s));
        for(i = 0;i < count;i ++)
        {
            blkinfo_init(info2,addr[i],info1->self_addr,
                info1->offset + blkdev->blksize * LFILE_LBLK_CNT,info1->blksize);
            err = blkinfo_write(info2,blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write blkinfo2 failed");
            info1->nextblk_addr = info2->self_addr;
            err = blkinfo_write(info1,blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write blkinfo1 failed");
            wind_memcpy(info1,info2,sizeof(lfile_blkinfo_s));
        }      
    }while(0);
    if(info1 != W_NULL)
        lfs_free(info1);
    if(info2 != W_NULL)
        lfs_free(info2);
    return W_ERR_OK;
}

//截断info及后面所有的blkinfo
w_err_t blkinfo_unlink(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_blkinfo_s *tmpinfo = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(info->offset == 0,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc blkinfo failed");
        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
        if(tmpinfo->prevblk_addr != 0)
        {
            err = blkinfo_get_prev(tmpinfo,blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkinfo failed");
            tmpinfo->nextblk_addr = 0;
            err = blkinfo_write(info,blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkinfo failed");
        }
    }while(0);
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);
    return err;
}

w_err_t blkinfo_add_dataaddr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t *addr,w_int32_t count)
{
    w_err_t err;
    w_int32_t i;
    w_int32_t index;
    lfile_blkinfo_s *tmpinfo = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc blkinfo failed");
        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
        err = blkinfo_get_tail(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get tail blkinfo failed.");
        index = 0;
        while(index < count)
        {
            if(tmpinfo->blkused < LFILE_LBLK_CNT)
            {
                for(i = tmpinfo->blkused;i < LFILE_LBLK_CNT;i ++)
                {
                    tmpinfo->dataaddr[i] = addr[index++];
                    if(index >= count)
                        break;
                }
                err = blkinfo_write(tmpinfo,blkdev);
                if(index >= count)
                    break;
                err = blkinfo_get_next(tmpinfo,blkdev);
                WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get next blkinfo failed");
            }
        }
    }while(0);
    
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);
    return err;
}

w_err_t blkinfo_del_dataaddr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t addridx,w_int32_t count)
{
    
    return W_ERR_FAIL;
}

w_addr_t blkinfo_get_addr(lfile_blkinfo_s *info,w_int32_t offset)
{
    w_int32_t index;
    WIND_ASSERT_RETURN(info != W_NULL,0);
    WIND_ASSERT_RETURN(offset >= 0, 0);
    index = (offset - info->offset + info->blksize - 1) / info->blksize;
    WIND_ASSERT_RETURN(index >= 0, 0);
    WIND_ASSERT_RETURN(index < LFILE_LBLK_CNT, 0);
    return info->dataaddr[index];
}



