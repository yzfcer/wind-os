#include "hostfs_api.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_filepath.h"
#include <fileapi.h>


#if WIND_HOSTFS_SUPPORT

hfileattr_e hostapi_file_type(char *path)
{
	w_uint32_t attr;
    attr = GetFileAttributes(path);
    if(INVALID_FILE_ATTRIBUTES == attr)
        return HFILE_TYPE_ERROR;
    if(attr & FILE_ATTRIBUTE_DIRECTORY)
        return HFILE_TYPE_DIR;
    return HFILE_TYPE_FILE;
}


void *hostapi_mem_malloc(w_int32_t size)
{
    void *ptr;
    ptr = wind_alloc(size,HP_ALLOCID_HOSTFS);
    return ptr;
}

w_err_t hostapi_mem_free(void *ptr)
{
    return wind_free(ptr);
}


w_err_t   hostapi_file_exist(char *path)
{
    FILE *file;
    file = fopen(path,"r");
    WIND_CHECK_RETURN(file != W_NULL, W_ERR_FAIL);
    fclose(file);
    return W_ERR_OK;
}

static w_err_t hostapi_file_destroy(hfile_s *hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    hfile->magic = 0;
    if(hfile->subhfile != W_NULL)
        hostapi_file_destroy(hfile->subhfile);
    if(hfile->path  != W_NULL)
        hostapi_mem_free(hfile->path);
    if(hfile->fileinfo != W_NULL)
        hostapi_mem_free(hfile->fileinfo);
    hostapi_mem_free(hfile);
    return W_ERR_OK;
}

static hfile_s*   hostapi_file_create(char *path,w_uint8_t mode,w_uint8_t isdir,FILE *fd)
{
    w_err_t err;
    hfile_s *hfile = W_NULL;
    WIND_ASSERT_RETURN(path != W_NULL, W_NULL);
    do
    {
        err = W_ERR_OK;
        hfile = hostapi_mem_malloc(sizeof(hfile_s));
        WIND_ASSERT_BREAK(hfile != W_NULL, W_ERR_MEM, "alloc hfile failed");
        wind_memset(hfile,0,sizeof(hfile_s));
        hfile->magic = HFILE_MAGIC;
        hfile->fd = fd;
        hfile->subhnd = -1;
        hfile->fileinfo = W_NULL;
        hfile->isdir = isdir;
        hfile->mode = mode;
        hfile->fileinfo = W_NULL;
        hfile->path = wind_salloc(path, HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(hfile->path != W_NULL, W_ERR_MEM, "alloc path failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
            hostapi_file_destroy(hfile);
        hfile = W_NULL;
    }
    return hfile;
}



static hfile_s*   hostapi_file_open_create(char *path,w_uint8_t mode)
{
    w_int8_t isdir;
    w_int32_t res;
    w_err_t err;
    FILE *file = W_NULL;
    hfile_s *hfile = W_NULL;
    WIND_ASSERT_RETURN(path != W_NULL, W_NULL);
    do
    {
        err = W_ERR_OK;
        if(!hfile->isdir)
        {
            file = fopen(path,"wb+");
            WIND_ASSERT_BREAK(file != W_NULL, W_ERR_FAIL, "create file failed");
        }
        else
        {
            res = _mkdir(path);
            WIND_ASSERT_BREAK(res == 0, W_ERR_FAIL, "make dir failed");
            file = W_NULL;
        }
        isdir = wind_filepath_isdir(path)?1:0;
        hfile = hostapi_file_create(path,mode,isdir,file);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create file obj failed");
    }while(0);
    
    if(err != W_ERR_OK)
    {
        if(hfile)
            hostapi_file_destroy(hfile);
        else if(file)
            fclose(file);
        hfile = W_NULL;
    }
    return hfile;
}

static hfile_s*   hostapi_file_open_exist(char *path,w_uint8_t mode)
{
    w_err_t err;
    FILE *file;
    hfile_s *hfile = W_NULL;
    hfileattr_e attr;
    w_uint8_t isdir;
    do
    {
        err = W_ERR_OK;
        attr = hostapi_file_type(path);
        WIND_ASSERT_BREAK(attr != HFILE_TYPE_ERROR, W_ERR_FAIL, "get file type failed");
        isdir = (attr == HFILE_TYPE_DIR)?1:0;
        if(!isdir)
        {
            file = fopen(path,"rb+");
            WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"open file failed");
        }
        
        hfile = hostapi_file_create(path,mode,isdir,file);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"create file obj failed");
        
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
            hostapi_file_destroy(hfile);
        hfile = W_NULL;
    }    
    return hfile;
    
}




