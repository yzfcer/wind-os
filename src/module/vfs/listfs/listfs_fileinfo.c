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
#include "listfs_fileinfo.h"
#include "listfs.h"
#include "wind_debug.h"
#include "wind_string.h"
w_err_t listfs_read_block(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t **blk)
{
    w_int32_t cnt;
    w_uint8_t *tmpblk;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    *blk = W_NULL;
    tmpblk = lfs_malloc(blkdev->blksize);
    WIND_ASSERT_RETURN(tmpblk != W_NULL,W_ERR_MEM);
    cnt = wind_blkdev_read(blkdev,addr,tmpblk,1);
    WIND_ASSERT_TODO_RETURN(cnt > 0,lfs_free(tmpblk),W_ERR_FAIL);
    *blk = tmpblk;
    return W_ERR_OK;
}

w_err_t listfs_write_block(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk)
{
    w_int32_t cnt;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr > 0,W_ERR_INVALID);
    cnt = wind_blkdev_write(blkdev,addr,blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    return W_ERR_OK;
}


static w_err_t lfs_get_fsinfo_by_blk(lfs_info_s *fsinfo,w_blkdev_s *blkdev,w_addr_t addr)
{
    w_err_t err;
    w_uint8_t *blk;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsinfo != W_NULL,W_ERR_PTR_NULL);
    err = listfs_read_block(blkdev,addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    wind_memcpy(fsinfo,blk,sizeof(lfs_info_s));
    lfs_free(blk);
    WIND_ASSERT_RETURN(fsinfo->magic == LISTFS_MAGIC,W_ERR_INVALID);
    return W_ERR_OK;
}

w_err_t listfs_get_fsinfo(lfs_info_s *fsinfo,w_blkdev_s *blkdev)
{
    w_int32_t i;
    w_err_t err;
    for(i = 0;i < 2;i ++)
    {
        err = lfs_get_fsinfo_by_blk(fsinfo,blkdev,i);
        if(err == W_ERR_OK)
            return W_ERR_OK;
    }
    return W_ERR_FAIL;
}



w_err_t listfs_fileinfo_init(lfile_info_s *info,char *name,
    w_addr_t self_addr,w_addr_t parent_addr,w_addr_t prev_addr,w_uint8_t attr)
{
    w_int32_t namelen;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    namelen = wind_strlen(name);
    WIND_ASSERT_RETURN(namelen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(namelen < LFILE_NAME_LEN,W_ERR_INVALID);
    wind_memset(info,0,sizeof(lfile_info_s));
    info->magic = LISTFILE_MAGIC;
    wind_strcpy(info->name,name);
    info->parent_addr = parent_addr;
    info->self_addr = self_addr;
    info->prevfile_addr = prev_addr;
    info->attr = attr;
    return W_ERR_OK;
}


w_err_t listfs_get_fileinfo(lfile_info_s *info,lfile_blkinfo_s *blkinfo,w_blkdev_s *blkdev,w_addr_t addr)
{
    w_err_t err;
    w_uint8_t *blk;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkinfo != W_NULL,W_ERR_PTR_NULL);
    
    err = listfs_read_block(blkdev,addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    wind_memcpy(info,blk,sizeof(lfile_info_s));
    wind_memcpy(blkinfo,FILEINFO_BLKINFO(blk), sizeof(lfile_blkinfo_s));
    lfs_free(blk);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    return W_ERR_OK;
}

w_err_t listfs_set_fileinfo(lfile_info_s *info,lfile_blkinfo_s *blkinfo,w_blkdev_s *blkdev)
{
    w_uint32_t cnt;
    w_uint8_t *blk;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkinfo != W_NULL,W_ERR_PTR_NULL);
    blk = lfs_malloc(blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    wind_memset(blk,0,blkdev->blksize);
    wind_memcpy(blk,info,sizeof(lfile_info_s));
    wind_memcpy(FILEINFO_BLKINFO(blk),blkinfo,sizeof(lfile_blkinfo_s));
    cnt = wind_blkdev_write(blkdev,info->self_addr,blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    lfs_free(blk);
    return W_ERR_OK;
}

lfile_info_s *fileinfo_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk;
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_NULL);
    err = listfs_read_block(blkdev,info->parent_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    tmpinfo = (lfile_info_s*)blk;
    WIND_ASSERT_TODO_RETURN(tmpinfo->magic == LISTFILE_MAGIC,lfs_free(blk),W_NULL);
    wind_memcpy(info,blk,sizeof(lfile_info_s));
    lfs_free(blk);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_NULL);
    return info;
}

