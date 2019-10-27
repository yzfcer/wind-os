/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: hostfs.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.09.08
**描        述: hostfs宿主机文件系统主题功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.09.08
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.09.08
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "hostfs.h"
#include "wind_conv.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_crc32.h"
#include "wind_filepath.h"
#if WIND_HOSTFS_SUPPORT


w_err_t _wind_hostfs_mod_init(void)
{
    wind_trace_enter();
    return W_ERR_OK;
}

void *hostfs_mem_malloc(w_int32_t size)
{
    void *ptr;
    wind_trace("hostfs_mem_malloc:0x%08x,%d",ptr,size);
    ptr = wind_alloc(size,HP_ALLOCID_HOSTFS);
    if(ptr)
        wind_memset(ptr,0,size);
    return ptr;
}

w_err_t hostfs_mem_free(void *ptr)
{
    wind_trace("hostfs_mem_free:0x%08x",ptr);
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}


w_err_t hostfs_init(w_hostfs_s *hfs,char *dir_prefix)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir_prefix != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir_prefix[0] != 0,W_ERR_INVALID);
    hfs->magic = HOSTFS_MAGIC;
    hfs->dir_prefix = wind_salloc(dir_prefix,HP_ALLOCID_HOSTFS);
    WIND_ASSERT_RETURN(hfs->dir_prefix != W_NULL,W_ERR_MEM);
    return W_ERR_OK;
}



w_err_t hostfs_deinit(w_hostfs_s *hfs)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    if(hfs->dir_prefix != W_NULL)
        wind_free(hfs->dir_prefix);
    return W_ERR_OK;
}

w_err_t hostfs_match(w_blkdev_s *blkdev)
{
    return W_ERR_FAIL;
}

static w_err_t host_file_destroy(w_hostfile_s *hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    hfile->magic = 0;
    if(hfile->subhfile != W_NULL)
        host_file_destroy(hfile->subhfile);
    if(hfile->path  != W_NULL)
        hostfs_mem_free(hfile->path);
    if(hfile->name != W_NULL)
        hostfs_mem_free(hfile->name);
    if(hfile->fileinfo != W_NULL)
        hostfs_mem_free(hfile->fileinfo);
    hostfs_mem_free(hfile);
    return W_ERR_OK;
}

