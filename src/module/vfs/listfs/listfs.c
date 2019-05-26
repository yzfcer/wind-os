/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: listfs文件系统主体功能
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
    //void *ptr = wind_malloc(size);
    void *ptr = wind_falloc(size,253);
    return ptr;
}

w_err_t lfs_free(void *ptr)
{
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}


static w_err_t lfs_search_child(lfile_info_s *info,char *name,w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    err = fileinfo_get_headchild(info,blkdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    for(;info != W_NULL;)
    {
        if(wind_strcmp(name,info->name) == 0)
            return W_ERR_OK;
        err = fileinfo_get_next(info,blkdev);
        if(err != W_ERR_OK)
            return err;
    }
    return W_ERR_FAIL;
}

static w_err_t lfs_search_file(listfs_s *lfs,listfile_s *file,const char *path)
{
    w_err_t err;
    w_uint8_t isdir = 0;
    w_int32_t len,segcnt,i = 0;
    char **nameseg = W_NULL;
    char *tmppath = W_NULL;
    lfile_info_s *finfo = W_NULL;
    lfile_blkinfo_s *blkinfo = W_NULL;
    
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_ERR_INVALID);
    do 
    {
        err = W_ERR_OK;
        wind_debug("search node path:%s",path);
        //分配内存
        nameseg = (char **)lfs_malloc(LISTFS_DIR_LAYCNT * sizeof(char*));
        WIND_ASSERT_BREAK(nameseg,W_ERR_MEM,"malloc nameseg failed");
        len = wind_strlen(path);
        tmppath = lfs_malloc(len+1);
        WIND_ASSERT_BREAK(tmppath,W_ERR_MEM,"malloc tmppath failed");
        finfo = lfs_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(finfo,W_ERR_MEM,"malloc finfo failed");
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(blkinfo,W_ERR_MEM,"malloc blkinfo failed");

        //拷贝和分割文件路径
        wind_memset(tmppath,0,len+1);
        wind_strcpy(tmppath,path);
        if(tmppath[len-1] == '/')
        {
            isdir = 1;
            tmppath[len-1] = 0;
        }
        segcnt = wind_strsplit(tmppath,'/',nameseg,LISTFS_DIR_LAYCNT);
        WIND_ASSERT_BREAK(segcnt > 0,W_ERR_INVALID,"split path failed");
        //拷贝根目录
        wind_memcpy(finfo,&lfs->root,sizeof(lfs_info_s));
        if(segcnt == 1)
        {
            err = W_ERR_OK;
            break;
        }

        err = W_ERR_OK;
        for(i = 1;i < segcnt;i ++)
        {
            if(finfo->headchild_addr == 0)
            {
                err = W_ERR_FAIL;
                break;
            }
            err = lfs_search_child(finfo,nameseg[i],lfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read directory failed.");
        }
    }while(0);
    if(err == W_ERR_OK)
    {
        wind_memcpy(&file->info,finfo,sizeof(lfs_info_s));
        file->blkinfo = blkinfo;
    }
        
    if(tmppath)
        lfs_free(tmppath);
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
    lfile_info_s *info;
    w_uint8_t *blk = W_NULL;
    lfile_blkinfo_s *blkinfo = W_NULL;
    wind_notice("lfs_make_root");
    do 
    {
        err = W_ERR_OK;
        blk = lfs_malloc(lfs->blkdev->blksize);
        WIND_ASSERT_BREAK(blk == W_NULL,W_ERR_MEM,"malloc blk failed.");
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(blkinfo == W_NULL,W_ERR_MEM,"malloc blkinfo failed.");

        
        info = &lfs->root;
        attr = (LFILE_ATTR_COMMAN | LFILE_ATTR_DIR);
        listfs_fileinfo_init(info,"root",lfs->lfs_info.root_addr,0,0,attr);
        blkinfo_init(blkinfo, lfs->lfs_info.root_addr,0,0,lfs->blkdev->blksize);
        err = fileinfo_write(info,lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"flush lfs root file info failed.");
        err = blkinfo_write(blkinfo,lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"flush lfs root blkinfo failed.");
        
        err = listfs_bitmap_set(&lfs->bitmap,lfs->lfs_info.root_addr,BITMAP_USED);
        if(err != W_ERR_OK)
        {
            wind_error("set lfs root bitmap failed.");
            err = W_ERR_FAIL;
            break;
        }
    }while(0);

    if(blk != W_NULL)
        lfs_free(blk);
    if(blkinfo != W_NULL)
        lfs_free(blkinfo);
    return err;
}


static w_err_t lfs_make_child(listfs_s *lfs,lfile_info_s *pinfo,char *name,w_uint8_t isdir)
{
    w_err_t err;
    w_uint8_t attr;
    w_uint8_t *blk = W_NULL;
    w_addr_t self_addr,cnt;
    lfile_info_s *info;
    lfile_blkinfo_s *blkinfo;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pinfo != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    
    err = listfs_bitmap_alloc_blk(&lfs->bitmap,&self_addr,1);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    do 
    {
        err = W_ERR_OK;
        blk = lfs_malloc(lfs->blkdev->blksize);
        WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
        wind_memset(blk,0,lfs->blkdev->blksize);
        attr = isdir?(LFILE_ATTR_COMMAN|LFILE_ATTR_DIR):LFILE_ATTR_COMMAN;
        info = (lfile_info_s*)blk;
        listfs_fileinfo_init(info,name,self_addr,pinfo->self_addr,pinfo->tailchild_addr,attr);
        blkinfo = FILEINFO_BLKINFO(blk);
        blkinfo_init(blkinfo, self_addr,0,0,lfs->blkdev->blksize);
        
        info->prevfile_addr = pinfo->tailchild_addr;
        cnt = wind_blkdev_write(lfs->blkdev,self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_FAIL,"write file info failed.");
        
        err = fileinfo_update_prev(info,lfs->blkdev);
        WIND_ASSERT_BREAK(cnt > 0,err,"update prev file info failed.");

        err = fileinfo_update_parent(info,lfs->blkdev);
        WIND_ASSERT_BREAK(cnt > 0,err,"update pinfo file info failed.");
        wind_memcpy(pinfo,info,sizeof(lfile_info_s));
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    if(err != W_ERR_OK)
        wind_error("fs should NOT be error here,must rebuild the file system");
    return err;
}


static w_err_t lfs_make_file(listfs_s *lfs,listfile_s *file,char *path)
{
    w_err_t err;
    w_int32_t i,pathlen,cnt;
    w_uint8_t isdir = 0;
    char **nameseg = W_NULL;
    lfile_info_s *finfo = W_NULL;
    lfile_blkinfo_s *blkinfo = W_NULL;
    char *tmppath = W_NULL;

    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_ERR_INVALID);
    
    do
    {
        err = W_ERR_OK;
        //分配内存
        pathlen = wind_strlen(path);
        tmppath = lfs_malloc(pathlen+1);
        WIND_ASSERT_BREAK(tmppath != W_NULL,W_ERR_MEM,"malloc tmppath failed.");
        nameseg = (char **)lfs_malloc(LISTFS_DIR_LAYCNT * sizeof(char*));
        WIND_ASSERT_BREAK(nameseg != W_NULL,W_ERR_MEM,"malloc nameseg failed.");
        finfo = lfs_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(finfo != W_NULL,W_ERR_MEM,"malloc finfo failed.");
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(blkinfo != W_NULL,W_ERR_MEM,"malloc blkinfo failed.");


        //拷贝分割文件路径
        wind_strcpy(tmppath,path);
        if(tmppath[pathlen - 1] == '/')
        {
            tmppath[pathlen - 1] = 0;
            isdir = 1;
        }
        cnt = wind_strsplit(tmppath,'/',nameseg,LISTFS_DIR_LAYCNT);
        WIND_ASSERT_BREAK(cnt > 1,W_ERR_OK,"split path failed.");

        //获取根
        wind_memcpy(finfo,&lfs->root,sizeof(lfs_info_s));

        err = W_ERR_OK;
        for(i = 1;i < cnt;i ++)
        {
            err = lfs_search_child(finfo,nameseg[i],lfs->blkdev);
            if(err != W_ERR_OK)
            {
                err = lfs_make_child(lfs,finfo,nameseg[i],i == cnt - 1?isdir:1);
                WIND_ASSERT_BREAK(err == W_ERR_OK,err,"make child failed");
            }
        }
        if(err != W_ERR_OK)
            break;
    }while(0);
    if(err == W_ERR_OK)
    {
        wind_memcpy(&file->info,finfo,sizeof(lfile_info_s));
        if(!LFILE_IS_DIR(file->info.attr))
        {
            blkinfo_read(blkinfo,lfs->blkdev,file->info.self_addr);
            file->blkinfo = blkinfo;
        }
        else
        {
            file->blkinfo = W_NULL;
        }
    }
    else if(blkinfo != W_NULL)
        lfs_free(blkinfo);
        
    if(tmppath != W_NULL)
        lfs_free(tmppath);
    if(finfo != W_NULL)
        lfs_free(finfo);
    if(nameseg != W_NULL)
        lfs_free(nameseg);
    return err;
}


static w_err_t listfile_destroy(listfile_s* file)
{
    if(file->blkinfo != W_NULL)
    {
        lfs_free(file->blkinfo);
        file->blkinfo = W_NULL;
    }
    
    if(file->subinfo!= W_NULL)
    {
        lfs_free(file->subinfo);
        file->subinfo = W_NULL;
    }
    file->lfs = W_NULL;
    lfs_free(file);
    return W_ERR_OK;
}


w_err_t listfile_remove(listfs_s *lfs,const char *path)
{
    w_err_t err;
    listfile_s *file;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    file = listfile_open(lfs,path,LFMODE_R);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(lfs->lfs_info.magic == LISTFS_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        file->info.magic = 0;
        err = blkinfo_get_byoffset(file->blkinfo,file->lfs->blkdev,0);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get head blkinfo failed");
        while(1)
        {
            err = listfs_bitmap_free_blk(&file->lfs->bitmap,file->blkinfo->dataaddr,LFILE_LBLK_CNT);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"free blk addrs failed,can NOT be error here");
            err = listfs_bitmap_free_blk(&file->lfs->bitmap,&file->blkinfo->self_addr,1);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"free blkinfo addr failed,can NOT be error here");
            err = blkinfo_get_next(file->blkinfo,file->lfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get head blkinfo failed,can NOT be error here");
        }
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"error occured");
        
    }while(0);
    if(file->subinfo != W_NULL)
        lfs_free(file->subinfo);
    lfs_free(file->blkinfo);
    lfs_free(file);
    return err;
}