lfile_info_s *fileinfo_next(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk;
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_NULL);
    err = listfs_read_block(blkdev,info->nextfile_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    tmpinfo = (lfile_info_s*)blk;
    WIND_ASSERT_TODO_RETURN(tmpinfo->magic == LISTFILE_MAGIC,lfs_free(blk),W_NULL);
    wind_memcpy(info,blk,sizeof(lfile_info_s));
    lfs_free(blk);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_NULL);
    return info;
}

lfile_info_s *fileinfo_headchild(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk;
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_NULL);
    err = listfs_read_block(blkdev,info->headchild_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    tmpinfo = (lfile_info_s*)blk;
    WIND_ASSERT_TODO_RETURN(tmpinfo->magic == LISTFILE_MAGIC,lfs_free(blk),W_NULL);
    wind_memcpy(info,blk,sizeof(lfile_info_s));
    lfs_free(blk);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_NULL);
    return info;
}

lfile_info_s *fileinfo_tailchild(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk;
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_NULL);
    err = listfs_read_block(blkdev,info->tailchild_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    tmpinfo = (lfile_info_s*)blk;
    WIND_ASSERT_TODO_RETURN(tmpinfo->magic == LISTFILE_MAGIC,lfs_free(blk),W_NULL);
    wind_memcpy(info,blk,sizeof(lfile_info_s));
    lfs_free(blk);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_NULL);
    return info;
}


w_err_t fileinfo_update_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk;
    w_addr_t self_addr;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    if(info->parent_addr == 0)
        return W_ERR_OK;
    self_addr = info->self_addr;
    err = listfs_read_block(blkdev,info->parent_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    tmpinfo = (lfile_info_s*)blk;
    if(tmpinfo->headchild_addr == 0)
        tmpinfo->headchild_addr = self_addr;
    tmpinfo->tailchild_addr = self_addr;
    err = wind_blkdev_write(blkdev,tmpinfo->self_addr,blk,1);
    lfs_free(blk);
    return err;
}

w_err_t fileinfo_update_prev(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk;
    w_addr_t self_addr;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    if(info->prevfile_addr == 0)
        return W_ERR_OK;
    self_addr = info->self_addr;
    err = listfs_read_block(blkdev,info->prevfile_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    tmpinfo = (lfile_info_s*)blk;
    tmpinfo->tailchild_addr = self_addr;
    err = wind_blkdev_write(blkdev,tmpinfo->self_addr,blk,1);
    lfs_free(blk);
    return err;
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
    lfile_blkinfo_s *tmpinfo;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != 0,W_ERR_INVALID);
    err = listfs_read_block(blkdev,addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    tmpinfo = (lfile_blkinfo_s*)blk;
    WIND_ASSERT_TODO_RETURN(tmpinfo->magic == LISTFILE_BLK_MAGIC,lfs_free(blk),W_ERR_INVALID);
    wind_memcpy(info,tmpinfo,sizeof(lfile_blkinfo_s));
    lfs_free(blk);
    return W_ERR_OK;
}

w_err_t blkinfo_write(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    blk = lfs_malloc(blkdev->blksize);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
    wind_memset(blk,0,blkdev->blksize);
    wind_memcpy(blk,info,sizeof(lfile_blkinfo_s));
    err = listfs_write_block(blkdev,info->self_addr,blk);
    lfs_free(blk);
    return err;
}


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
    count = info->offset + info->blkused*info->blksize;
    return count;
}

w_int32_t blkinfo_tail_offset(lfile_blkinfo_s *info)
{
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    if(info->blkused <= 0)
        return info->offset;
    return (info->blkused-1)*info->blksize + info->byteused;
}

w_err_t blkinfo_get_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    return blkinfo_read(info,blkdev,info->prevblk_addr);
}

