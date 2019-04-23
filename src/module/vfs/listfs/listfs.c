/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统主体功能
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
#include "wind_conv.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_crc32.h"
#include "listfs_bitmap.h"
#if WIND_FS_SUPPORT


#define NODE_TO_LISTFILE(node) (listfile_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((listfile_s*)0)->list.listnode)))
//static listfile_s *listfile_rootnode = W_NULL;

void *lfs_malloc(w_int32_t size)
{
    void *ptr = wind_malloc(size);
    return ptr;
}

w_err_t lfs_free(void *ptr)
{
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}


static lfile_info_s *lfs_search_child(lfile_info_s *parent,char *name,w_blkdev_s *blkdev)
{
    lfile_info_s *info;
    for(info = fileinfo_headchild(parent,blkdev);info != W_NULL;info = fileinfo_next(info,blkdev))
    {
        if(wind_strcmp(name,info->name) == 0)
            return info;
    }
    return W_NULL;
}

static w_err_t lfs_search_file(listfs_s *lfs,listfile_s *file,const char *path)
{
    w_err_t err = W_ERR_FAIL;
    w_int32_t len,cnt,i = 0;
    char **nameseg = W_NULL;
    char *pathname = W_NULL;
    lfile_info_s *finfo = W_NULL;
    lfile_blkinfo_s *blkinfo = W_NULL;
    
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_ERR_INVALID);
    do 
    {
        wind_debug("search node path:%s",path);
        //分配内存
        len = wind_strlen(path);
        nameseg = (char **)lfs_malloc(LISTFS_DIR_LAYCNT * sizeof(char*));
        pathname = lfs_malloc(len+1);
        finfo = lfs_malloc(sizeof(lfile_info_s));
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));

        WIND_ASSERT_BREAK((pathname != W_NULL && nameseg != W_NULL && 
            finfo != W_NULL && blkinfo != W_NULL),
            W_ERR_MEM,"alloc memory error");
        
        wind_memset(pathname,0,len+1);
        wind_strcpy(pathname,path);
        pathname[len] = 0;
        
        cnt = wind_strsplit(pathname,'/',nameseg,LISTFS_DIR_LAYCNT);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_INVALID,"split path failed");

        err = listfs_get_fileinfo(finfo,blkinfo,lfs->blkdev,lfs->lfs_info.root_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"read root directory failed.");

    
        if(cnt == 1)
        {
            err = W_ERR_OK;
            break;
        }

        err = W_ERR_OK;
        for(i = 1;i < cnt;i ++)
        {
            if(finfo->headchild_addr == 0)
            {
                err = W_ERR_FAIL;
                break;
            }
            finfo = lfs_search_child(finfo,nameseg[i],lfs->blkdev);
            WIND_ASSERT_BREAK(finfo != W_NULL,W_ERR_FAIL,"read directory failed.");
        }
    }while(0);
    if(err == W_ERR_OK)
    {
        wind_memcpy(&file->info,finfo,sizeof(lfs_info_s));
        file->blkinfo = blkinfo;
    }
        
    if(pathname)
        lfs_free(pathname);
    if(nameseg)
        lfs_free(nameseg);
    if(finfo)
        lfs_free(finfo);
    if(blkinfo)
        lfs_free(blkinfo);
    return err;
}

static w_err_t lfs_make_root(listfs_s *lfs)
{
    w_err_t err;
    w_uint8_t attr;
    lfile_info_s *info = W_NULL;
    lfile_blkinfo_s *blkinfo = W_NULL;
    do 
    {
        info = lfs_malloc(sizeof(lfile_info_s));
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        if(info == W_NULL || blkinfo == W_NULL)
        {
            wind_error("malloc failed.");
            err = W_ERR_MEM;
            break;
        }
        
        attr = (LFILE_ATTR_COMMAN | LFILE_ATTR_DIR);
        listfs_fileinfo_init(info,"root",lfs->lfs_info.root_addr,0,0,attr);
        blkinfo_init(blkinfo, lfs->lfs_info.root_addr,0,0,lfs->blkdev->blksize);
        err = listfs_set_fileinfo(info,blkinfo,lfs->blkdev,lfs->lfs_info.root_addr);
        if(err != W_ERR_OK)
        {
            wind_error("flush lfs root failed.");
            err = W_ERR_FAIL;
            break;
        }
        err = listfs_bitmap_set(&lfs->bitmap,lfs->lfs_info.root_addr,BITMAP_USED);
        if(err != W_ERR_OK)
        {
            wind_error("set lfs root bitmap failed.");
            err = W_ERR_FAIL;
            break;
        }
    }while(0);
    if(info != W_NULL)
        lfs_free(info);
    if(blkinfo != W_NULL)
        lfs_free(blkinfo);
    return err;
}