w_uint16_t calc_unit_size(w_int32_t blkcnt,w_int32_t blksize)
{
    w_int32_t i,maxcnt;
    maxcnt = 128 * 2048;
    for(i = 0;i < 4;i ++)
    {
        if(blkcnt <= maxcnt)
            break;
        else
        {
            maxcnt *= 2;
            blksize *= 2;
        }
    }
    return (w_uint16_t)blksize;
}
static w_err_t lfs_get_fsinfo_by_blk(lfs_info_s *fsinfo,w_blkdev_s *blkdev,w_addr_t addr)
{
    w_err_t err;
    w_int32_t cnt;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsinfo != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        cnt = wind_blkdev_read(blkdev,addr,blk,1);
        WIND_ASSERT_BREAK(cnt == 1,W_ERR_FAIL,"read blkdata failed");
        wind_memcpy(fsinfo,blk,sizeof(lfs_info_s));
        WIND_ASSERT_BREAK(fsinfo->magic == LISTFS_MAGIC,W_ERR_INVALID,"invalid fs header");
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return W_ERR_OK;
}

static w_err_t listfs_get_fsinfo(lfs_info_s *fsinfo,w_blkdev_s *blkdev)
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

static w_err_t lfs_info_init(listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint32_t crc;
    w_int32_t cnt;
    lfs_info_s *lfs_info;
    w_uint8_t *blk = (w_uint8_t *)W_NULL;
    do
    {
        err = W_ERR_OK;
        lfs->blkdev = blkdev;
        blk = (w_uint8_t *)lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        wind_memset(blk,0,blkdev->blksize);
        lfs_info = &lfs->lfs_info;
        lfs_info->magic = LISTFS_MAGIC;
        lfs_info->blkcount = blkdev->blkcnt;
        lfs_info->unit_size = calc_unit_size(blkdev->blkcnt,blkdev->blksize);
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
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"write fsinfo failed.");
        
        cnt = wind_blkdev_write(blkdev,1,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"write bak fsinfo failed.");

    }while(0);
    return err;
}

