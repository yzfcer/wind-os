/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: adapter_treefs.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: treefs文件系统适配层接口
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
#include "treefs.h"
#include "treefile.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#if WIND_TREEFS_SUPPORT

static w_err_t treefs_op_opsinit(void)
{
    w_err_t err;
    err = _wind_treefs_mod_init();
    return err;
}


static void* treefs_op_init(w_vfs_s *vfs)
{
    w_treefs_s *tfs;
    tfs = wind_treefs_create(vfs->obj.name);
    WIND_ASSERT_RETURN(tfs != W_NULL,W_NULL);
    wind_treefs_format(tfs);
    return tfs;
}

static w_err_t treefs_op_deinit(w_vfs_s *vfs)
{
    w_err_t err;
    w_treefs_s *tfs;
    tfs = (w_treefs_s *)vfs->fsobj;
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_PTR_NULL);
    err = wind_treefs_destroy(tfs);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return W_ERR_OK;
}


static w_err_t treefs_op_format(w_vfs_s *vfs)
{
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    return wind_treefs_format((w_treefs_s *)vfs->fsobj);
}
w_err_t treefs_op_matchfs(char *devname)
{
    return W_ERR_FAIL;
}

static w_err_t treefs_op_open(w_file_s *file,w_uint8_t fmode)
{
    w_treefile_s *tfile;
    w_treefs_s *tfs;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    tfs = (w_treefs_s*)file->vfs->fsobj;
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_FAIL);
    tfile = treefile_open(tfs,file->realpath,fmode);
    if(tfile == W_NULL)
        return W_ERR_FAIL;
    
    file->fileobj = tfile;
    file->isdir = tfile->isdir;
    file->offset = 0;
    
    return W_ERR_OK;
}

static w_err_t treefs_op_close(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_close((w_treefile_s *)file->fileobj);
}

static w_err_t treefs_op_remove(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_remove((w_treefile_s *)file->fileobj);
}

static w_err_t treefs_op_readdir(w_file_s* dir,w_file_s* sub)
{
    w_err_t err;
    w_int32_t len;
    w_treefile_s *subtfile;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;

        subtfile = (w_treefile_s *)sub->fileobj;
        err = treefile_readdir((w_treefile_s *)dir->fileobj,(w_treefile_s **)&subtfile);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
        WIND_ASSERT_BREAK(subtfile != W_NULL,W_ERR_FAIL,"get subtfile failed");
        WIND_ASSERT_BREAK(subtfile->magic == TREEFILE_MAGIC,W_ERR_INVALID,"invalid treefile dound");
        sub->fileobj = subtfile;
        sub->obj.magic = WIND_FILE_MAGIC;
        if(sub->obj.name != W_NULL)
            wind_free(sub->obj.name);
        sub->obj.name = wind_salloc(subtfile->filename,HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(sub->obj.name != W_NULL,W_ERR_MEM,"malloc filename failed");
        sub->isdir = subtfile->isdir;
        if(sub->fullpath)
            wind_filepath_release(sub->fullpath);
        sub->fullpath = wind_filepath_generate(dir->fullpath,sub->obj.name,sub->isdir);
        WIND_ASSERT_BREAK(sub->fullpath != W_NULL,W_ERR_MEM,"malloc fullpath failed");

        len = wind_strlen(dir->vfs->mount_path);
        WIND_ASSERT_BREAK(len >= 1,W_ERR_INVALID,"get mount path lenth failed");
        sub->realpath = &sub->fullpath[len - 1];
        sub->vfs = dir->vfs;
        
    }while(0);

    return err;
}

static w_err_t treefs_op_seek(w_file_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_seek((w_treefile_s *)file->fileobj,offset);
}

static w_err_t treefs_op_rename(w_file_s* file,char *newname)
{
    w_treefile_s *tfile;
    char *name;
    w_int32_t len;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    tfile = (w_treefile_s *)file->fileobj;
    len = wind_strlen(newname) + 1;
    name = wind_malloc(len);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_MEM);
    wind_free(tfile->filename);
    tfile->filename = name;
    return W_ERR_OK;
}

static w_int32_t treefs_op_ftell(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_ftell((w_treefile_s *)file->fileobj);
}

static w_int32_t treefs_op_read(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_read((w_treefile_s *)file->fileobj,buff,size);
}

static w_int32_t treefs_op_write(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_write((w_treefile_s *)file->fileobj,buff,size);
}
static w_err_t treefs_op_fgets(w_file_s* file,char *buff, w_int32_t maxlen)
{
    return W_ERR_FAIL;
}
static w_err_t treefs_op_fputs(w_file_s* file,char *buff)
{
    return W_ERR_FAIL;
}

FS_OPS_DEF(treefs);

#endif