static w_err_t lfs_make_child(listfs_s *lfs,lfile_info_s *parent,char *name,w_uint8_t isdir)
{
    w_err_t err;
    w_uint8_t attr;
    w_uint8_t *blk = W_NULL;
    w_addr_t self_addr,cnt;
    lfile_info_s *info;
    lfile_blkinfo_s *blkinfo;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    
    err = listfs_bitmap_find_free(&lfs->bitmap,&self_addr);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    do 
    {
        err = W_ERR_OK;
        blk = lfs_malloc(lfs->blkdev->blksize);
        WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
        wind_memset(blk,0,lfs->blkdev->blksize);
        attr = isdir?(LFILE_ATTR_COMMAN|LFILE_ATTR_DIR):LFILE_ATTR_COMMAN;
        info = (lfile_info_s*)blk;
        listfs_fileinfo_init(info,name,self_addr,parent->self_addr,parent->tailchild_addr,attr);
        blkinfo = FILEINFO_BLKINFO(blk);
        blkinfo_init(blkinfo, self_addr,0,0,lfs->blkdev->blksize);
        
        info->prevfile_addr = parent->tailchild_addr;
        cnt = wind_blkdev_write(lfs->blkdev,self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_FAIL,"write file info failed.");
        
        err = fileinfo_update_prev(info,lfs->blkdev);
        WIND_ASSERT_BREAK(cnt > 0,err,"update prev file info failed.");

        err = fileinfo_update_parent(info,lfs->blkdev);
        WIND_ASSERT_BREAK(cnt > 0,err,"update parent file info failed.");
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    if(err != W_ERR_OK)
        wind_error("fs should NOT be error here,must restore the file system");
    return err;
}


static w_err_t lfs_make_file(listfs_s *lfs,listfile_s *file,char *path)
{
    w_err_t err;
    w_int32_t i,pathlen,cnt;
    w_uint8_t isdir = 0;
    char **nameseg = W_NULL;
    lfile_info_s *finfo = W_NULL,*tmpinfo;
    lfile_blkinfo_s *blkinfo = W_NULL;
    char *pathname = W_NULL;

    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_ERR_INVALID);
    
    do
    {
        err = W_ERR_OK;
        //分配内存
        pathlen = wind_strlen(path);
        pathname = lfs_malloc(pathlen+1);
        nameseg = (char **)lfs_malloc(LISTFS_DIR_LAYCNT * sizeof(char*));
        finfo = lfs_malloc(sizeof(lfile_info_s));
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        if(pathname == W_NULL || nameseg == W_NULL || finfo == W_NULL || blkinfo == W_NULL)
        {
            wind_error("alloc memory error");
            err = W_ERR_MEM;
            break;
        }

        //拷贝参数
        wind_strcpy(pathname,path);
        if(pathname[pathlen - 1] == '/')
        {
            pathname[pathlen - 1] = 0;
            isdir = 1;
        }
        cnt = wind_strsplit(pathname,'/',nameseg,LISTFS_DIR_LAYCNT);
        err = listfs_get_fileinfo(finfo,blkinfo,lfs->blkdev,lfs->lfs_info.root_addr);
        if(err != W_ERR_OK)
        {
            wind_debug("get root info failed");
            break;
        }
        if(cnt <= 1)
        {
            err = W_ERR_OK;
            break;
        }

        err = W_ERR_OK;
        for(i = 1;i < cnt;i ++)
        {
            tmpinfo = lfs_search_child(finfo,nameseg[i],lfs->blkdev);
            if(tmpinfo != W_NULL)
            {
                wind_memcpy(finfo,tmpinfo,sizeof(lfile_info_s));
                lfs_free(tmpinfo);
            }
            else
            {
                err = lfs_make_child(lfs,finfo,nameseg[i],i == cnt - 1?isdir:1);
                if(err != W_ERR_OK)
                {
                    wind_error("make child failed,%d",err);
                    break;
                }
            }
        }
        if(err != W_ERR_OK)
            break;
    }while(0);
    if(err == W_ERR_OK)
    {
        wind_memcpy(&file->info,finfo,sizeof(lfile_info_s));
    }
        
    if(pathname != W_NULL)
        lfs_free(pathname);
    if(finfo != W_NULL)
        lfs_free(finfo);
    if(nameseg != W_NULL)
        lfs_free(nameseg);
    return err;
}

w_err_t listfile_remove(listfile_s *file)
{
    return W_ERR_FAIL;
}