w_err_t listfs_format(listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->obj.magic == WIND_BLKDEV_MAGIC,W_ERR_INVALID);
    wind_notice("listfs format,blkdev:%s",wind_obj_name(&blkdev->obj));
    do
    {
        err = lfs_info_init(lfs,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init listfs info failed");
        err = listfs_bitmap_init(&lfs->bitmap,lfs->lfs_info.bitmap1_addr,lfs->lfs_info.bitmap_cnt,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init listfs bitmap failed");
        err = listfs_bitmap_clear(&lfs->bitmap);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"clear listfs bitmap info failed");
        err = lfs_make_root(lfs);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"make listfs root failed");
        err = listfs_bitmap_update(&lfs->bitmap);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"update listfs bitmap failed");
        lfs->file_ref = 0;
    }while(0);
    wind_notice("listfs format result:%d",err);
    return err;
}


w_err_t listfs_init(listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    
    wind_notice("listfs init,blkdev:%s",blkdev->obj.name);
    err = listfs_get_fsinfo(&lfs->lfs_info,blkdev);
    if(err != W_ERR_OK)
    {
        wind_error("No file system detected,format dev %s now.",
            wind_obj_name(&blkdev->obj));
        return err;
    }
    fileinfo_read(&lfs->root,blkdev,lfs->lfs_info.root_addr);
    lfs->blkdev = blkdev;
    lfs->file_ref = 0;
    listfs_bitmap_init(&lfs->bitmap,lfs->lfs_info.bitmap1_addr,
        lfs->lfs_info.bitmap_cnt,lfs->blkdev);
    err = listfs_bitmap_update(&lfs->bitmap);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = fileinfo_read(&lfs->root,blkdev,lfs->lfs_info.root_addr);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

w_err_t listfs_deinit(listfs_s *lfs)
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
        err = W_ERR_OK;
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
        listfile_destroy(file);
        file = W_NULL;
    }
    
    if(file != W_NULL)
    {
        file->lfs = lfs;
        file->lfs->file_ref ++;
        file->mode = (w_uint8_t)mode;
        file->subinfo = W_NULL;
        if(mode & LFMODE_A)
            file->offset = file->info.filesize;
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
    if(file->blkinfo != W_NULL)
    {
        lfs_free(file->blkinfo);
        file->blkinfo = W_NULL;
    }
    if(file->subinfo != W_NULL)
    {
        lfs_free(file->subinfo);
        file->subinfo = W_NULL;
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
	err = fileinfo_write(&file->info,file->lfs->blkdev);
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
        err = W_ERR_OK;
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



static w_int32_t do_read_file(listfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    w_err_t err;
    w_addr_t addr; 
    w_int32_t len,blkidx,buffidx,cpsize;
    w_uint8_t *blk = W_NULL;
    if(!BLKINFO_HAS_OFFSET(file->blkinfo, file->offset, file->lfs->lfs_info.blksize))
    {
        err = blkinfo_get_byoffset(file->blkinfo,file->lfs->blkdev, file->offset);
        WIND_ASSERT_RETURN(err == W_ERR_OK, -1);
    }
    err = W_ERR_OK;
    buffidx = 0;
    while(1)
    {
        if(blk == W_NULL)
        {
            blk = lfs_malloc(file->blkinfo->blksize);
            WIND_ASSERT_BREAK(blk != W_NULL, W_ERR_MEM, "malloc blk failed");
        }
        
        blkidx = file->offset % file->blkinfo->blksize;
        cpsize = file->blkinfo->blksize - blkidx;
        if(cpsize > size - buffidx)
            cpsize = size - buffidx;
        addr = blkinfo_get_addr(file->blkinfo,file->offset);
        if(addr == 0)
        {
            blkinfo_get_next(file->blkinfo,file->lfs->blkdev);
            addr = blkinfo_get_addr(file->blkinfo,file->offset);
        }
        WIND_ASSERT_BREAK(addr > 0, W_ERR_FAIL, "get addr failed");
        len = wind_blkdev_read(file->lfs->blkdev, addr,blk, 1);
        WIND_ASSERT_BREAK(len > 0, W_ERR_FAIL,"read data failed");
        wind_memcpy(&buff[buffidx],&blk[blkidx],cpsize);
        file->offset += cpsize;
        buffidx += cpsize;
        if(buffidx >= size)
            break;
    }
    if(blk == W_NULL)
        lfs_free(blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK, -1);
    return size;
}

static w_int32_t do_write_file(listfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    w_err_t err;
    w_addr_t addr; 
    w_int32_t len,blkidx,buffidx,cpsize;
    w_uint8_t *blk = W_NULL;
    if(!BLKINFO_HAS_OFFSET(file->blkinfo, file->offset, file->lfs->lfs_info.blksize))
    {
        err = blkinfo_get_byoffset(file->blkinfo,file->lfs->blkdev, file->offset);
        WIND_ASSERT_RETURN(err == W_ERR_OK, -1);
    }
    err = W_ERR_OK;
    buffidx = 0;
    while(1)
    {
        if(blk == W_NULL)
        {
            blk = lfs_malloc(file->blkinfo->blksize);
            WIND_ASSERT_BREAK(blk != W_NULL, W_ERR_MEM, "malloc blk failed");
        }
        
        blkidx = file->offset % file->blkinfo->blksize;
        cpsize = file->blkinfo->blksize - blkidx;
        if(cpsize > size - buffidx)
            cpsize = size - buffidx;
        addr = blkinfo_get_addr(file->blkinfo,file->offset);
        if(addr == 0)
        {
            blkinfo_get_next(file->blkinfo,file->lfs->blkdev);
            addr = blkinfo_get_addr(file->blkinfo,file->offset);
        }
        WIND_ASSERT_BREAK(addr > 0, W_ERR_FAIL, "get addr failed");
        if((blkidx != 0) || (cpsize != file->blkinfo->blksize))
        {
            len = wind_blkdev_read(file->lfs->blkdev, addr,blk, 1);
            WIND_ASSERT_BREAK(len > 0, W_ERR_FAIL,"read data failed");
        }
        wind_memcpy(&blk[blkidx],&buff[buffidx],cpsize);
        len = wind_blkdev_write(file->lfs->blkdev, addr,blk, 1);
        WIND_ASSERT_BREAK(len > 0, W_ERR_FAIL,"read data failed");
        file->offset += cpsize;
        buffidx += cpsize;
        if(buffidx >= size)
            break;
    }
    if(blk == W_NULL)
        lfs_free(blk);
    WIND_ASSERT_RETURN(err == W_ERR_OK, -1);
    return size;
}

static w_int32_t calc_needed_blks(listfile_s* file,w_int32_t size)
{
    w_int32_t blksize;
    w_int32_t needsize;
    if(file->offset + size <= file->info.spacesize)
        return 0;
    blksize = file->lfs->blkdev->blksize;
    needsize = file->offset + size - file->info.spacesize;
    return (needsize + blksize - 1) / blksize;
}

static w_int32_t calc_needed_blkinfo(listfile_s* file,w_int32_t blkcnt)
{
    w_err_t err;
    w_int32_t blkinfo_cnt;
    lfile_blkinfo_s *tmpinfo;
    tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_RETURN(tmpinfo != W_NULL,-1);
    
    do
    {
        err = W_ERR_OK;
        wind_memcpy(tmpinfo,&file->info,sizeof(lfile_blkinfo_s));
        err = blkinfo_get_tail(tmpinfo,file->lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"find tail blkinfo failed.");
        if(tmpinfo->blkused + blkcnt <= LFILE_LBLK_CNT)
        {
            blkinfo_cnt = 0;
            err = W_ERR_OK;
            break;
        }
        blkcnt -= (LFILE_LBLK_CNT - tmpinfo->blkused);
        blkinfo_cnt = (blkcnt + LFILE_LBLK_CNT - 1) / LFILE_LBLK_CNT;
        
    }while(0);
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);
    if(err == W_ERR_OK)
        return blkinfo_cnt;
    return -1;
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

    do
    {
        err = W_ERR_OK;
        if(file->offset + rsize > file->info.filesize)
            rsize = file->info.filesize - file->offset;
        err = do_read_file(file,buff,rsize);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"do read file data failed");
        file->offset += rsize;
    }while(0);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);        
    return rsize;
}


