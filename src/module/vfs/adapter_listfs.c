/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: adapter_listfs.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: listfs文件系统适配层接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 备  注：不对用户程序暴露
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_fs.h"
#include "wind_file.h"
#include "listfs.h"
//#include "listfile.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#if WIND_LISTFS_SUPPORT
static w_err_t listfs_op_opsinit(void)
{
    w_err_t err;
    err = _wind_listfs_mod_init();
    return err;
}

static void* listfs_op_init(w_vfs_s *vfs)
{
    w_err_t err;
    w_listfs_s *lfs;
    WIND_ASSERT_RETURN(vfs != W_NULL, W_NULL);
    do
    {
        err = W_ERR_OK;
        if(vfs->fsobj == W_NULL)
            vfs->fsobj = listfs_mem_malloc(sizeof(w_listfs_s));
        WIND_ASSERT_BREAK(vfs->fsobj != W_NULL,W_ERR_MEM,"alloc fsobj failed");
        lfs = (w_listfs_s *)vfs->fsobj;
        lfs->blkdev = vfs->blkdev;
        err = listfs_init(lfs,vfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"listfs init failed");
    }while(0);
    
    if(err != W_ERR_OK)
    {
        if(vfs->fsobj != W_NULL)
        {
            listfs_mem_free(vfs->fsobj);
            vfs->fsobj = W_NULL;
        }
    }
    return vfs->fsobj;
}

static w_err_t listfs_op_deinit(w_vfs_s *vfs)
{
    w_err_t err;
    w_listfs_s *lfs;
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    do 
    {
        err = W_ERR_OK;
        lfs = (w_listfs_s *)vfs->fsobj;
        err = listfs_deinit(lfs);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"listfs deinit failed");
        listfs_mem_free(vfs->fsobj);
        vfs->fsobj = W_NULL;
    }while(0);
    return err;
}


static w_err_t listfs_op_format(w_vfs_s *vfs)
{
    w_err_t err;
    w_listfs_s *lfs;
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    //WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        if(vfs->fsobj == W_NULL)
            vfs->fsobj = listfs_mem_malloc(sizeof(w_listfs_s));
        WIND_ASSERT_BREAK(vfs->fsobj != W_NULL,W_ERR_MEM,"alloc fsobj failed");
        lfs = (w_listfs_s *)vfs->fsobj;
        lfs->blkdev = vfs->blkdev;
        err = listfs_format(lfs,vfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"listfs init failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        if(vfs->fsobj != W_NULL)
        {
            listfs_mem_free(vfs->fsobj);
            vfs->fsobj = W_NULL;
        }
    }
    return err;
}

w_err_t listfs_op_matchfs(char *devname)
{
    w_err_t err;
    w_blkdev_s *blkdev;
    w_listfs_s * lfs = W_NULL;
    do
    {
        err = W_ERR_OK;
        lfs = wind_alloc(sizeof(w_listfs_s),HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(lfs != W_NULL,W_ERR_MEM,"alloc lfs failed");
        blkdev = wind_blkdev_get(devname);
        WIND_ASSERT_BREAK(blkdev != W_NULL,W_ERR_FAIL,"blkdev is not exist");
        err = listfs_init(lfs,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init lfs failed");
        listfs_deinit(lfs);
    }while(0);
    if(lfs != W_NULL)
        wind_free(lfs);
    return err;
}


static w_err_t listfs_op_open(w_file_s *file,w_uint8_t fmode)
{
    w_listfile_s *lfile;
    w_listfs_s *lfs;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    lfs = (w_listfs_s*)file->vfs->fsobj;
    WIND_ASSERT_RETURN(lfs != W_NULL,W_ERR_FAIL);

    lfile = listfile_open(lfs,file->realpath,fmode);
    if(lfile == W_NULL)
        return W_ERR_FAIL;
    
    file->fileobj = lfile;
    file->isdir = IS_LFILE_ATTR_DIR(lfile->info.attr)?1:0;
    file->offset = 0;
    
    return W_ERR_OK;
}

static w_err_t listfs_op_close(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_close((w_listfile_s *)file->fileobj);
}

static w_err_t listfs_op_remove(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
	return listfile_remove((w_listfs_s *)file->vfs->fsobj,file->realpath);
}

static w_err_t listfs_op_readdir(w_file_s* dir,w_file_s* sub)
{
    w_err_t err;
    w_int32_t len;
    w_listfile_s *sublfile = (w_listfile_s *)W_NULL;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        sublfile = (w_listfile_s *)sub->fileobj;
        err = listfile_readdir((w_listfile_s *)dir->fileobj,&sublfile);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
        WIND_ASSERT_BREAK(sublfile->info.magic == LISTFILE_MAGIC,W_ERR_INVALID,"invalid listfile dound");
        sub->fileobj = sublfile;
        sub->obj.magic = WIND_FILE_MAGIC;
        
        if(sub->obj.name != W_NULL)
            wind_free((void*)sub->obj.name);
        sub->obj.name = (const char*)wind_salloc(sublfile->info.name,HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(sub->obj.name != W_NULL,W_ERR_MEM,"malloc filename failed");
        sub->isdir = IS_LFILE_ATTR_DIR(sublfile->info.attr)?1:0;
        
        if(sub->fullpath)
            wind_filepath_release(sub->fullpath);
        sub->fullpath = wind_filepath_generate(dir->fullpath,(char *)sub->obj.name,sub->isdir);
        WIND_ASSERT_BREAK(sub->fullpath != W_NULL,W_ERR_MEM,"malloc fullpath failed");

        len = wind_strlen(dir->vfs->mount_path);
        WIND_ASSERT_BREAK(len >= 1,W_ERR_INVALID,"get mount path lenth failed");
        sub->realpath = &sub->fullpath[len - 1];
        
        sub->vfs = dir->vfs;
        
    }while(0);

    return err;
}

static w_err_t listfs_op_seek(w_file_s* file,w_uint32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_seek((w_listfile_s *)file->fileobj,offset);
}

static w_err_t listfs_op_rename(w_file_s* file,char *newname)
{
    w_err_t err;
    w_int32_t len;
    w_listfile_s *lfile;
    char *oldname = (char *)W_NULL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname[0] != 0,W_ERR_INVALID);
    len = wind_strlen(newname);
    WIND_ASSERT_RETURN(len < LFILE_NAME_LEN,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        lfile = (w_listfile_s *)file->fileobj;
        oldname = (char *)wind_salloc(lfile->info.name,HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(oldname != W_NULL,W_ERR_MEM,"malloc filename failed");
        wind_strcpy(lfile->info.name,newname);
        err = fileinfo_write(&lfile->info,file->vfs->blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write fileinfo failed");
    }while(0);
    if(oldname != W_NULL)
        wind_free(oldname);
    return err;
}

static w_int32_t listfs_op_ftell(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_ftell((w_listfile_s *)file->fileobj);
}

static w_int32_t listfs_op_read(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_read((w_listfile_s *)file->fileobj,buff,size);
}

static w_int32_t listfs_op_write(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_write((w_listfile_s *)file->fileobj,buff,size);
}

static w_err_t listfs_op_fgets(w_file_s* file,char *buff, w_int32_t maxlen)
{
    return W_ERR_FAIL;
}

static w_err_t listfs_op_fputs(w_file_s* file,char *buff)
{
    return W_ERR_FAIL;
}

FS_OPS_DEF(listfs);

#endif
