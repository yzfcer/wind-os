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
#if WIND_MODULE_VFS_SUPPORT


#define NODE_TO_LISTFILE(node) (w_listfile_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_listfile_s*)0)->list.listnode)))
w_err_t _wind_listfs_mod_init(void)
{
    return W_ERR_OK;
}

void lfs_info_be2le(lfs_info_s *info)
{
    if(wind_endian() == ENDIAN_BIG)
    {
        BE2LE_4(info->magic);
        BE2LE_4(info->blkcount);
        BE2LE_2(info->unit_size);
        BE2LE_2(info->blksize);
        BE2LE_2(info->reserve_blk);
        BE2LE_2(info->attr);
        BE2LE_4(info->bitmap_cnt);
        BE2LE_4(info->bitmap1_addr);
        BE2LE_4(info->bitmap2_addr);
        BE2LE_4(info->root_addr);
    }
}


void *lfs_malloc(w_int32_t size)
{
    void *ptr = wind_falloc(size,253);
    if(ptr != W_NULL)
        wind_memset(ptr,0,size);
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
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    if(info->headchild_addr == 0)
    {
        wind_debug("fileinfo has no children");
        return W_ERR_FAIL;
    }        
    do
    {
        err = W_ERR_OK;
        tmpinfo = lfs_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        wind_memcpy(tmpinfo,info,sizeof(lfile_info_s));
        err = fileinfo_get_headchild(tmpinfo,blkdev);
        if(err != W_ERR_OK)
        {
            wind_debug("file has no child");
            break;
        }
        //WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        for(;tmpinfo != W_NULL;)
        {
            if(wind_strcmp(name,tmpinfo->name) == 0)
            {
                wind_memcpy(info,tmpinfo,sizeof(lfile_info_s));
                break;
            }
                
            if(tmpinfo ->nextfile_addr == 0)
            {
                err = W_ERR_FAIL;
                break;
            }
                
            err = fileinfo_get_next(tmpinfo,blkdev);
            if(err != W_ERR_OK)
                break;
        }
        
    }while(0);
    if(tmpinfo != W_NULL)
        lfs_free(tmpinfo);

    return err;
}

static w_err_t lfs_search_file(w_listfs_s *lfs,w_listfile_s *file,const char *path)
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
        //拷贝和分割文件路径
        wind_debug("search node path:%s",path);
        len = wind_strlen(path);
        tmppath = lfs_malloc(len+1);
        WIND_ASSERT_BREAK(tmppath,W_ERR_MEM,"malloc tmppath failed");
        wind_strcpy(tmppath,path);
        if(tmppath[len-1] == '/')
        {
            isdir = 1;
            tmppath[len-1] = 0;
        }

        //分割目录
        nameseg = (char **)lfs_malloc(LISTFS_DIR_LAYCNT * sizeof(char*));
        WIND_ASSERT_BREAK(nameseg,W_ERR_MEM,"malloc nameseg failed");
        segcnt = wind_strsplit(tmppath,'/',nameseg,LISTFS_DIR_LAYCNT);
        WIND_ASSERT_BREAK(segcnt > 0,W_ERR_INVALID,"split path failed");
        
        //分配文件信息和数据块信息结构
        finfo = lfs_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(finfo,W_ERR_MEM,"malloc finfo failed");
        if(!isdir)
        {
            blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
            WIND_ASSERT_BREAK(blkinfo,W_ERR_MEM,"malloc blkinfo failed");
        }


        //读取根目录文件信息
        err = fileinfo_read(finfo,lfs->blkdev,lfs->lfs_info.root_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read root failed");
        if(segcnt == 1)
        {
            err = W_ERR_OK;
            break;
        }

        //从根目录开始搜索子文件
        err = W_ERR_OK;
        for(i = 1;i < segcnt;i ++)
        {
            if(finfo->headchild_addr == 0)
            {
                err = W_ERR_FAIL;
                break;
            }
            err = lfs_search_child(finfo,nameseg[i],lfs->blkdev);
            if(err != W_ERR_OK)
            {
                wind_debug("search child file %s failed",nameseg[i]);
                break;
            }
            //WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read directory failed.");
        }
    }while(0);
    if(err == W_ERR_OK)
    {
        wind_memcpy(&file->info,finfo,sizeof(lfile_info_s));
        if(!isdir)
        {
            blkinfo_read(blkinfo,lfs->blkdev,finfo->self_addr);
            file->blkinfo = blkinfo;
        }
        else
        {
            lfs_free(blkinfo);
            file->blkinfo = W_NULL;
        }
    }
        
    if(tmppath)
        lfs_free(tmppath);
    if(nameseg)
        lfs_free(nameseg);
    if(finfo)
        lfs_free(finfo);
    return err;
}

