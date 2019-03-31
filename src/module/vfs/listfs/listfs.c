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

void *listfs_malloc(w_int32_t size)
{
    void *ptr = wind_malloc(size);
    return ptr;
}

w_err_t listfs_free(void *ptr)
{
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}



static lfile_info_s *listfs_search_child(lfile_info_s *parent,char *name,w_blkdev_s *blkdev)
{
    lfile_info_s *info;
    for(info = fileinfo_headchild(parent,blkdev);info != W_NULL;info = fileinfo_next(info,blkdev))
    {
        if(wind_strcmp(name,info->name) == 0)
            return info;
    }
    return W_NULL;
}

static w_err_t listfs_search_file(listfs_s *lfs,const char *path,lfile_info_s *info)
{
    w_err_t err = W_ERR_FAIL;
    w_int32_t len,cnt,i = 0;
    char **nameseg = W_NULL;
    char *pathname = W_NULL;
    lfile_info_s *finfo = W_NULL;
    do 
    {
        wind_debug("search node path:%s",path);
        //分配内存
        len = wind_strlen(path);
        nameseg = (char **)listfs_malloc(LISTFS_DIR_LAYCNT * sizeof(char*));
        pathname = listfs_malloc(len+1);
        finfo = listfs_malloc(sizeof(lfile_info_s));
        if(pathname == W_NULL || nameseg == W_NULL || info == W_NULL)
        {
            wind_error("alloc memory error");
            err = W_ERR_MEM;
            break;
        }
        
        wind_memset(pathname,0,len+1);
        wind_strcpy(pathname,path);
        pathname[len] = 0;
        //cnt = split_path(pathname,nameseg,LISTFS_DIR_LAYCNT);
        cnt = wind_strsplit(pathname,'/',nameseg,LISTFS_DIR_LAYCNT);
        if(cnt < 0)
        {
            wind_error("split path failed");
            err = W_ERR_INVALID;
            break;
        }

        err = listfs_get_fileinfo(info,lfs->blkdev,lfs->lfs_info.root_addr);
        if(err != W_ERR_OK)
        {
            wind_error("read root directory failed.");
            err = W_ERR_FAIL;
            break;
        }
    
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
            finfo = listfs_search_child(finfo,nameseg[i],lfs->blkdev);
            if(finfo == W_NULL)
            {
                wind_error("read directory %s failed.",nameseg[i]);
                err = W_ERR_FAIL;
                break;
            }
        }
    }while(0);
    if(err == W_ERR_OK)
        wind_memcpy(info,finfo,sizeof(lfs_info_s));
    if(pathname)
        listfs_free(pathname);
    if(nameseg)
        listfs_free(nameseg);
    if(finfo)
        listfs_free(finfo);
    return err;
}

static w_err_t listfs_make_root(listfs_s *lfs)
{
    w_err_t err;
    w_uint8_t attr;
    lfile_info_s *info;
    info = listfs_malloc(sizeof(lfile_info_s));
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_MEM);
    attr = (LFILE_ATTR_COMMAN | LFILE_ATTR_DIR);
    listfs_fileinfo_init(info,"root",lfs->lfs_info.root_addr,0,0,attr);
    err = listfs_set_fileinfo(info,lfs->blkdev,lfs->lfs_info.root_addr);
    listfs_free(info);
    return err;
}


static w_err_t listfs_make_node(listfs_s *lfs,lfile_info_s *parent,char *name,w_uint8_t isdir)
{
    w_err_t err;
    w_uint8_t attr;
    w_uint8_t *blk = W_NULL;
    w_int32_t self_addr,cnt;//brother_addr,
    lfile_info_s *info;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    
    err = listfs_bitmap_find_free(lfs,&self_addr);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    
    do 
    {
        err = W_ERR_OK;
        blk = listfs_malloc(lfs->blkdev->blksize);
        WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
        wind_memset(blk,0,lfs->blkdev->blksize);
        attr = isdir?(LFILE_ATTR_COMMAN|LFILE_ATTR_DIR):LFILE_ATTR_COMMAN;
        info = (lfile_info_s*)blk;
        listfs_fileinfo_init(info,name,self_addr,parent->self_addr,parent->tailchild_addr,attr);
        info->prevfile_addr = parent->tailchild_addr;
        cnt = wind_blkdev_write(lfs->blkdev,self_addr,blk,1);
        if(cnt <= 0)
        {
            wind_error("write file info failed.");
            err = W_ERR_FAIL;
            break;
        }
        err = fileinfo_update_prev(info,lfs->blkdev);
        if(err != W_ERR_OK)
        {
            wind_error("update prev file info failed.");
            break;
        }

        err = fileinfo_update_parent(info,lfs->blkdev);
        if(err != W_ERR_OK)
        {
            wind_error("update parent file info failed.");
            break;
        }
    }while(0);
    if(blk != W_NULL)
        listfs_free(blk);
    if(err != W_ERR_OK)
        wind_error("fs should NOT be error here,must restore the file system");
    return err;
}


