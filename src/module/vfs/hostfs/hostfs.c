/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : hostfs.c
** Author      : Jason Zhou
** Last Date   : 2019.09.08
** Description : hostfs host file system principal function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.09.08
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.09.08
** Description : 
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
#include <stdlib.h>
#if WIND_HOSTFS_SUPPORT

#if  HOST_OS_TYPE == HOST_OS_WINDOWS
#include "windows_filepath.h"
static w_int32_t host_mkdir(char *path)
{
    return _mkdir(path);
}

static void host_release_subfile(w_hostfile_s *hfile)
{
    if(hfile && (hfile->handle > 0))
    {
        _findclose(hfile->handle);
        hfile->handle = 0;
    }
}

static w_err_t get_subinfo(w_hostfile_s *hfile,char *path)
{
    w_err_t err;
    w_int32_t res;
    char *tmppath = (char*)W_NULL;
    do
    {
        err = W_ERR_OK;
        tmppath = host_filepath_generate(path,"/*",0);
        if(!hfile->subinfo.has_sub)
        {
            wind_memset(&hfile->finddata,0,sizeof(_finddata_t));
            hfile->handle = _findfirst(tmppath,&hfile->finddata);
            WIND_CHECK_BREAK(hfile->handle >= 0,W_ERR_FAIL);
            hfile->subinfo.has_sub = 1;
        }
        else
        {
            WIND_ASSERT_BREAK(hfile->handle >= 0, W_ERR_INVALID, "handle is invalid");
            res = _findnext(hfile->handle,&hfile->finddata);
            WIND_CHECK_BREAK(res >= 0,W_ERR_FAIL)
        }
        hfile->subinfo.attr = (hfile->finddata.attrib & _A_SUBDIR)?HFILE_ATTR_DIR:0;
        if(hfile->subinfo.name != W_NULL)
            wind_free(hfile->subinfo.name);
        hfile->subinfo.name = (char*)wind_salloc(hfile->finddata.name,HP_ALLOCID_HOSTFS);
    }while(0);
    if(tmppath != W_NULL)
		host_filepath_release(tmppath);
    return err;
}

w_bool_t hostfile_existing(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    w_int32_t res,len;
    w_uint8_t isdir = 0;
    char *fullpath = (char*)W_NULL;
    WIND_ASSERT_RETURN(hfs != W_NULL,W_FALSE);
    WIND_ASSERT_RETURN(path != W_NULL,W_FALSE);
    do
    {
        err = W_ERR_OK;
        if(path[0] != 0)
            isdir = host_filepath_isdir(path);
        fullpath = host_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        len = wind_strlen(fullpath);
        if(fullpath[len - 1] == '/')
            fullpath[len - 1] = 0;
        res = _access(fullpath,0);
        WIND_CHECK_BREAK(res == 0, W_ERR_FAIL);

    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err == W_ERR_OK ? W_TRUE : W_FALSE;
}

static w_err_t host_remove_dir(char *fullpath)
{
    w_int32_t len;
    w_err_t err;
    char *cmd = (char *)W_NULL;
    WIND_ASSERT_RETURN(fullpath != W_NULL,W_ERR_NULL_PTR);
    
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
        wind_printf("execute cmd:%s\r\n",cmd);
        system(cmd);
    }while(0);
    if(cmd != W_NULL)
        hostfs_mem_free(cmd);
    return err;
}
#endif

#if  HOST_OS_TYPE == HOST_OS_LINUX

#include "sys/stat.h"
#include "unistd.h"
#define host_filepath_get_filename wind_filepath_get_filename 
#define host_filepath_release wind_filepath_release
#define host_filepath_remove_tail wind_filepath_remove_tail
#define  host_filepath_isdir wind_filepath_isdir
#define host_filepath_check_valid wind_filepath_check_valid

static w_int32_t host_mkdir(char *path)
{
    return mkdir(path,0755);
}