static w_err_t lfs_make_root(w_listfs_s *lfs)
{
    w_err_t err;
    w_uint8_t attr;
    lfile_info_s *finfo;
    w_addr_t addr;
    w_uint8_t *blk = W_NULL;
    lfile_blkinfo_s *blkinfo = W_NULL;
    wind_notice("lfs_make_root");
    do 
    {
        err = W_ERR_OK;
        finfo = lfs_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(finfo != W_NULL,W_ERR_MEM,"malloc finfo failed");
        blk = lfs_malloc(lfs->blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed.");
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(blkinfo != W_NULL,W_ERR_MEM,"malloc blkinfo failed.");

        err = listfs_bitmap_alloc_blk(&lfs->bitmap,&addr,1);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"malloc addr failed");
        WIND_ASSERT_BREAK(addr == lfs->lfs_info.root_addr,W_ERR_FAIL,"root addr error");

        attr = (LFILE_ATTR_COMMAN | LFILE_ATTR_DIR);
        fileinfo_init(finfo,"root",lfs->lfs_info.root_addr,0,0,attr);
        blkinfo_init(blkinfo, lfs->lfs_info.root_addr,0,0,lfs->blkdev->blksize);
        err = fileinfo_write(finfo,lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"flush lfs root file info failed.");
        err = blkinfo_write(blkinfo,lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"flush lfs root blkinfo failed.");

    }while(0);

    if(blk != W_NULL)
        lfs_free(blk);
    if(finfo != W_NULL)
        lfs_free(finfo);
    
    if(blkinfo != W_NULL)
        lfs_free(blkinfo);
    return err;
}


static w_err_t lfs_make_child(w_listfs_s *lfs,lfile_info_s *pinfo,char *name,w_uint8_t isdir)
{
    w_err_t err;
    w_uint8_t attr;
    w_uint8_t *blk = (w_uint8_t *)W_NULL;
    w_addr_t self_addr,cnt;
    lfile_info_s *info;
    lfile_blkinfo_s *blkinfo;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pinfo != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    
    do 
    {
        err = W_ERR_OK;
        err = listfs_bitmap_alloc_blk(&lfs->bitmap,&self_addr,1);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"alloc blk addr failed");
        blk = (w_uint8_t*)lfs_malloc(lfs->blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"alloc blk failed");
        attr = isdir?(LFILE_ATTR_COMMAN|LFILE_ATTR_DIR):LFILE_ATTR_COMMAN;
        info = (lfile_info_s*)blk;
        fileinfo_init(info,name,self_addr,pinfo->self_addr,pinfo->tailchild_addr,attr);
        if(!isdir)
        {
            blkinfo = FILEINFO_BLKINFO(blk);
            blkinfo_init(blkinfo, self_addr,0,0,lfs->blkdev->blksize);
        }
        if(self_addr == info->parent_addr)
            wind_error("error addr alloced:0x%x",self_addr);
        cnt = wind_blkdev_write(lfs->blkdev,self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_FAIL,"write file info failed.");

        
        if(info->prevfile_addr != 0)
        {
            err = fileinfo_add_update_prev(info,lfs->blkdev);
            WIND_ASSERT_BREAK(cnt > 0,err,"update prev file info failed.");
        }

        err = fileinfo_add_update_parent(info,lfs->blkdev);
        WIND_ASSERT_BREAK(cnt > 0,err,"update pinfo file info failed.");
        wind_memcpy(pinfo,info,sizeof(lfile_info_s));
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    if(err != W_ERR_OK)
    {
        wind_error("fs should NOT be error here,must rollback here");
    }
    return err;
}


