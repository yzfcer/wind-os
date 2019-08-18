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
static void* listfs_op_init(w_vfs_s *vfs)
{
    w_err_t err;
    w_listfs_s *lfs;
    lfs = (w_listfs_s *)vfs->fsobj;
    err = listfs_init(lfs,vfs->blkdev);
    WIND_ASSERT_RETURN(err != W_ERR_OK,W_NULL);
    return lfs;
}

static w_err_t listfs_op_format(w_vfs_s *vfs)
{
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    return listfs_format((w_listfs_s *)vfs->fsobj,vfs->blkdev);
}

static w_err_t listfs_op_open(w_file_s *file,w_uint16_t fmode)
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
    file->isdir = LFILE_IS_DIR(lfile->info.attr)?1:0;
    file->offset = 0;
    
    return W_ERR_OK;
}

static w_err_t listfs_op_close(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_close((w_listfile_s *)file->fileobj);
}

static w_err_t listfs_op_rmfile(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_remove((w_listfile_s *)file->vfs,file->realpath);
}

static w_err_t listfs_op_readdir(w_file_s* dir,w_file_s* sub)
{
    w_err_t err;
    w_listfile_s *lfile;
    w_listfile_s *sublfile = W_NULL;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        sublfile = (w_listfile_s *)sub->fileobj;
        if(sublfile == W_NULL)
        {
            sublfile = lfs_malloc(sizeof(w_listfile_s));
            WIND_ASSERT_BREAK(sublfile != W_NULL,W_ERR_MEM,"malloc listfile failed");
            sub->fileobj = sublfile;
            lfile = (w_listfile_s *)dir->fileobj;
            wind_memcpy(&sublfile->info,&lfile->info,sizeof(lfile_info_s));
            err = fileinfo_get_headchild(&sublfile->info,dir->vfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get fileinfo failed");
        }
        else
        {
            err = fileinfo_get_next(&sublfile->info,dir->vfs->blkdev);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"get fileinfo failed");
        }
        WIND_ASSERT_BREAK(sublfile->info.magic == LISTFILE_MAGIC,W_ERR_INVALID,"invalid listfile dound");
        sub->fileobj = sublfile;
        sub->obj.magic = WIND_FILE_MAGIC;
        
        if(sub->obj.name != W_NULL)
            wind_free(sub->obj.name);
        sub->obj.name = wind_salloc(sublfile->info.name);
        sub->isdir = LFILE_IS_DIR(sublfile->info.attr)?1:0;
        
        if(sub->fullpath)
            wind_filepath_release(sub->fullpath);
        sub->fullpath = wind_filepath_generate(dir->fullpath,sub->obj.name,sub->isdir);
        
        if(sub->realpath)
            wind_filepath_release(sub->realpath);
        sub->realpath = wind_filepath_generate(dir->realpath,sub->obj.name,sub->isdir);
        
        sub->vfs = dir->vfs;
        
    }while(0);

    return err;
}

static w_err_t listfs_op_seek(w_file_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return listfile_seek((w_listfile_s *)file->fileobj,offset);
}

static w_err_t listfs_op_rename(w_file_s* file,char *newname)
{
    w_err_t err;
    w_int32_t len;
    w_listfile_s *lfile;
    char *oldname = W_NULL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname[0] != 0,W_ERR_INVALID);
    len = wind_strlen(newname);
    WIND_ASSERT_RETURN(len < LFILE_NAME_LEN,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        lfile = (w_listfile_s *)file->fileobj;
        oldname = wind_salloc(lfile->info.name);
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