w_err_t listfs_format(listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint32_t crc;
    w_int32_t cnt;
    lfs_info_s *lfs_info;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        lfs->blkdev = blkdev;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
        wind_memset(blk,0,blkdev->blksize);
        lfs_info = &lfs->lfs_info;
        lfs_info->magic = LISTFS_MAGIC;
        lfs_info->blkcount = blkdev->blkcnt;
        lfs_info->unit_size = (w_uint16_t)blkdev->blksize;
        lfs_info->blksize = (w_uint16_t)blkdev->blksize;
        lfs_info->reserve_blk = 5;
        lfs_info->attr = 0;
        lfs_info->bitmap1_addr = lfs_info->reserve_blk + 1;
        lfs_info->bitmap_cnt = (blkdev->blkcnt - lfs_info->bitmap1_addr) / (lfs_info->unit_size + 1) + 1;
        lfs_info->bitmap2_addr = lfs_info->bitmap1_addr + lfs_info->bitmap_cnt;
        lfs_info->root_addr = lfs_info->bitmap2_addr + lfs_info->bitmap_cnt;
        wind_memset(blk,0,blkdev->blksize);
        wind_memcpy(blk,lfs_info,sizeof(lfs_info_s));
        crc = wind_crc32(blk,blkdev->blksize-4,0xffffffff);
        wind_from_uint32(&blk[blkdev->blksize-4],crc);
        
        cnt = wind_blkdev_write(blkdev,0,blk,1);
        if(cnt <= 0)
        {
            wind_error("write fsinfo failed.");
            err = W_ERR_FAIL;
            break;
        }

        cnt = wind_blkdev_write(blkdev,1,blk,1);
        if(cnt <= 0)
        {
            wind_error("write bakeup fsinfo failed.");
            err = W_ERR_FAIL;
            break;
        }

        listfs_bitmap_init(&lfs->bitmap,lfs_info->bitmap1_addr,lfs_info->bitmap_cnt,blkdev);
        listfs_bitmap_clear(&lfs->bitmap);
        lfs_make_root(lfs);
        listfs_bitmap_update(&lfs->bitmap);
        lfs->file_ref = 0;
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}


w_err_t listfs_mount(listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    err = listfs_get_fsinfo(&lfs->lfs_info,blkdev);
    if(err != W_ERR_OK)
    {
        wind_notice("No file system detected,format dev %s now.",
            wind_obj_name(&blkdev->obj));
        err = listfs_format(lfs,blkdev);
    }
    else
    {
        lfs->blkdev = blkdev;
        lfs->file_ref = 0;
        listfs_bitmap_init(&lfs->bitmap,lfs->lfs_info.bitmap1_addr,
            lfs->lfs_info.bitmap_cnt,lfs->blkdev);
        err = listfs_bitmap_update(&lfs->bitmap);
        
    }
    return err;
}

w_err_t listfs_unmount(listfs_s *lfs)
{
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(lfs->lfs_info.magic == LISTFS_MAGIC,W_ERR_INVALID);
    lfs->blkdev = W_NULL;
    lfs->lfs_info.magic = (~LISTFS_MAGIC);
    return W_ERR_OK;
}



listfile_s* listfile_open(listfs_s *lfs,const char *path,w_uint16_t mode)
{
    w_err_t err;
    w_bool_t is_crt;
    listfile_s *file = W_NULL;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_NULL);
    do 
    {
        file = lfs_malloc(sizeof(listfile_s));
        WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
        wind_memset(file,0,sizeof(listfile_s));
        is_crt = (mode & LFMODE_CRT)?W_TRUE:W_FALSE;
        err = lfs_search_file(lfs,file,path);
        if((err != W_ERR_OK) && (!is_crt))
        {   //没有创建标记，且文件不存在
            err = W_ERR_FAIL;
            break;
        }
        else if((err == W_ERR_OK) && (!is_crt))
        {   //没有创建标记，且文件存在
            err = W_ERR_OK;
            break;
        }

        //有创建标记，且文件不存在
        err = lfs_make_file(lfs,file,(char*)path);
        if(err != W_ERR_OK)
        {
            wind_debug("make file %s failed",path);
            break;
        }
    }while(0);
    if((err != W_ERR_OK)&&(file != W_NULL))
    {   //打开文件过程中出错
        lfs_free(file);
        file = W_NULL;
    }
    if(file != W_NULL)
    {
        file->lfs = lfs;
        file->lfs->file_ref ++;
        file->mode = (w_uint8_t)mode;
        //file->blkinfo = &file->info.blkinfo;
        
        if(mode & LFMODE_A)
            file->offset = file->filelen;
        else
            file->offset = 0;
    }
    return file;
}



