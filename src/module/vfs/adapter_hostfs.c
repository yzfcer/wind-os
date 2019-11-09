/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: adapter_hostfs.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: hostfs文件系统适配层接口
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
#include "hostfs.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#if WIND_HOSTFS_SUPPORT
static w_err_t hostfs_op_opsinit(void)
{
    w_err_t err;
    err = _wind_hostfs_mod_init();
    return err;
}

static void* hostfs_op_init(w_vfs_s *vfs)
{
    w_err_t err;
    w_hostfs_s *hfs = W_NULL;
    char *prefix = W_NULL;
    w_bool_t isexist;
    WIND_ASSERT_RETURN(vfs != W_NULL, W_NULL);
    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(vfs->fsobj == W_NULL,W_ERR_OK,"hostfs has been init");
        hfs = hostfs_mem_malloc(sizeof(w_hostfs_s));
        WIND_ASSERT_BREAK(hfs != W_NULL,W_ERR_MEM,"alloc fsobj failed");
        wind_memset(hfs,0,sizeof(w_hostfs_s));
        err = hostfs_init(hfs,vfs->usr_arg);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"host fs init failed");
        isexist = hostfile_existing(hfs,"/");
        WIND_ASSERT_BREAK(isexist == W_TRUE,err,"dirent is not exist");
        vfs->fsobj = hfs;
    }while(0);
    
    if(err != W_ERR_OK)
    {
        if(prefix != W_NULL)
            wind_free(prefix);
        if(hfs != W_NULL)
            hostfs_mem_free(hfs);
        vfs->fsobj = W_NULL;
    }
    return vfs->fsobj;
}

static w_err_t hostfs_op_deinit(w_vfs_s *vfs)
{
    w_err_t err;
    w_hostfs_s *hfs;
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->fsobj != W_NULL,W_ERR_PTR_NULL);
    do 
    {
        err = W_ERR_OK;
        hfs = (w_hostfs_s *)vfs->fsobj;
        err = hostfs_deinit(hfs);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"hostfs deinit failed");
        hostfs_mem_free(vfs->fsobj);
        vfs->fsobj = W_NULL;
    }while(0);
    return err;
}


static w_err_t hostfs_op_format(w_vfs_s *vfs)
{
    return W_ERR_FAIL;
}

w_err_t hostfs_op_matchfs(char *devname)
{
    return W_ERR_FAIL;
}


static w_err_t hostfs_op_open(w_file_s *file,w_uint8_t fmode)
{
    w_hostfile_s *hfile;
    w_hostfs_s *hfs;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    hfs = (w_hostfs_s*)file->vfs->fsobj;
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_FAIL);

    hfile = hostfile_open(hfs,file->realpath,fmode);
    if(hfile == W_NULL)
        return W_ERR_FAIL;
    
    file->fileobj = hfile;
    file->isdir = IS_HFILE_ATTR_DIR(hfile->attr)?1:0;
    file->offset = 0;
    
    return W_ERR_OK;
}

static w_err_t hostfs_op_close(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return hostfile_close((w_hostfile_s *)file->fileobj);
}

static w_err_t hostfs_op_remove(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
	return hostfile_remove((w_hostfs_s *)file->vfs->fsobj,file->realpath);
}

static w_err_t hostfs_op_readdir(w_file_s* dir,w_file_s* sub)
{
    w_err_t err;
    w_int32_t len;
    w_hostfile_s *subhfile = (w_hostfile_s *)W_NULL;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        subhfile = (w_hostfile_s *)sub->fileobj;
        err = hostfile_readdir((w_hostfile_s *)dir->fileobj,&subhfile);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
        WIND_ASSERT_BREAK(subhfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID,"invalid hostfile dound");
        sub->fileobj = subhfile;
        sub->obj.magic = WIND_FILE_MAGIC;
        
        if(sub->obj.name != W_NULL)
            wind_free((void*)sub->obj.name);
        sub->obj.name = (const char*)wind_salloc(subhfile->name,HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(sub->obj.name != W_NULL,W_ERR_MEM,"malloc filename failed");
        sub->isdir = IS_HFILE_ATTR_DIR(subhfile->attr)?1:0;
        
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

static w_err_t hostfs_op_seek(w_file_s* file,w_uint32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return hostfile_seek((w_hostfile_s *)file->fileobj,offset);
}

static w_err_t hostfs_op_rename(w_file_s* file,char *newname)
{
    w_err_t err;
    w_int32_t len;
    w_hostfile_s *hfile;
    char *oldname = (char *)W_NULL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname[0] != 0,W_ERR_INVALID);
    len = wind_strlen(newname);
    WIND_ASSERT_RETURN(len < HFILE_NAME_LEN,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        hfile = (w_hostfile_s *)file->fileobj;
        oldname = (char *)wind_salloc(hfile->name,HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(oldname != W_NULL,W_ERR_MEM,"malloc filename failed");
    }while(0);
    if(oldname != W_NULL)
        wind_free(oldname);
    return err;
}

static w_int32_t hostfs_op_ftell(w_file_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return hostfile_ftell((w_hostfile_s *)file->fileobj);
}

static w_int32_t hostfs_op_read(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return hostfile_read((w_hostfile_s *)file->fileobj,buff,size);
}

static w_int32_t hostfs_op_write(w_file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    return hostfile_write((w_hostfile_s *)file->fileobj,buff,size);
}


FS_OPS_DEF(hostfs);

#endif