hfile_s*  hostapi_file_open(char *path,w_uint8_t mode)
{
    w_err_t err,exist;
    FILE *fd = W_NULL;
    hfile_s *hfile = W_NULL;
    
    WIND_ASSERT_RETURN(path != W_NULL, W_NULL);
    do
    {
        err = W_ERR_OK;
        exist = hostapi_file_exist(path);
        WIND_ASSERT_BREAK((exist == W_ERR_OK) || ((mode & HFMDOE_CRT) != 0),W_ERR_NOFILE,"file is not exist");
        if(exist != W_ERR_OK)
            hfile = hostapi_file_open_create(path,mode);
        else
            hfile = hostapi_file_open_exist(path,mode);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"open hfile failed");

    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile)
            hostapi_file_destroy(hfile);
        hfile = W_NULL;
    }
    return hfile;
}

w_int32_t   hostapi_file_read(hfile_s *hfile,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t rdlen;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL, W_ERR_INVALID);
    rdlen = fread(buff,1,len,hfile->fd);
    return rdlen;
}

w_int32_t   hostapi_file_write(hfile_s *hfile,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t wrlen;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->fd != W_NULL, W_ERR_INVALID);
    wrlen = fwrite(buff,1,len,hfile->fd);
    return wrlen;
}

hfile_s *hostapi_file_readdir(hfile_s *hfile)
{
    w_err_t err;
    w_int32_t res;
    hfile_s *subhfile;
    WIND_ASSERT_RETURN(hfile != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(hfile->isdir == 1,W_NULL);
    WIND_ASSERT_RETURN(hfile->path != W_NULL,W_NULL);
    do 
    {
        err = W_ERR_OK;
        if(hfile->fileinfo == W_NULL)
        {
            err = W_ERR_OK;
            hfile->fileinfo = hostapi_mem_malloc(sizeof(hfileinfo_s));
            WIND_ASSERT_BREAK(hfile->fileinfo != W_NULL, W_ERR_MEM, "alloc fileinfo failed");
            hfile->subhnd = _findfirst(hfile->path,hfile->fileinfo);
        }
        else
        {
            WIND_ASSERT_BREAK(hfile->subhnd >= 0, W_ERR_INVALID, "handle is invalid");
            res = _findnext(hfile->subhnd,hfile->fileinfo);
            if(res < 0)
            {
                hostapi_mem_free(hfile->fileinfo);
                hfile->fileinfo = W_NULL;
            }
        }
        WIND_CHECK_BREAK(hfile->fileinfo != W_NULL, W_ERR_FAIL);
        if(hfile->subhfile == W_NULL)
        {
            hfile->subhfile = hostapi_mem_malloc(sizeof(hfile_s));
            WIND_ASSERT_BREAK(hfile->subhfile != W_NULL, W_ERR_MEM, "alloc sub hfile failed");
            wind_memset(hfile->subhfile,0,sizeof(hfile_s));
        }
        subhfile = hfile->subhfile;
        
    }while(0);
    if(err == W_ERR_OK)
        return hfile->subhfile;
    return W_NULL;

}

w_err_t   hostapi_file_close(hfile_s *hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic = HFILE_MAGIC,W_ERR_INVALID);
    if(hfile->isdir == 0)
    {
        if(hfile->fd != W_NULL)
            fclose(hfile->fd);
    }
    else
    {
		if(hfile->subhnd >= 0)
            _findclose(hfile->subhnd);
    }
    return hostapi_file_destroy(hfile);
}

w_err_t   hostapi_file_seek(hfile_s *hfile,w_uint32_t offset)
{
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    fseek(hfile->fd,offset,SEEK_SET);
    return W_ERR_OK;
}

w_uint32_t   hostapi_file_tell(hfile_s *hfile)
{
    w_uint32_t offset;
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
    offset = ftell(hfile->fd);
    return offset;
}

w_uint32_t hostapi_file_size(hfile_s *hfile)
{
    w_uint32_t len;
	w_int32_t offset;
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(hfile->isdir == 0,W_ERR_INVALID);
	offset = ftell(hfile->fd);
    fseek(hfile->fd,0,SEEK_END);
    len = ftell(hfile->fd);
	fseek(hfile->fd,offset,SEEK_SET);
	return len;
}


#endif