static w_hostfile_s*   host_file_create(char *path,w_uint8_t mode,w_uint8_t isdir,FILE *fd)
{
    w_err_t err;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    WIND_ASSERT_RETURN(path != W_NULL, (w_hostfile_s *)W_NULL);
    do
    {
        err = W_ERR_OK;
        hfile = (w_hostfile_s *)hostfs_mem_malloc(sizeof(w_hostfile_s));
        WIND_ASSERT_BREAK(hfile != W_NULL, W_ERR_MEM, "alloc hfile failed");
        hfile->magic = HOSTFILE_MAGIC;
        hfile->name = wind_filepath_get_filename(path);
        WIND_ASSERT_BREAK(hfile->name != W_NULL, W_ERR_MEM, "alloc hfile name failed");
        hfile->fd = fd;
        hfile->subfd = (FILE *)W_NULL;
        hfile->isdir = isdir;
        hfile->mode = mode;
        hfile->path = (char*)wind_salloc(path, HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(hfile->path != W_NULL, W_ERR_MEM, "alloc path failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
            host_file_destroy(hfile);
        hfile = (w_hostfile_s *)W_NULL;
    }
    return hfile;
}

static w_hostfile_s*   host_file_open_exist(char *path,w_uint8_t mode)
{
    w_err_t err;
    FILE *fd;
    w_hostfile_s *hfile = W_NULL;
    hfileattr_e attr;
    w_uint8_t isdir;
    do
    {
        err = W_ERR_OK;
        attr = host_file_type(path);
        WIND_ASSERT_BREAK(attr != HFILE_TYPE_ERROR, W_ERR_FAIL, "get hfile type failed");
        isdir = (attr == HFILE_TYPE_DIR)?1:0;
        if(!isdir)
        {
            fd = fopen(path,"rb+");
            WIND_ASSERT_BREAK(fd != W_NULL,W_ERR_FAIL,"open hfile failed");
        }
        
        hfile = host_file_create(path,mode,isdir,fd);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create hfile obj failed");
        
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
            host_file_destroy(hfile);
        hfile = (w_hostfile_s *)W_NULL;
    }    
    return hfile;
    
}

static w_hostfile_s*   host_file_open_create(char *path,w_uint8_t mode)
{
    w_int8_t isdir;
    w_int32_t res;
    w_err_t err;
    FILE *fd = (FILE *)W_NULL;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    WIND_ASSERT_RETURN(path != W_NULL, (w_hostfile_s *)W_NULL);
    do
    {
        err = W_ERR_OK;
        if(!hfile->isdir)
        {
            fd = fopen(path,"wb+");
            WIND_ASSERT_BREAK(fd != W_NULL, W_ERR_FAIL, "create hfile failed");
        }
        else
        {
            res = _mkdir(path);
            WIND_ASSERT_BREAK(res == 0, W_ERR_FAIL, "make dir failed");
            fd = (FILE *)W_NULL;
        }
        isdir = wind_filepath_isdir(path)?1:0;
        hfile = host_file_create(path,mode,isdir,fd);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create hfile obj failed");
    }while(0);
    
    if(err != W_ERR_OK)
    {
        if(hfile)
            host_file_destroy(hfile);
        else if(fd)
            fclose(fd);
        hfile = (w_hostfile_s *)W_NULL;
    }
    return hfile;
}


w_hostfile_s* hostfile_open(w_hostfs_s *hfs,const char *path,w_uint8_t mode)
{
    w_err_t err,exist;
    w_uint8_t isdir;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    char *fullpath = (char *)W_NULL;
    do
    {
        err = W_ERR_OK;
        
        exist = hostfile_existing(hfs,path);
        WIND_ASSERT_BREAK((exist == W_ERR_OK) || ((mode & HFMDOE_CRT) != 0),W_ERR_NOFILE,"hfile is not exist");
        isdir = wind_filepath_isdir((char*)path);
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        if(exist != W_ERR_OK)
            hfile = host_file_open_create((char*)path,mode);
        else
            hfile = host_file_open_exist((char*)path,mode);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"open hfile failed");
        hfile = (w_hostfile_s *)hostfs_mem_malloc(sizeof(w_hostfile_s));
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_MEM,"alloc hostfile failed");
        hfile->magic = HOSTFILE_MAGIC;
        hfile->hfs = hfs;
        //hostfile->hfile = hfile;
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    if(err != W_ERR_OK)
    {
        if(hfile)
            wind_free(hfile);
        hfile = (w_hostfile_s *)W_NULL;
    }
    return hfile;
}



hfileattr_e host_file_type(char *path)
{
    char* fileName = path; 
    struct _stat stat; 
    int result; 
    result = _stat(fileName, &stat);
    if(result != 0)
        return HFILE_TYPE_ERROR;
    if(_S_IFDIR & stat.st_mode)
        return HFILE_TYPE_DIR;
    else if(_S_IFREG & stat.st_mode)
        return HFILE_TYPE_FILE;
    return HFILE_TYPE_ERROR;
}


w_err_t hostfile_close(w_hostfile_s* hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL,W_ERR_INVALID);

    if(hfile->isdir == 0)
        fclose(hfile->fd);
    else if(hfile->subfd >= 0)
        _findclose(hfile->subfd);
    return host_file_destroy(hfile);
}

w_err_t hostfile_remove(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    w_uint8_t isdir;
    hfileattr_e attr;
    char *fullpath = (char*)W_NULL;
    do
    {
        err = W_ERR_OK;
        isdir = wind_filepath_isdir((char*)path);
        fullpath = wind_filepath_generate(hfs->dir_prefix,(char*)path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        attr = host_file_type((char*)path);
        if(attr == HFILE_TYPE_DIR)
            _rmdir(path);
        else if(attr == HFILE_TYPE_FILE)
            remove(path);
        else
            return W_ERR_NOFILE;
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"remove hfile failed");
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err;
}


w_err_t hostfile_set_attr(w_hostfile_s* hfile,w_uint8_t attr)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    hfile->attr = attr;
    return W_ERR_OK;
}

w_err_t hostfile_get_attr(w_hostfile_s* hfile,w_uint8_t *attr)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    *attr = hfile->attr;
    return W_ERR_OK;
}


