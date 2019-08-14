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
#include "listfile.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#if WIND_TREEFS_SUPPORT
static void* listfs_op_init(w_vfs_s *vfs)
{
    w_listfs_s *lfs;
    //lfs = wind_listfs_get("lfs0");
    //WIND_ASSERT_RETURN(lfs != W_NULL,W_NULL);
    
    lfs = wind_listfs_create(vfs->obj.name);
    WIND_ASSERT_RETURN(lfs != W_NULL,W_NULL);
    wind_listfs_format(lfs);
    return lfs;
}

static w_err_t listfs_op_format(w_vfs_s *vfs)
{
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    return wind_listfs_format((w_listfs_s *)vfs->fsobj);
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
    file->isdir = lfile->isdir;
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
    return listfile_rm((w_listfile_s *)file->fileobj);
}

static w_err_t listfs_op_subfile(w_file_s* dir,w_file_s* sub)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_listfile_s *lfile;
    w_listfile_s *subtfile;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        subtfile = (w_listfile_s *)sub->fileobj;
        if(subtfile == W_NULL)
        {
            lfile = (w_listfile_s *)dir->fileobj;
            dnode = lfile->tree.child_list.head;
            WIND_CHECK_BREAK(dnode != W_NULL,W_ERR_NOFILE);
            subtfile = NODE_TO_TREEFILE(dnode);
        }
        else
        {
            WIND_CHECK_BREAK(subtfile->tree.treenode.next != W_NULL,W_ERR_PTR_NULL);
            dnode = subtfile->tree.treenode.next;
            WIND_CHECK_BREAK(dnode != W_NULL,W_ERR_NOFILE);
            subtfile = NODE_TO_TREEFILE(dnode);
            WIND_ASSERT_BREAK(subtfile->magic == TREEFILE_MAGIC, W_ERR_INVALID, "error listfile object");
        }
        WIND_ASSERT_BREAK(subtfile->magic == TREEFILE_MAGIC,W_ERR_INVALID,"invalid listfile dound");
        sub->fileobj = subtfile;
        sub->obj.magic = WIND_FILE_MAGIC;
        if(sub->obj.name != W_NULL)
            wind_free(sub->obj.name);
        sub->obj.name = wind_salloc(subtfile->filename);
        sub->isdir = subtfile->isdir;
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
    w_listfile_s *lfile;
    char *name;
    w_int32_t len;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    lfile = (w_listfile_s *)file->fileobj;
    len = wind_strlen(newname) + 1;
    name = wind_malloc(len);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_MEM);
    wind_free(lfile->filename);
    lfile->filename = name;
    return W_ERR_OK;
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