w_int32_t listfile_write(listfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    w_err_t err;
    w_int32_t wsize,needdatablk,needblkinfo;
    w_addr_t *addrlist = W_NULL;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    WIND_ASSERT_RETURN((file->mode & LFMODE_W)||(file->mode & LFMODE_A),-1);
    WIND_ASSERT_RETURN(file->offset + size < LISTFS_MAX_FILE_SIZE,-1);
    wsize = size;
    do
    {
        err = W_ERR_OK;
        needdatablk = calc_needed_blks(file,wsize);
        needblkinfo = calc_needed_blkinfo(file,needdatablk);
        if(needdatablk + needblkinfo > 0)
        {
            addrlist = (w_addr_t *)lfs_malloc((needdatablk+needblkinfo)*sizeof(w_addr_t *));
            WIND_ASSERT_BREAK(addrlist != W_NULL,W_ERR_MEM,"alloc addrlist failed");
            err = listfs_bitmap_alloc_blk(&file->lfs->bitmap,addrlist,needdatablk+needblkinfo);
            WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"alloc blks failed");
            
            if(needblkinfo > 0)
            {
                err = blkinfo_link(file->blkinfo,file->lfs->blkdev,&addrlist[0],needblkinfo);
                WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"append blkinfo failed.");
            }
            if(needdatablk > 0)
            {
                err = blkinfo_add_dataaddr(file->blkinfo,file->lfs->blkdev,&addrlist[needblkinfo],needdatablk);
                WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"append blk failed.");
            }
        }
        err = do_write_file(file,buff,wsize);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"do write file data failed");
        if(file->info.filesize < file->offset + wsize)
            file->info.filesize += wsize;
        file->offset += wsize;
        err = fileinfo_write(&file->info,file->lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"set file info failed");
    }while(0);
    if(addrlist != W_NULL)
        lfs_free(addrlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);
    return wsize;
}

listfile_s *listfile_readdir(listfile_s* file,w_int32_t index)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(file->mode & LFMODE_R,W_NULL);
    
    WIND_ASSERT_RETURN(file->info.attr,W_NULL);
    return W_NULL;
}


w_err_t listfile_fgets(listfile_s* file,char *buff, w_int32_t maxlen)
{
    w_int32_t i,len;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    WIND_ASSERT_RETURN(file->mode & LFMODE_R,-1);
    
    len = listfile_read(file,buff,maxlen);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    for(i = 0;i < len;i ++)
    {
        if(buff[i] == '\n')
        {
            buff[i] = 0;
            break;
        }
    }
    WIND_ASSERT_RETURN(i < len,W_ERR_FAIL);
    return W_ERR_OK;
}

w_err_t listfile_fputs(listfile_s* file,char *buff)
{
    w_int32_t len;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->mode & LFMODE_W,W_ERR_NOT_SUPPORT);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff[0] != 0,W_ERR_INVALID);
    len = wind_strlen(buff);
    len = listfile_write(file,buff,len);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    return W_ERR_OK;
}

#endif

