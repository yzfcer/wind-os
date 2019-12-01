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
//#include "windows_filepath.h"
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
#include "windows_filepath.h"
#endif
#if WIND_HOSTFS_SUPPORT


w_err_t _wind_hostfs_mod_init(void)
{
    wind_trace_enter();
    return W_ERR_OK;
}

void *hostfs_mem_malloc(w_int32_t size)
{
    void *ptr;
    wind_trace("hostfs_mem_malloc:%d",size);
    ptr = wind_alloc(size,HP_ALLOCID_HOSTFS);
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
    {
        host_file_destroy(hfile->subhfile);
        hostfs_mem_free(hfile->subhfile);
    }
        
    if(hfile->path  != W_NULL)
        hostfs_mem_free(hfile->path);
    if(hfile->name != W_NULL)
        hostfs_mem_free(hfile->name);
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
        wind_memset(hfile,0,sizeof(w_hostfile_s));
        hfile->magic = HOSTFILE_MAGIC;
        hfile->name = windows_filepath_get_filename(path);
        WIND_ASSERT_BREAK(hfile->name != W_NULL, W_ERR_MEM, "alloc hfile name failed");
        hfile->mode = mode;
        hfile->attr = isdir?HFILE_ATTR_DIR:0;
        hfile->isdir = isdir;
        hfile->has_sub = 0;
        hfile->subhfile = W_NULL;
        hfile->fd = fd;
#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
        hfile->handle = -1;
#endif
        hfile->path = (char*)wind_salloc(path, HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(hfile->path != W_NULL, W_ERR_MEM, "alloc path failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
        {
            if(hfile->name)
                windows_filepath_release(hfile->name);
            if(hfile->path)
                wind_free(hfile->path);
            hostfs_mem_free(hfile);
        }
        hfile = (w_hostfile_s *)W_NULL;
    }
    return hfile;
}

static w_hostfile_s*   host_file_open_exist(char *path,w_uint8_t mode)
{
    errno_t errno;
    w_err_t err;
    FILE *fd;
    w_int32_t len;
    w_hostfile_s *hfile = W_NULL;
    char *realpath = (char*)W_NULL;
    hfileattr_e attr;
    w_uint8_t isdir;
    do
    {
        err = W_ERR_OK;
        attr = windows_file_type(path);
        WIND_ASSERT_BREAK(attr != HFILE_TYPE_ERROR, W_ERR_FAIL, "get hfile type failed");
        isdir = (attr == HFILE_TYPE_DIR)?1:0;
        if(!isdir)
        {
            realpath = windows_filepath_remove_tail(path);
            WIND_ASSERT_BREAK(realpath != W_NULL,W_ERR_MEM,"alloc realpath failed");
            errno = fopen_s(&fd,realpath,"rb+");
            WIND_ASSERT_BREAK(errno == 0,W_ERR_FAIL,"open hfile failed");
            WIND_ASSERT_BREAK(fd != W_NULL,W_ERR_FAIL,"open hfile failed");
        }
        else
            fd = W_NULL;
        
        hfile = host_file_create(path,mode,isdir,fd);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create hfile obj failed");
        
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
            host_file_destroy(hfile);
        hfile = (w_hostfile_s *)W_NULL;
    }
    if(realpath != W_NULL)
        wind_free(realpath);
    return hfile;
    
}

static w_hostfile_s*   host_file_open_create(char *path,w_uint8_t mode)
{
    errno_t errno;
    w_int8_t isdir;
    w_int32_t res;
    w_err_t err;
    char *realpath = (char*)W_NULL;
    FILE *fd = (FILE *)W_NULL;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    WIND_ASSERT_RETURN(path != W_NULL, (w_hostfile_s *)W_NULL);
    do
    {
        err = W_ERR_OK;
#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
        isdir = windows_filepath_isdir(path);
#else
        isdir = windows_filepath_isdir(path);
#endif

        if(!isdir)
        {
            realpath = windows_filepath_remove_tail(path);
            WIND_ASSERT_BREAK(realpath != W_NULL,W_ERR_MEM,"alloc realpath failed");
            errno = fopen_s(&fd,realpath,"w+");
            WIND_ASSERT_BREAK(errno == 0,W_ERR_FAIL,"open hfile failed");
            WIND_ASSERT_BREAK(fd != W_NULL, W_ERR_FAIL, "create hfile failed");
        }
        else
        {
            res = _mkdir(path);
            WIND_ASSERT_BREAK(res == 0, W_ERR_FAIL, "make dir failed");
            fd = (FILE *)W_NULL;
        }
        //isdir = windows_filepath_isdir(path)?1:0;
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
    w_err_t err;
    w_bool_t exist;
    w_uint8_t isdir;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    char *fullpath = (char *)W_NULL;
    do
    {
        err = W_ERR_OK;
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
        isdir = windows_filepath_isdir(path);
        fullpath = windows_filepath_generate(hfs->dir_prefix,path,isdir);
#else
        isdir = windows_filepath_isdir(path);
        fullpath = windows_filepath_generate(hfs->dir_prefix,path,isdir);
#endif
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        exist = hostfile_existing(hfs,path);
        WIND_CHECK_BREAK((exist == W_TRUE) || ((mode & HFMODE_CRT) != 0),W_ERR_NOFILE);
        if(exist != W_TRUE)
            hfile = host_file_open_create((char*)fullpath,mode);
        else
            hfile = host_file_open_exist((char*)fullpath,mode);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"open hfile failed");
        hfile->hfs = hfs;
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




w_err_t hostfile_close(w_hostfile_s* hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);

    if(hfile->isdir == 0)
        fclose(hfile->fd);
    else if(hfile->handle >= 0)
        _findclose(hfile->handle);
    return host_file_destroy(hfile);
}

w_err_t do_remove_windows_dir(char *fullpath)
{
    w_int32_t len;
    w_err_t err;
    char *cmd = (char *)W_NULL;
    WIND_ASSERT_RETURN(fullpath != W_NULL,W_ERR_PTR_NULL);
    
    wind_trace("fullpath=%s",fullpath);
    do
    {
        err = W_ERR_OK;
        len = wind_strlen(fullpath) + 1;
        wind_strrpc(fullpath,'/','\\');
        len += sizeof("rd /s /q ");
        cmd = (char *)hostfs_mem_malloc(len);
        WIND_ASSERT_BREAK(cmd != W_NULL,W_ERR_MEM,"alloc cmd failed");
        wind_strcpy(cmd,"rd /s /q ");
        wind_strcat(cmd,fullpath);
        wind_printf(cmd);
        system(cmd);
    }while(0);
    if(cmd != W_NULL)
        hostfs_mem_free(cmd);
    return err;
}


w_err_t hostfile_remove(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    w_uint8_t isdir;
    hfileattr_e attr;
    w_int32_t len;
    
    char *fullpath = (char*)W_NULL;
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
        isdir = windows_filepath_isdir((char*)path);
        fullpath = windows_filepath_generate(hfs->dir_prefix,path,isdir);
#else
        isdir = windows_filepath_isdir((char*)path);
        fullpath = windows_filepath_generate(hfs->dir_prefix,path,isdir);
#endif
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        attr = windows_file_type((char*)fullpath);
        if(attr == HFILE_TYPE_DIR)
        {
            len = wind_strlen(fullpath);
            if(fullpath[len - 1] == '/')
                fullpath[len - 1] = 0;
            err = do_remove_windows_dir(fullpath);
            WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"rmdir failed,result:%d",err);
        }
            
        else if(attr == HFILE_TYPE_FILE)
            remove(fullpath);
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
    w_int32_t res,len;
    w_uint8_t isdir;
    //char *newpath = (char*)W_NULL;;
    char *fullpath = (char*)W_NULL;
    WIND_ASSERT_RETURN(hfs != W_NULL,W_FALSE);
    WIND_ASSERT_RETURN(path != W_NULL,W_FALSE);
    do
    {
        err = W_ERR_OK;
        isdir = 0;
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
        if(path[0] != 0)
            isdir = windows_filepath_isdir(path);
        fullpath = windows_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        len = wind_strlen(fullpath);
        if(fullpath[len - 1] == '/')
            fullpath[len - 1] = 0;
        res = _access(fullpath,0);
        WIND_CHECK_BREAK(res == 0, W_ERR_FAIL);
#else
        if(path[0] != 0)
            isdir = windows_filepath_isdir(path);
        fullpath = windows_filepath_generate(hfs->dir_prefix,path,isdir);
        err = W_ERR_NOT_SUPPORT;
#endif
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err == W_ERR_OK ? W_TRUE : W_FALSE;
}

w_err_t hostfile_seek(w_hostfile_s* hfile,w_int32_t offset)
{
    w_err_t err;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
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
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
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
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
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
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
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
    char *path = W_NULL;
    w_hostfile_s *subhfile = (w_hostfile_s *)W_NULL;
    WIND_ASSERT_RETURN(hfile != W_NULL,(w_hostfile_s *)W_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,(w_hostfile_s *)W_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 1,(w_hostfile_s *)W_NULL);
    WIND_ASSERT_RETURN(hfile->path != W_NULL,(w_hostfile_s *)W_NULL);
    do 
    {
        err = W_ERR_OK;
        path = windows_filepath_generate(hfile->path,"/*",0);
        WIND_ASSERT_BREAK(path != W_NULL,W_ERR_MEM,"generate path failed");
        if(!hfile->has_sub)
        {
            wind_memset(&hfile->fileinfo,0,sizeof(_finddata_t));
            hfile->handle = _findfirst(path,&hfile->fileinfo);
            WIND_CHECK_BREAK(hfile->handle >= 0,W_ERR_FAIL);
            hfile->has_sub = 1;
        }
        else
        {
            WIND_ASSERT_BREAK(hfile->handle >= 0, W_ERR_INVALID, "handle is invalid");
            res = _findnext(hfile->handle,&hfile->fileinfo);
            WIND_CHECK_BREAK(res >= 0,W_ERR_FAIL)
        }
        if(hfile->subhfile == W_NULL)
        {
            subhfile = (w_hostfile_s *)hostfs_mem_malloc(sizeof(w_hostfile_s));
            WIND_ASSERT_BREAK(subhfile != W_NULL, W_ERR_MEM, "alloc sub hfile failed");
            wind_memset(subhfile,0,sizeof(w_hostfile_s));
        }
        subhfile = hfile->subhfile;
        subhfile->attr = (hfile->fileinfo.attrib & _A_SUBDIR)?HFILE_ATTR_DIR:0;
        subhfile->isdir = (subhfile->attr & HFILE_ATTR_DIR)?1:0;
        if(subhfile->name != W_NULL)
            wind_free(subhfile->name);
        subhfile->name = (char*)wind_salloc(hfile->fileinfo.name,HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(subhfile->name != W_NULL,W_ERR_MEM,"clone filename failed");
        hfile->subhfile = subhfile;
    }while(0);
    if(path != W_NULL)
        hostfs_mem_free(path);
    if(err == W_ERR_OK)
        return hfile->subhfile;
    if(subhfile)
    {
        if(subhfile->name)
            wind_free(subhfile->name);
        wind_free(subhfile);
        hfile->subhfile = (w_hostfile_s *)W_NULL;
    }
        
    return (w_hostfile_s *)W_NULL;
}
 


w_err_t hostfile_readdir(w_hostfile_s* dir,w_hostfile_s** sub)
{
    w_err_t err;
    w_hostfile_s *hfile;
    w_hostfile_s *tmp = *sub;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->magic == HOSTFILE_MAGIC,W_ERR_INVALID);

    WIND_ASSERT_RETURN(dir->hfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->mode & HFMODE_R,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_HFILE_ATTR_DIR(dir->attr) != 0,W_ERR_INVALID);

    do
    {
        err = W_ERR_OK;
        if(tmp == W_NULL)
        {
            tmp = hostfs_mem_malloc(sizeof(w_hostfile_s));
            WIND_ASSERT_BREAK(tmp != W_NULL,W_ERR_MEM,"alloc hostfile obj failed");
            wind_memset(tmp,0,sizeof(w_hostfile_s));
            *sub = tmp;
            dir->subhfile = tmp;
            tmp->magic = HOSTFILE_MAGIC;
            tmp->hfs = dir->hfs;
        }
        hfile = do_host_file_readdir(dir);
        WIND_CHECK_BREAK(hfile != W_NULL,W_ERR_FAIL);
    }while(0);
    if(err != W_ERR_OK)
    {
        if(dir->subhfile != W_NULL)
            hostfs_mem_free(dir->subhfile);
        dir->subhfile = W_NULL;
        *sub = W_NULL;
    }
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


#endif