w_err_t listfile_close(listfile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->lfs->file_ref > 0,W_ERR_INVALID);
    file->info.magic = 0;
    file->lfs->file_ref --;
    if(file->blkinfo->offset != 0)
    {
        lfs_free(file->blkinfo);
        file->blkinfo = W_NULL;
    }
    lfs_free(file);
    return W_ERR_OK;
}

w_err_t listfile_set_attr(listfile_s* file,w_uint8_t attr)
{
    w_err_t err;
    w_uint8_t tmpattr = 0;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    tmpattr = (file->info.attr & LFILE_ATTR_DIR);
    (attr & LFILE_ATTR_READ)?(tmpattr |= LFILE_ATTR_READ):(tmpattr &= ~LFILE_ATTR_READ);
    (attr & LFILE_ATTR_WRITE)?(tmpattr |= LFILE_ATTR_WRITE):(tmpattr &= ~LFILE_ATTR_WRITE);
    (attr & LFILE_ATTR_HIDE)?(tmpattr |= LFILE_ATTR_HIDE):(tmpattr &= ~LFILE_ATTR_HIDE);
    (attr & LFILE_ATTR_VERIFY)?(tmpattr |= LFILE_ATTR_VERIFY):(tmpattr &= ~LFILE_ATTR_VERIFY);
    file->info.attr = tmpattr;
	err = listfs_set_fileinfo(&file->info,file->blkinfo,file->lfs->blkdev,file->blkinfo->self_addr);
    return err;
}

w_err_t listfile_get_attr(listfile_s* file,w_uint8_t *attr)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    *attr = file->info.attr;
    return W_ERR_OK;
}


w_bool_t listfile_existing(listfs_s *lfs,const char *path)
{
    listfile_s *file;
    file = listfile_open(lfs,path,LFMODE_R);
    if(file == W_NULL)
        return W_FALSE;
    listfile_close(file);
    return W_TRUE;
}

w_err_t listfile_seek(listfile_s* file,w_int32_t offset)
{
    w_err_t err;
    lfile_blkinfo_s *blkinfo = W_NULL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    if(file->info.filesize < offset)
        offset = file->info.filesize;
    blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_RETURN(blkinfo != W_NULL, W_ERR_MEM);
    do 
    {
        wind_memcpy(blkinfo,file->blkinfo,sizeof(lfile_blkinfo_s));
        err = blkinfo_get_byoffset(blkinfo,file->lfs->blkdev,offset);
        if(err != W_ERR_OK)
            break;
        file->offset = offset;
    }while(0);
    if(blkinfo != W_NULL)
        lfs_free(blkinfo);
    return err;
}

w_int32_t listfile_ftell(listfile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    return file->offset;
}

w_int32_t listfile_read(listfile_s* file,w_uint8_t *buff, w_int32_t size)
{
    w_err_t err;
    w_int32_t rsize;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    WIND_ASSERT_RETURN(file->mode & LFMODE_R,-1);
    WIND_ASSERT_RETURN(file->offset < file->info.filesize,-1);
    rsize = size;
    if(file->offset + rsize > file->info.filesize)
        rsize = file->info.filesize - file->offset;
    
        
    return rsize;
}


w_int32_t listfile_write(listfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    w_err_t err;
    w_int32_t wsize,allocsize;
    lfile_blkinfo_s *blkinfo;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    WIND_ASSERT_RETURN((file->mode & LFMODE_W)||(file->mode & LFMODE_A),-1);
    wsize = size;
    if(file->offset + wsize > file->info.filesize)
    {
        allocsize = file->offset + wsize - file->info.filesize;
        allocsize = (allocsize + file->lfs->blkdev->blksize - 1) / file->lfs->blkdev->blksize;
    }

    if(file->info.filesize < file->offset + wsize)
        file->info.filesize += wsize;
    file->offset += wsize;
    return wsize;
}

listfile_s *listfile_readdir(listfile_s* file,w_int32_t index)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(file->mode & LFMODE_R,W_NULL);
    return W_NULL;
}


w_err_t listfile_fgets(listfile_s* file,char *buff, w_int32_t maxlen)
{
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    WIND_ASSERT_RETURN(file->mode & LFMODE_R,-1);
    return W_ERR_FAIL;
}

w_err_t listfile_fputs(listfile_s* file,char *buff)
{
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    WIND_ASSERT_RETURN(file->mode & LFMODE_W,-1);
    return W_ERR_FAIL;
}

#endif

