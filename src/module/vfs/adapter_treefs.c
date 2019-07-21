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
static void* treefs_op_init(w_vfs_s *vfs)
{
    w_treefs_s *tfs;
    //tfs = wind_treefs_get("tfs0");
    //WIND_ASSERT_RETURN(tfs != W_NULL,W_NULL);
    
    tfs = wind_treefs_create(vfs->obj.name);
    WIND_ASSERT_RETURN(tfs != W_NULL,W_NULL);
    wind_treefs_format(tfs);
    return tfs;
}

static w_err_t treefs_op_format(w_vfs_s *vfs)
{
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    return wind_treefs_format((w_treefs_s *)vfs->fsobj);
}

static w_err_t treefs_op_open(w_file_s *file,w_uint16_t fmode)
{
    w_treefile_s *tfile;
    w_treefs_s *tfs;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    tfs = (w_treefs_s*)file->vfs->fsobj;
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_FAIL);
    tfile = treefile_open(tfs,file->path,fmode);
    if(tfile == W_NULL)
        return W_ERR_FAIL;
    if(file->isdir != tfile->isdir)
    {
        treefile_close(tfile);
        return W_ERR_INVALID;
    }
    file->fileobj = tfile;
    return W_ERR_OK;
}

static w_err_t treefs_op_close(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_close((w_treefile_s *)file->fileobj);
}

static w_err_t treefs_op_rmfile(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_rm((w_treefile_s *)file->fileobj);
}

static w_err_t treefs_op_subfile(w_file_s* dir,w_file_s* sub)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_treefile_s *tfile;
    w_treefile_s *subtfile;
    w_int32_t len;
    subtfile = (w_treefile_s *)sub->fileobj;
    do
    {
        err = W_ERR_OK;
        if(subtfile->magic != TREEFILE_MAGIC)
        {
            tfile = (w_treefile_s *)dir->fileobj;
            dnode = tfile->tree.child_list.head;
            WIND_CHECK_BREAK(dnode != W_NULL,W_ERR_NOFILE);
            subtfile = NODE_TO_TREEFILE(dnode);
            if(subtfile != W_NULL)
                break;
        }
        WIND_CHECK_BREAK(subtfile->tree.treenode.next != W_NULL,W_ERR_PTR_NULL);

        dnode = subtfile->tree.treenode.next;
        WIND_CHECK_BREAK(dnode != W_NULL,W_ERR_NOFILE);
        subtfile = NODE_TO_TREEFILE(dnode);
        WIND_ASSERT_BREAK(subtfile->magic == TREEFILE_MAGIC,W_ERR_INVALID,"invalid treefile dound");
    }while(0);
    if(err == W_ERR_OK)
    {
        sub->fileobj = subtfile;
    }


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

#if 0
w_vfs_s fs_treefs[1] = 
{
    WIND_FS_DEF(treefs,FSTYPE_TREEFS,fs_ops),
};
#endif


#endif