static void host_release_subfile(w_hostfile_s *hfile)
{
    if(hfile && hfile->dir)
    {
        closedir(hfile->dir);
        hfile->dir = (DIR *)W_NULL;
    }
}

static w_err_t get_subinfo(w_hostfile_s *hfile,char *path)
{
    w_err_t err;
    do
    {
        err = W_ERR_OK;
        if(hfile->dir == W_NULL)
            hfile->dir = opendir(path);
        WIND_ASSERT_BREAK(hfile->dir != W_NULL,W_ERR_FAIL,"dirent is NULL");
        hfile->dirinfo = readdir(hfile->dir);
        WIND_CHECK_BREAK(hfile->dirinfo,W_ERR_FAIL);
        hfile->subinfo.attr = (hfile->dirinfo->d_type == DT_DIR)?HFILE_ATTR_DIR:0;
        if(hfile->subinfo.name != W_NULL)
            wind_free(hfile->subinfo.name);
		hfile->subinfo.name = (char*)wind_salloc(hfile->dirinfo->d_name,HP_ALLOCID_HOSTFS);
		//wind_notice("hfile->subinfo.name=%s",hfile->subinfo.name);
    }while(0);
    return err;
}

hfileattr_e host_file_type(char *path)
{
    int res;
    struct stat st;
    hfileattr_e attr = HFILE_TYPE_ERROR;
    
    res = stat(path,&st);
    if(res != 0)
        attr = HFILE_TYPE_ERROR;
    else if (S_ISDIR(st.st_mode))
        attr = HFILE_TYPE_DIR;
    else
        attr = HFILE_TYPE_FILE;
	//wind_notice("host_file_type attr=0x%02x",attr);
    return attr;
}


static char *host_filepath_generate(const char *pre_path,const char *relative_path,w_uint16_t isdir)
{
    w_err_t err;
    char *path = (char*)W_NULL;
    w_int32_t len,len1,len2;
    WIND_ASSERT_RETURN(pre_path != W_NULL,(char*)W_NULL);
    WIND_ASSERT_RETURN(pre_path[0] != 0,(char*)W_NULL);
    WIND_ASSERT_RETURN(pre_path[0] == '/',(char*)W_NULL);
    WIND_ASSERT_RETURN(relative_path != W_NULL,(char*)W_NULL);
    //wind_notice("pre_path:%s",pre_path);
    //wind_notice("relative_path:%s",relative_path);
    len1 = wind_strlen(pre_path);
    len2 = wind_strlen(relative_path);

    len = len1 + len2 + 3;
    path = (char*)hostfs_mem_malloc(len);
    wind_memset(path,0,len);
    wind_strcpy(path,pre_path);
    if(pre_path[len1-1] != '/')
        path[len1] = '/';
    if(relative_path[0] == '/')
        relative_path ++;
    wind_strcat(path,relative_path);
    len = wind_strlen(path);
	if(!isdir)
	{
		if(path[len - 1] == '/')
			path[len - 1] = 0;	
	}
	else
	{
		if(path[len - 1] != '/')
			path[len - 1] = '/';	
	}

    //wind_notice("gen path:%s",path);
    
    err = host_filepath_check_valid(path);
    if(err != W_ERR_OK)
    {
        wind_free(path);
        path = (char*)W_NULL;
    }
    return path;
}