static w_err_t lfs_make_file(w_listfs_s *lfs,w_listfile_s *file,char *path)
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
        err = fileinfo_read(finfo,lfs->blkdev,lfs->lfs_info.root_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read root failed");

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


static w_err_t listfile_destroy(w_listfile_s* file)
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

static w_err_t lfile_free_blkaddr(w_listfs_s *lfs,lfile_info_s *finfo)
{
    w_err_t err;
    lfile_blkinfo_s *blkinfo = W_NULL;
    do
    {
        err = W_ERR_OK;
        blkinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
        WIND_ASSERT_BREAK(blkinfo != W_NULL,W_ERR_MEM,"alloc blkinfo failed");
        err = blkinfo_read(blkinfo,lfs->blkdev,finfo->self_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get offset failed");
        err = blkinfo_read(blkinfo,lfs->blkdev,finfo->self_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkinfo failed");
        while(1)
        {
            err = listfs_bitmap_free_blk(&lfs->bitmap,blkinfo->dataaddr,LFILE_LBLK_CNT);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"free blk addrs failed,can NOT be error here");
            err = listfs_bitmap_free_blk(&lfs->bitmap,&blkinfo->self_addr,1);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"free blkinfo addr failed,can NOT be error here");
            if(blkinfo->nextblk_addr == 0)
                break;
            err = blkinfo_get_next(blkinfo,lfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get head blkinfo failed,can NOT be error here");
        }
        //if(err != W_ERR_OK)
        //    break;
    }while(0);
    if(blkinfo != W_NULL)
        lfs_free(blkinfo);
    return err;
}

static w_err_t do_remove_file(w_listfs_s *lfs,lfile_info_s *finfo)
{
    w_err_t err;
    do
    {
        err = W_ERR_OK;
        if(finfo->nextfile_addr != 0)
        {
            err = fileinfo_rm_update_next(finfo,lfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"update next file info failed");
        }
        if(finfo->prevfile_addr != 0)
        {
            err = fileinfo_rm_update_prev(finfo,lfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"update prev file info failed");
        }
        if(finfo->parent_addr != 0)
        {
            err = fileinfo_rm_update_parent(finfo,lfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"update parent file info failed");
        }
        finfo->nextfile_addr = 0;
        finfo->prevfile_addr = 0;
        finfo->parent_addr = 0;
        if(finfo->filesize > 0)
        {
            err = lfile_free_blkaddr(lfs,finfo);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"free blkaddr failed");
            finfo->filesize = 0;
        }
        else
        {
            listfs_bitmap_free_blk(&lfs->bitmap,&finfo->self_addr,1);
        }
        finfo->magic = 0;
    }while(0);
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
    return err;
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

w_err_t lfs_info_read(lfs_info_s *lfs_info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t cnt;
    w_uint32_t crc,calc_crc;
    lfs_info_s *tmpinfo;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(lfs_info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        blk = (w_uint8_t *)lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        
        cnt = wind_blkdev_read(blkdev,0,blk,1);
        if(cnt <= 0)
            cnt = wind_blkdev_read(blkdev,1,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"read fsinfo failed.");
        wind_to_uint32(&blk[blkdev->blksize-4],&crc);
        calc_crc = wind_crc32(blk,blkdev->blksize-4,0xffffffff);
        WIND_ASSERT_BREAK(calc_crc == crc,W_ERR_INVALID,"check lfs info crc failed");
        tmpinfo = (lfs_info_s*)blk;
        lfs_info_be2le(tmpinfo);
        WIND_ASSERT_BREAK(tmpinfo->magic == LISTFS_MAGIC,W_ERR_INVALID,"invalid lfs head");
        wind_memcpy(lfs_info,tmpinfo,sizeof(lfs_info_s));
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}


w_err_t lfs_info_write(lfs_info_s *lfs_info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t cnt;
    w_uint32_t crc;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(lfs_info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    do 
    {
        err = W_ERR_OK;
        blk = (w_uint8_t *)lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        lfs_info_be2le(lfs_info);

        //wind_memset(blk,0,blkdev->blksize);
        wind_memcpy(blk,lfs_info,sizeof(lfs_info_s));
        crc = wind_crc32(blk,blkdev->blksize-4,0xffffffff);
        wind_from_uint32(&blk[blkdev->blksize-4],crc);
        
        cnt = wind_blkdev_write(blkdev,0,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"write fsinfo failed.");
        
        cnt = wind_blkdev_write(blkdev,1,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"write bak fsinfo failed.");
        
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
    
}

static w_err_t lfs_info_init(w_listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfs_info_s *lfs_info;

    err = W_ERR_OK;
    lfs->blkdev = blkdev;

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
    
    err = lfs_info_write(lfs_info,blkdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

w_err_t listfs_format(w_listfs_s *lfs,w_blkdev_s *blkdev)
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
        lfs->file_ref = 0;
    }while(0);
    wind_notice("listfs format result:%d",err);
    return err;
}


w_err_t listfs_init(w_listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_info_s *finfo = W_NULL;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    do 
    {
        err = W_ERR_OK;
        finfo = lfs_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(finfo != W_NULL,W_ERR_MEM,"malloc finfo failed");
        
        wind_notice("listfs init,blkdev:%s",blkdev->obj.name);
        err = listfs_get_fsinfo(&lfs->lfs_info,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read fsinfo failed");
        
        lfs->blkdev = blkdev;
        lfs->file_ref = 0;
        listfs_bitmap_init(&lfs->bitmap,lfs->lfs_info.bitmap1_addr,
            lfs->lfs_info.bitmap_cnt,lfs->blkdev);
        err = fileinfo_read(finfo,blkdev,lfs->lfs_info.root_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"check root failed");
    }while(0);
    if(finfo != W_NULL)
        lfs_free(finfo);
    return err;
}

w_err_t listfs_deinit(w_listfs_s *lfs)
{
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(lfs->lfs_info.magic == LISTFS_MAGIC,W_ERR_INVALID);
    lfs->blkdev = W_NULL;
    lfs->lfs_info.magic = (~LISTFS_MAGIC);
    return W_ERR_OK;
}


w_listfile_s* listfile_open(w_listfs_s *lfs,const char *path,w_uint16_t mode)
{
    w_err_t err;
    w_bool_t is_crt;
    w_listfile_s *file = W_NULL;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_NULL);
    wind_trace("open file:%s",path);
    do 
    {
        err = W_ERR_OK;
        file = lfs_malloc(sizeof(w_listfile_s));
        WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
        //wind_memset(&file->info,0,sizeof(lfile_info_s));
        file->lfs = lfs;
        file->mode = mode;
        file->offset = 0;
        file->subinfo = W_NULL;
        file->blkinfo = W_NULL;
        
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
        file->lfs->file_ref ++;
        if(mode & LFMODE_A)
            file->offset = file->info.filesize;
    }
    return file;
}



w_err_t listfile_close(w_listfile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->lfs->file_ref > 0,W_ERR_INVALID);
    wind_trace("close file:%s",file->info.name);
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


w_err_t listfile_remove(w_listfs_s *lfs,const char *path)
{
    w_err_t err;
    w_listfile_s *file;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    wind_trace("remove file:%s",path);
    file = listfile_open(lfs,path,LFMODE_R);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(lfs->lfs_info.magic == LISTFS_MAGIC,W_ERR_INVALID);
    if(file->info.attr & LFILE_ATTR_DIR)
    {
        wind_error("can not remove directory");
        return W_ERR_FAIL;
    }
    do
    {
        err = do_remove_file(lfs,&file->info);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"remove fale failed");

    }while(0);
    if(file->subinfo != W_NULL)
        lfs_free(file->subinfo);
    lfs_free(file->blkinfo);
    lfs_free(file);
    return err;
}


w_err_t listfile_set_attr(w_listfile_s* file,w_uint8_t attr)
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

w_err_t listfile_get_attr(w_listfile_s* file,w_uint8_t *attr)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    *attr = file->info.attr;
    return W_ERR_OK;
}


w_bool_t listfile_existing(w_listfs_s *lfs,const char *path)
{
    w_listfile_s *file;
    file = listfile_open(lfs,path,LFMODE_R);
    if(file == W_NULL)
        return W_FALSE;
    listfile_close(file);
    return W_TRUE;
}

w_err_t listfile_seek(w_listfile_s* file,w_int32_t offset)
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

w_int32_t listfile_ftell(w_listfile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(file->info.magic == LISTFILE_MAGIC,-1);
    return file->offset;
}



static w_int32_t do_read_file(w_listfile_s* file,w_uint8_t *buff,w_int32_t size)
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

static w_int32_t do_write_file(w_listfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    w_err_t err;
    w_addr_t addr; 
    w_int32_t len,blkidx,buffidx,cpsize;
    w_uint8_t *blk = W_NULL;
    blkinfo_read(file->blkinfo,file->lfs->blkdev,file->blkinfo->self_addr);
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

static w_int32_t calc_needed_datablks(w_listfile_s* file,w_int32_t size)
{
    w_int32_t blksize;
    w_int32_t needsize;
    if(file->offset + size <= file->info.spacesize)
        return 0;
    blksize = file->lfs->blkdev->blksize;
    needsize = file->offset + size - file->info.spacesize;
    return (needsize + blksize - 1) / blksize;
}

static w_int32_t calc_needed_blkinfo(w_listfile_s* file,w_int32_t blkcnt)
{
    w_err_t err;
    w_int32_t blkinfo_cnt;
    lfile_blkinfo_s *tmpinfo;
    tmpinfo = lfs_malloc(sizeof(lfile_blkinfo_s));
    WIND_ASSERT_RETURN(tmpinfo != W_NULL,-1);
    
    do
    {
        err = W_ERR_OK;
        wind_memcpy(tmpinfo,file->blkinfo,sizeof(lfile_blkinfo_s));
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


w_int32_t listfile_read(w_listfile_s* file,w_uint8_t *buff, w_int32_t size)
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
        size = do_read_file(file,buff,rsize);
        WIND_ASSERT_BREAK(size > 0,W_ERR_FAIL,"do read file data failed");
        file->offset += rsize;
    }while(0);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);        
    return size;
}


w_int32_t listfile_write(w_listfile_s* file,w_uint8_t *buff,w_int32_t size)
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
        needdatablk = calc_needed_datablks(file,wsize);
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
        size = do_write_file(file,buff,wsize);
        WIND_ASSERT_BREAK(size == wsize,W_ERR_FAIL,"do write file data failed");
        if(file->info.filesize < file->offset + wsize)
            file->info.filesize += wsize;
        file->offset += wsize;
        err = fileinfo_write(&file->info,file->lfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"set file info failed");
    }while(0);
    if(addrlist != W_NULL)
        lfs_free(addrlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);
    return size;
}

w_err_t listfile_readdir(w_listfile_s* dir,w_listfile_s** sub)
{
    w_err_t err;
    w_listfile_s* sublfile;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->lfs->blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->info.magic == LISTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dir->mode & LFMODE_R,W_ERR_INVALID);
    WIND_ASSERT_RETURN(dir->info.attr,W_ERR_INVALID);
    
    do
    {
        err = W_ERR_OK;
        sublfile = *sub;
        if(sublfile == W_NULL)
        {
            sublfile = lfs_malloc(sizeof(w_listfile_s));
            WIND_ASSERT_BREAK(sublfile != W_NULL,W_ERR_MEM,"malloc listfile failed");
            wind_memcpy(&sublfile->info,&dir->info,sizeof(lfile_info_s));
            err = fileinfo_get_headchild(&sublfile->info,dir->lfs->blkdev);
        }
        else
        {
            err = fileinfo_get_next(&sublfile->info,dir->lfs->blkdev);
        }
        *sub = sublfile;
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
        WIND_ASSERT_BREAK(sublfile->info.magic == LISTFILE_MAGIC,W_ERR_FAIL,"invalid fileinfo");
        
    }while(0);
    if(err != W_ERR_OK)
    {
        listfile_destroy(sublfile);
        *sub = W_NULL;
    }
    
    return err;
}


w_err_t listfile_fgets(w_listfile_s* file,char *buff, w_int32_t maxlen)
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

w_err_t listfile_fputs(w_listfile_s* file,char *buff)
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