w_err_t blkinfo_get_next(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
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
        err = blkinfo_read(info,blkdev,info->nextblk_addr);
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

    tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_RETURN(tmpinfo != W_NULL,W_ERR_MEM);
    wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
    err = W_ERR_FAIL;
    while(info->offset > offset)
    {
        err = blkinfo_get_prev(tmpinfo,blkdev);
        if(err != W_ERR_OK)
        {
            lfs_free(tmpinfo);
            return W_ERR_FAIL;
        }
            
        if(BLKINFO_HAS_OFFSET(tmpinfo,offset,blkdev->blksize))
        {
            wind_memcpy(info,tmpinfo,sizeof(lfile_blkinfo_s));
            lfs_free(tmpinfo);
            return W_ERR_OK;
        }
    }
    err = W_ERR_OK;
    while(err == W_ERR_OK)
    {
        if(tmpinfo->nextblk_addr == 0)
        {
            wind_memcpy(info,tmpinfo,sizeof(lfile_blkinfo_s));
            lfs_free(tmpinfo);
            return W_ERR_OK;
        }
        err = blkinfo_get_next(tmpinfo,blkdev);
        if(err != W_ERR_OK)
        {
            lfs_free(tmpinfo);
            return W_ERR_FAIL;
        }
            
        if(BLKINFO_HAS_OFFSET(tmpinfo,offset,blkdev->blksize))
        {
            wind_memcpy(info,tmpinfo,sizeof(lfile_blkinfo_s));
            lfs_free(tmpinfo);
            return W_ERR_OK;
        }

    }
    lfs_free(tmpinfo);
    return W_ERR_FAIL;
}

w_err_t blkinfo_update_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_blkinfo_s *tmpinfo;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    err = listfs_read_block(blkdev,info->prevblk_addr,&blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    tmpinfo = (lfile_blkinfo_s*)blk;
    if(tmpinfo->magic == LISTFILE_MAGIC)
        tmpinfo = FILEINFO_BLKINFO(blk);
    WIND_ASSERT_TODO_RETURN(tmpinfo->magic == LISTFILE_BLK_MAGIC,lfs_free(blk),W_ERR_INVALID);
    tmpinfo->nextblk_addr = info->self_addr;
    err = listfs_write_block(blkdev,info->prevblk_addr,blk);
    lfs_free(blk);
    return W_ERR_OK;
}

w_err_t blkinfo_link(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t *addr,w_int32_t count)
{
    w_err_t err;
    w_int32_t i;
    lfile_blkinfo_s *info1 = W_NULL,*info2 = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    info1 = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_RETURN(info1 != W_NULL,W_ERR_MEM);
    info2 = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_TODO_RETURN(info2 != W_NULL,lfs_free(info1),W_ERR_MEM);
    wind_memcpy(info1,info,sizeof(lfile_blkinfo_s));
    for(i = 0;i < count;i ++)
    {
        blkinfo_init(info2,addr[i],info1->self_addr,
            info1->offset + blkdev->blksize * LFILE_LBLK_CNT,info1->blksize);
        err = blkinfo_write(info2,blkdev);
        info1->nextblk_addr = info2->self_addr;
        err = blkinfo_write(info1,blkdev);
        wind_memcpy(info1,info2,sizeof(lfile_blkinfo_s));
    }
    lfs_free(info1);
    lfs_free(info2);
    return W_ERR_OK;
}

w_err_t blkinfo_unlink(lfile_blkinfo_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(info->offset == 0,W_ERR_INVALID);
    
    info->nextblk_addr = 0;
    err = blkinfo_write(info,blkdev);
    return err;
}

w_err_t blkinfo_append_addr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t *addr,w_int32_t count)
{
    w_err_t err;
    w_int32_t i;
    w_int32_t rest;
    lfile_blkinfo_s *tmpinfo = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,W_ERR_INVALID);
    tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_RETURN(tmpinfo != W_NULL,W_ERR_MEM);
    do
    {
        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
        err = blkinfo_get_tail(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get tail blkinfo failed.");
        rest = count;
        while(rest > 0)
        {
            
        }
    }while(0);
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);

}

w_int32_t blkinfo_calc_restspace(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t tail_offset)
{
    w_int32_t err;
    w_int32_t space = 0;
    lfile_blkinfo_s *tmpinfo;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(info != W_NULL,-1);
    WIND_ASSERT_RETURN(blkdev != W_NULL,-1);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_BLK_MAGIC,-1);
    do
    {
        tmpinfo = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_RETURN(tmpinfo != W_NULL,W_ERR_MEM);
        
        wind_memcpy(tmpinfo,info,sizeof(lfile_blkinfo_s));
        err = blkinfo_get_tail(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get offset failed");
        space = blkinfo_get_space(tmpinfo);
        wind_error("error here");
        //if(space >= )
        
    }while(0);
    
    if(blk != W_NULL)
        lfs_free(blk);
    return W_ERR_OK;
}