w_bool_t hostfile_existing(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    hfileattr_e hfattr;
    w_uint8_t isdir = 0;
    char *fullpath = (char*)W_NULL;
    WIND_ASSERT_RETURN(hfs != W_NULL,W_FALSE);
    WIND_ASSERT_RETURN(path != W_NULL,W_FALSE);
    do
    {
        err = W_ERR_OK;
        if(path[0] != 0)
            isdir = host_filepath_isdir(path);
        fullpath = host_filepath_generate(hfs->dir_prefix,path,isdir);
        hfattr = host_file_type(fullpath);
        WIND_CHECK_BREAK(hfattr != HFILE_TYPE_ERROR,W_ERR_FAIL);
        //err = W_ERR_NOT_SUPPORT;
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err == W_ERR_OK ? W_TRUE : W_FALSE;
}

static w_err_t host_remove_dir(char *fullpath)
{
    w_int32_t len;
    w_err_t err;
    char *cmd = (char *)W_NULL;
    WIND_ASSERT_RETURN(fullpath != W_NULL,W_ERR_NULL_PTR);
    
    wind_trace("fullpath=%s",fullpath);
    do
    {
        err = W_ERR_OK;
        len = wind_strlen(fullpath) + 1;
        len += sizeof("rm -rf ");
        cmd = (char *)hostfs_mem_malloc(len);
        WIND_ASSERT_BREAK(cmd != W_NULL,W_ERR_MEM,"alloc cmd failed");
        wind_strcpy(cmd,"rm -rf ");
        wind_strcat(cmd,fullpath);
		//host_filepath_remove_tail(cmd);
        wind_printf("execute cmd:%s\r\n",cmd);
        system(cmd);
    }while(0);
    if(cmd != W_NULL)
        hostfs_mem_free(cmd);
    return err;
}
#endif



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
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dir_prefix != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dir_prefix[0] != 0,W_ERR_INVALID);
    hfs->magic = HOSTFS_MAGIC;
    hfs->dir_prefix = (char*)wind_salloc(dir_prefix,HP_ALLOCID_HOSTFS);
    WIND_ASSERT_RETURN(hfs->dir_prefix != W_NULL,W_ERR_MEM);
    return W_ERR_OK;
}



w_err_t hostfs_deinit(w_hostfs_s *hfs)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_NULL_PTR);
    if(hfs->dir_prefix != W_NULL)
        wind_free(hfs->dir_prefix);
    wind_memset(hfs,0,sizeof(w_hostfs_s));
    return W_ERR_OK;
}

w_err_t hostfs_match(w_blkdev_s *blkdev)
{
    return W_ERR_FAIL;
}

static w_err_t host_file_destroy(w_hostfile_s *hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_NULL_PTR);
    //WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    hfile->magic = 0;
    if(hfile->subhfile != W_NULL)
    {
        host_file_destroy(hfile->subhfile);
        //hostfs_mem_free(hfile->subhfile);
    }
        
    if(hfile->path  != W_NULL)
        hostfs_mem_free(hfile->path);
    if(hfile->name != W_NULL)
        hostfs_mem_free(hfile->name);
    if(hfile->subinfo.name != W_NULL)
        hostfs_mem_free(hfile->subinfo.name);
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
        hfile->name = host_filepath_get_filename(path);

        WIND_ASSERT_BREAK(hfile->name != W_NULL, W_ERR_MEM, "alloc hfile name failed");
        hfile->mode = mode;
        hfile->attr = isdir?HFILE_ATTR_DIR:0;
        hfile->isdir = isdir;
        hfile->subinfo.has_sub = 0;
        hfile->subhfile = (w_hostfile_s*)W_NULL;
        hfile->fd = fd;
        hfile->path = (char*)wind_salloc(path, HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(hfile->path != W_NULL, W_ERR_MEM, "alloc path failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
        {
            if(hfile->name)
                host_filepath_release(hfile->name);
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
    //errno_t errno;
    w_err_t err;
    FILE *fd;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    char *realpath = (char*)W_NULL;
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
            realpath = host_filepath_remove_tail(path);
            WIND_ASSERT_BREAK(realpath != W_NULL,W_ERR_MEM,"alloc realpath failed");

            fd = fopen(realpath,"rb+");
            //WIND_ASSERT_BREAK(fd == 0,W_ERR_FAIL,"open hfile failed");
            WIND_ASSERT_BREAK(fd != W_NULL,W_ERR_FAIL,"open hfile failed");
        }
        else
            fd = (FILE *)W_NULL;
        
        hfile = host_file_create(path,mode,isdir,fd);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create hfile obj failed");
        
    }while(0);
    if(realpath != W_NULL)
        wind_free(realpath);
    if(err != W_ERR_OK)
    {
        if(hfile)
            host_file_destroy(hfile);
        hfile = (w_hostfile_s *)W_NULL;
    }
	//wind_notice("host_file_open_exist hfile=%p",hfile);
    return hfile;
    
}