static w_err_t listfs_make_file(listfs_s *lfs,lfile_info_s *info,char *path)
{
    w_err_t err;
    char *ptr;
    w_int32_t pathlen;
    w_uint8_t isdir = 0;
    char *filename = W_NULL;
    listfile_s *file = W_NULL;
    char *nodename;
    do
    {
        err = W_ERR_OK;
        pathlen = wind_strlen(path);
        filename = listfs_malloc(pathlen+1);
        WIND_ASSERT_RETURN(filename != W_NULL,W_ERR_MEM);
        wind_strcpy(filename,path);
        if(filename[pathlen - 1] == '/')
        {
            filename[pathlen - 1] = 0;
            isdir = 1;
        }
        
        ptr = wind_strrchr(filename,'/');
        if(ptr == W_NULL)
        {
            wind_error("get dir failed,path:%s",path);
            err = W_ERR_INVALID;
            break;
        }
        *ptr = 0;
        nodename = &ptr[1];
        file = listfs_malloc(sizeof(listfile_s));
        
        err = listfs_search_file(lfs,path,&file->info);
        if(err != W_ERR_OK)
        {
            wind_error("search filenode failed");
            err = W_ERR_FAIL;
            break;
        }
        err = listfs_make_node(lfs,&file->info,nodename,isdir);
    }while(0);
    if(filename != W_NULL)
        listfs_free(filename);
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
        blk = listfs_malloc(blkdev->blksize);
        WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_MEM);
        wind_memset(blk,0,blkdev->blksize);
        lfs_info = &lfs->lfs_info;
        lfs_info->magic = LISTFS_MAGIC;
        lfs_info->blkcount = blkdev->blkcnt;
        lfs_info->unit_size = (w_uint16_t)blkdev->blksize;
        lfs_info->blksize = (w_uint16_t)blkdev->blksize;
        lfs_info->reserve_blk = 1;
        lfs_info->attr = 0;
        lfs_info->bitmap1 = lfs_info->reserve_blk + 1;
        lfs_info->bitmap_cnt = (blkdev->blkcnt - lfs_info->bitmap1) / (lfs_info->unit_size + 1) + 1;
        lfs_info->bitmap2 = lfs_info->bitmap1 + lfs_info->bitmap_cnt;
        lfs_info->root_addr = lfs_info->bitmap2 + lfs_info->bitmap_cnt;
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

        listfs_bitmap_clear(lfs_info,blkdev);
        wind_memset(blk,0,blkdev->blksize);
        blk[0] = BITMAP_USED;
        wind_blkdev_write(blkdev,lfs_info->bitmap1,blk,1);
        wind_blkdev_write(blkdev,lfs_info->bitmap2,blk,1);
        listfs_make_root(lfs);
    }while(0);
    if(blk != W_NULL)
        listfs_free(blk);
    return err;
}


w_err_t listfs_mount(listfs_s *lfs,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t len;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    lfs->blkdev = blkdev;
    err = listfs_get_fsinfo(&lfs->lfs_info,blkdev);
    if(err != W_ERR_OK)
    {
        wind_notice("No file system detected,format dev %s now.",
            wind_obj_name(&blkdev->obj));
        listfs_format(lfs,blkdev);
    }
    //WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    listfs_bitmap_init(lfs);
    return W_ERR_OK;
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
        file = listfs_malloc(sizeof(listfile_s));
        WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
        is_crt = (mode & LF_FMODE_CRT)?W_TRUE:W_FALSE;
        err = listfs_search_file(lfs,path,&file->info);
        if((err != W_ERR_OK) && (!is_crt))
        {   //没有创建标记，且文件不存在
            err = W_ERR_FAIL;
            break;
        }
        else if(!is_crt)
        {   //没有创建标记，且文件存在
            err = W_ERR_OK;
            file->mode = (w_uint8_t)mode;
            file->offset = 0;
            break;
        }

        //有创建标记，且文件不存在
        err = listfs_get_fileinfo(&file->info,lfs->blkdev,lfs->lfs_info.root_addr);
        if(err != W_ERR_OK)
        {
            wind_debug("get root info failed");
            break;
        }
        err = listfs_make_file(lfs,&file->info,(char*)path);
        if(err != W_ERR_OK)
        {
            wind_debug("make file %s failed",path);
            break;
        }
        file->mode = (w_uint8_t)mode;
        if(mode & LF_FMODE_A)
            file->offset = file->filelen;
        else
            file->offset = 0;
    }while(0);
    if((err != W_ERR_OK)&&(file != W_NULL))
    {   //打开文件过层中出错
        listfs_free(file);
        file = W_NULL;
    }
    return file;
}



w_err_t listfile_close(listfile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    file->info.magic = 0;
    listfs_free(file);
    return W_ERR_OK;
}

w_bool_t listfile_existing(listfs_s *lfs,const char *path)
{
    listfile_s *file;
    file = listfile_open(lfs,path,LF_FMODE_R);
    if(file == W_NULL)
        return W_FALSE;
    listfile_close(file);
    return W_TRUE;
}

w_err_t listfile_seek(listfile_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    
    return W_ERR_FAIL;
}

w_int32_t listfile_ftell(listfile_s* file)
{
    return -1;
}

w_int32_t listfile_read(listfile_s* file,w_uint8_t *buff, w_int32_t size)
{
    return 0;
}

w_int32_t listfile_write(listfile_s* file,w_uint8_t *buff, w_int32_t size)
{
    return 0;
}

listfile_s *listfile_readdir(listfile_s* file,w_int32_t index)
{
    return W_NULL;
}


w_err_t listfile_fgets(listfile_s* file,char *buff, w_int32_t maxlen)
{
    return W_ERR_FAIL;
}
w_err_t listfile_fputs(listfile_s* file,char *buff)
{
    return W_ERR_FAIL;
}

#endif

