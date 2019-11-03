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
    return ptr;
}

w_err_t hostfs_mem_free(void *ptr)
{
    wind_trace("hostfs_mem_free:0x%08x",ptr);
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
static w_err_t hostfs_filepath_check_valid(char *path)
{
    w_int32_t i,len;
    //char chset[] = {'~','!','@','#','$','%','^','&','*','+','=','?','\t','\r','\“'};
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0,W_ERR_INVALID);
    //WIND_ASSERT_RETURN(path[0] == '/',W_ERR_INVALID);

        
    for(i = 0;i < len; i ++)
    {
        if(path[i] >= 0x2D && path[i] <= 0x39)
            continue;
        if(path[i] >= 0x41 && path[i] <= 0x5A)
            continue;
        if(path[i] >= 0x61 && path[i] <= 0x7A)
            continue;
        if((path[i] == '_')||(path[i] == ':'))
            continue;
        wind_error("invalid character:%c",path[i]);
        return W_ERR_FAIL;
    }

    len -= 1;
    for(i = 0;i < len;i ++)
    {
        if((path[i] == '/') && (path[i+1] == '/'))
        {
            wind_error("invalid path:%s",path);
            return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

static char *hostfs_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir)
{
    w_err_t err;
    char *path = W_NULL;
    w_int32_t prelen,relalen,pathlen;
    WIND_ASSERT_RETURN(pre_path != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(relative_path != W_NULL,W_NULL);
    prelen = wind_strlen(pre_path);
    WIND_ASSERT_RETURN(prelen >= 2,W_ERR_INVALID);
    WIND_ASSERT_RETURN(pre_path[1] == ':',W_ERR_INVALID);
    
    
    relalen = wind_strlen(relative_path);
    prelen = wind_strlen(pre_path);
    pathlen = prelen + relalen+3;
    path = wind_alloc(pathlen,HP_ALLOCID_VFS);
    wind_strcpy(path,pre_path);
    if(pre_path[prelen-1] != '/')
    {
        path[prelen] = '/';
        prelen ++;
    }
    if((relative_path[0] == '/'))
    {
        path[prelen-1] = 0;
        prelen --;
    }
    wind_strcat(path,relative_path);
    
    
    pathlen = wind_strlen(path);
    if(isdir && (path[pathlen-1] != '/'))
        path[pathlen] = '/';
    err = hostfs_filepath_check_valid(path);
    if(err != W_ERR_OK)
    {
        wind_free(path);
        path = W_NULL;
    }
    return path;
}
#endif

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
        hfile->name = wind_filepath_get_filename(path);
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
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
        fullpath = hostfs_filepath_generate(hfs->dir_prefix,path,isdir);
#else
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
#endif
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        if(exist != W_ERR_OK)
            hfile = host_file_open_create((char*)path,mode);
        else
            hfile = host_file_open_exist((char*)path,mode);
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
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    //WIND_ASSERT_RETURN(hfile->fd != W_NULL,W_ERR_INVALID);

    if(hfile->isdir == 0)
        fclose(hfile->fd);
    else if(hfile->handle >= 0)
        _findclose(hfile->handle);
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
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
        fullpath = hostfs_filepath_generate(hfs->dir_prefix,path,isdir);
#else
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
#endif
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
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        if(path[0] != 0)
            isdir = wind_filepath_isdir(path);
        else
            isdir = 0;
#if  HOST_OS_TYPE == HOST_OS_WINDOWS
        fullpath = hostfs_filepath_generate(hfs->dir_prefix,path,isdir);
#else
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
#endif
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
    w_hostfile_s *subhfile = W_NULL;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 1,W_NULL);
    WIND_ASSERT_RETURN(hfile->path != W_NULL,W_NULL);
    do 
    {
        err = W_ERR_OK;
        if(!hfile->has_sub)
        {
            err = W_ERR_OK;
            wind_memset(&hfile->fileinfo,0,sizeof(_finddata_t));
            hfile->handle = _findfirst(hfile->path,&hfile->fileinfo);
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
            subhfile = hostfs_mem_malloc(sizeof(w_hostfile_s));
            WIND_ASSERT_BREAK(subhfile != W_NULL, W_ERR_MEM, "alloc sub hfile failed");
            wind_memset(subhfile,0,sizeof(w_hostfile_s));
        }
        subhfile->attr = (hfile->fileinfo.attrib & _A_SUBDIR)?HFILE_ATTR_DIR:0;
        subhfile->isdir = (subhfile->attr & HFILE_ATTR_DIR)?1:0;
        subhfile->name = wind_salloc(subhfile->name,HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(subhfile->name != W_NULL,W_ERR_MEM,"clone filename failed");
        hfile->subhfile = subhfile;
    }while(0);
    if(err == W_ERR_OK)
        return hfile->subhfile;
    if(subhfile)
    {
        if(subhfile->name)
            wind_free(subhfile->name);
        wind_free(subhfile);
    }
        
    return W_NULL;
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
    WIND_ASSERT_RETURN(dir->mode & HFMDOE_R,W_ERR_INVALID);
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