static w_hostfile_s*   host_file_open_create(char *path,w_uint8_t mode)
{
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
        isdir = host_filepath_isdir(path);
        if(!isdir)
        {
            realpath = host_filepath_remove_tail(path);
            WIND_ASSERT_BREAK(realpath != W_NULL,W_ERR_MEM,"alloc realpath failed");

            //wind_notice("open file: %s",realpath);
            fd = fopen(realpath,"wb+");
            //WIND_ASSERT_BREAK(errno == 0,W_ERR_FAIL,"open hfile failed");
            WIND_ASSERT_BREAK(fd != W_NULL, W_ERR_FAIL, "create hfile failed");
        }
        else
        {
            res = host_mkdir(path);
            WIND_ASSERT_BREAK(res == 0, W_ERR_FAIL, "make dir failed");
            fd = (FILE *)W_NULL;
        }
        hfile = host_file_create(path,mode,isdir,fd);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create hfile obj failed");
    }while(0);
	//wind_notice("hfile=%p",hfile);
    if(realpath)
        host_filepath_release(realpath);
    
    if(err != W_ERR_OK)
    {
        if(hfile)
            host_file_destroy(hfile);
        else if(fd)
            fclose(fd);
        hfile = (w_hostfile_s *)W_NULL;
    }
	//wind_notice("host_file_open_create hfile=%p",hfile);
    return hfile;
}


w_hostfile_s* hostfile_open(w_hostfs_s *hfs,const char *path,w_uint8_t mode)
{
    w_err_t err;
    w_bool_t exist;
    w_uint8_t isdir;
    w_hostfile_s *hfile = (w_hostfile_s *)W_NULL;
    char *fullpath = (char *)W_NULL;
	//wind_notice("hostfile_open %s,mode=0x%02x",path,mode);
    do
    {
        err = W_ERR_OK;
        isdir = host_filepath_isdir(path);
        fullpath = host_filepath_generate(hfs->dir_prefix,path,isdir);

        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        exist = hostfile_existing(hfs,path);
		//wind_notice("exist=%d",exist);
        WIND_CHECK_BREAK((exist == W_TRUE) || ((mode & HFMODE_CRT) != 0),W_ERR_NOFILE);
        if(exist != W_TRUE)
            hfile = host_file_open_create((char*)fullpath,mode);
        else
            hfile = host_file_open_exist((char*)fullpath,mode);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"open hfile failed");
        hfile->hfs = hfs;
    }while(0);
	//wind_notice("hostfile_open[1] err=%d",err);
	//wind_notice("hostfile_open[1] hfile=%p",hfile);
    if(fullpath)
        wind_free(fullpath);
    if(err != W_ERR_OK)
    {
        if(hfile)
            wind_free(hfile);
        hfile = (w_hostfile_s *)W_NULL;
    }
	//wind_notice("hostfile_open hfile=%p",hfile);
    return hfile;
}




w_err_t hostfile_close(w_hostfile_s* hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);

    if(hfile->isdir == 0)
    {
        fclose(hfile->fd);
    }
	host_release_subfile(hfile);
    return host_file_destroy(hfile);
}




