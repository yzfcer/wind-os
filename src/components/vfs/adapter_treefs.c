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
//#include "adapter_treefs.h"
#include "wind_file.h"
#include "treefs.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#if WIND_FS_SUPPORT
static w_err_t treefs_op_init(w_fs_s *fs)
{
    treefs_format();
    return W_ERR_OK;
}

static w_err_t treefs_op_format(w_fs_s *fs)
{
    return W_ERR_OK;
}

static w_err_t treefs_op_open(w_file_s *file,w_uint16_t fmode)
{
    treefile_s *tfile;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    tfile = treefile_open(file->path,fmode);
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
    return treefile_close((treefile_s *)file->fileobj);
}

static w_err_t treefs_op_rmfile(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_rm((treefile_s *)file->fileobj);
}

static char *treefs_op_subfile(w_file_s* dir,w_int32_t index)
{
    treefile_s *tfile;
    w_int32_t len;
    WIND_ASSERT_RETURN(dir->subname != W_NULL,W_NULL);
    tfile = treefile_readdir((treefile_s *)dir->fileobj,index);
    if(tfile == W_NULL)
        return W_NULL;
    len = wind_strlen(tfile->filename)+2;
    if(len >= WFILE_NAME_LEN)
    {
        wind_error("file name:\"%s\" is too long.",tfile->filename);
        return W_NULL;
    }
    wind_strcpy(dir->subname,tfile->filename);
    if(tfile->isdir)
    {
        dir->subname[len-2] = '/';
        dir->subname[len-1] = 0;
    }
    return dir->subname;
}

static w_err_t treefs_op_seek(w_file_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_seek((treefile_s *)file->fileobj,offset);
}

static w_err_t treefs_op_rename(w_file_s* file,char *newname)
{
    treefile_s *tfile;
    char *name;
    w_int32_t len;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    tfile = (treefile_s *)file->fileobj;
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
    return treefile_ftell((treefile_s *)file->fileobj);
}

static w_int32_t treefs_op_read(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_read((treefile_s *)file->fileobj,buff,size);
}

static w_int32_t treefs_op_write(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return treefile_write((treefile_s *)file->fileobj,buff,size);
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

w_fs_s fs_treefs[1] = 
{
    WIND_FS_DEF(treefs,FSTYPE_TREEFS,fs_ops),
};



#endif