w_bool_t hostfile_existing(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    FILE *fd;
    w_uint8_t isdir;
    char *fullpath = W_NULL;
    do
    {
        err = W_ERR_OK;
        isdir = wind_filepath_isdir(path);
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        fd = fopen(path,"r");
        WIND_CHECK_BREAK(fd != W_NULL, W_ERR_FAIL);
        fclose(fd);
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err == W_ERR_OK ? W_TRUE : W_FALSE;
}

w_err_t hostfile_seek(w_hostfile_s* hfile,w_int32_t offset)
{
    w_err_t err;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        fseek(hfile->fd,offset,SEEK_SET);
    }while(0);
    return err;
}

w_int32_t hostfile_ftell(w_hostfile_s* hfile)
{
    w_err_t err;
    w_int32_t offset = -1;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        offset = ftell(hfile->fd);
        WIND_ASSERT_BREAK(offset >= 0,err,"tell hfile failed");
    }while(0);
    return offset;
}



w_int32_t hostfile_read(w_hostfile_s* hfile,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t rdsize = 0;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL, W_ERR_INVALID);
    rdsize = fread(buff,1,size,hfile->fd);
    return rdsize;
}


w_int32_t hostfile_write(w_hostfile_s* hfile,w_uint8_t *buff,w_int32_t size)
{
    w_int32_t wrsize = 0;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL, W_ERR_INVALID);
    wrsize = fwrite(buff,1,size,hfile->fd);
    return wrsize;
}

static w_hostfile_s *do_host_file_readdir(w_hostfile_s *hfile)
{
    w_err_t err;
    w_int32_t res;
    w_hostfile_s *subhfile;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 1,W_NULL);
    WIND_ASSERT_RETURN(hfile->path != W_NULL,W_NULL);
    do 
    {
        err = W_ERR_OK;
        if(hfile->fileinfo == W_NULL)
        {
            err = W_ERR_OK;
            hfile->fileinfo = hostfs_mem_malloc(sizeof(hfileinfo_s));
            WIND_ASSERT_BREAK(hfile->fileinfo != W_NULL, W_ERR_MEM, "alloc fileinfo failed");
            hfile->subfd = _findfirst(hfile->path,hfile->fileinfo);
        }
        else
        {
            WIND_ASSERT_BREAK(hfile->subfd >= 0, W_ERR_INVALID, "handle is invalid");
            res = _findnext(hfile->subfd,hfile->fileinfo);
            if(res < 0)
            {
                hostfs_mem_free(hfile->fileinfo);
                hfile->fileinfo = W_NULL;
            }
        }
        WIND_CHECK_BREAK(hfile->fileinfo != W_NULL, W_ERR_FAIL);
        if(hfile->subhfile == W_NULL)
        {
            hfile->subhfile = hostfs_mem_malloc(sizeof(w_hostfile_s));
            WIND_ASSERT_BREAK(hfile->subhfile != W_NULL, W_ERR_MEM, "alloc sub hfile failed");
        }
        subhfile = hfile->subhfile;
        
    }while(0);
    if(err == W_ERR_OK)
        return hfile->subhfile;
    return W_NULL;

}
 


w_err_t hostfile_readdir(w_hostfile_s* dir,w_hostfile_s** sub)
{
    w_err_t err;
    w_hostfile_s *hfile;
    w_hostfile_s *tmp = *sub;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tmp != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        hfile = do_host_file_readdir(dir);
        WIND_CHECK_BREAK(hfile != W_NULL,W_ERR_FAIL);
        tmp->magic = HOSTFILE_MAGIC;
        tmp->hfs = dir->hfs;
    }while(0);
    return err;
}


w_uint32_t host_file_size(w_hostfile_s *hfile)
{
    w_uint32_t len;
	w_int32_t offset;
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
	offset = ftell(hfile->fd);
    fseek(hfile->fd,0,SEEK_END);
    len = ftell(hfile->fd);
	fseek(hfile->fd,offset,SEEK_SET);
	return len;
}


w_err_t hostfile_fgets(w_hostfile_s* hfile,char *buff, w_int32_t maxlen)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    return W_ERR_OK;
}

w_err_t hostfile_fputs(w_hostfile_s* hfile,char *buff)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    return W_ERR_OK;
}

#endif