w_err_t hostfile_remove(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    w_uint8_t isdir;
    hfileattr_e attr;
    w_int32_t len;
    
    char *fullpath = (char*)W_NULL;
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_NULL_PTR);
    do
    {
        err = W_ERR_OK;
        isdir = host_filepath_isdir((char*)path);
        fullpath = host_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        attr = host_file_type((char*)fullpath);
        if(attr == HFILE_TYPE_DIR)
        {
            len = wind_strlen(fullpath);
            if(fullpath[len - 1] == '/')
                fullpath[len - 1] = 0;
            err = host_remove_dir(fullpath);
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
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    hfile->attr = attr;
    return W_ERR_OK;
}

w_err_t hostfile_get_attr(w_hostfile_s* hfile,w_uint8_t *attr)
{
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    *attr = hfile->attr;
    return W_ERR_OK;
}




w_err_t hostfile_seek(w_hostfile_s* hfile,w_int32_t offset)
{
    w_err_t err;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
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
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
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
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL, W_ERR_INVALID);
    rdsize = fread(buff,1,size,hfile->fd);
    return rdsize;
}


w_int32_t hostfile_write(w_hostfile_s* hfile,w_uint8_t *buff,w_int32_t size)
{
    w_int32_t wrsize = 0;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL, W_ERR_INVALID);
    wrsize = fwrite(buff,1,size,hfile->fd);
    return wrsize;
}




static w_err_t do_host_file_readdir(w_hostfile_s *hfile)
{
    w_err_t err;
    w_hostfile_s *subhfile;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->isdir == 1,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->path != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->subhfile != W_NULL,W_ERR_NULL_PTR);
    do 
    {
        err = W_ERR_OK;
        err = get_subinfo(hfile,hfile->path);
        WIND_CHECK_BREAK(err == W_ERR_OK,err);

        subhfile = hfile->subhfile;
        subhfile->attr = hfile->subinfo.attr;
        subhfile->isdir = (subhfile->attr & HFILE_ATTR_DIR)?1:0;
        if(subhfile->name != W_NULL)
            wind_free(subhfile->name);
        subhfile->name = (char*)wind_salloc(hfile->subinfo.name,HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(subhfile->name != W_NULL,W_ERR_MEM,"clone filename failed");
        hfile->subhfile = subhfile;
    }while(0);
    return err;
}
 


w_err_t hostfile_readdir(w_hostfile_s* dir,w_hostfile_s** sub)
{
    w_err_t err;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dir->magic == HOSTFILE_MAGIC,W_ERR_INVALID);

    WIND_ASSERT_RETURN(dir->hfs != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dir->mode & HFMODE_R,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_HFILE_ATTR_DIR(dir->attr) != 0,W_ERR_INVALID);

    do
    {
        err = W_ERR_OK;
        *sub = (w_hostfile_s*)W_NULL;
        if(dir->subhfile == W_NULL)
        {
            dir->subhfile = (w_hostfile_s*)hostfs_mem_malloc(sizeof(w_hostfile_s));
            WIND_ASSERT_BREAK(dir->subhfile != W_NULL,W_ERR_MEM,"alloc hostfile obj failed");
            wind_memset(dir->subhfile,0,sizeof(w_hostfile_s));
            dir->subhfile->magic = HOSTFILE_MAGIC;
            dir->subhfile->hfs = dir->hfs;
        }
        err = do_host_file_readdir(dir);
        WIND_CHECK_BREAK(err == W_ERR_OK,W_ERR_FAIL);
        *sub = dir->subhfile;
    }while(0);
    if(err != W_ERR_OK)
    {
        
        if(dir->subhfile != W_NULL)
        {
            host_file_destroy(dir->subhfile);
        }
            
        dir->subhfile = (w_hostfile_s*)W_NULL;
        *sub = (w_hostfile_s*)W_NULL;
    }
    return err;
}


w_uint32_t host_file_size(w_hostfile_s *hfile)
{
    w_uint32_t len;
    w_int32_t offset;
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(hfile->magic == HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    offset = ftell(hfile->fd);
    fseek(hfile->fd,0,SEEK_END);
    len = ftell(hfile->fd);
    fseek(hfile->fd,offset,SEEK_SET);
    return len;
}


#endif

