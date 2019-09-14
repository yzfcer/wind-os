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

hfile_s*   hostapi_file_create(char *path)
{
    w_err_t err;
    FILE *file;
    hfile_s *hfile = W_NULL;
    w_int8_t isdir;
    w_int32_t res;
    WIND_ASSERT_RETURN(path != W_NULL, W_NULL);
    do
    {
        err = W_ERR_OK;
        isdir = wind_filepath_isdir(path)?1:0;
        if(!isdir)
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
        hfile = hostapi_mem_malloc(sizeof(hfile_s));
        WIND_ASSERT_BREAK(hfile != W_NULL, W_ERR_MEM, "alloc hfile failed");
        hfile->magic = HFILE_MAGIC;
        hfile->fd = file;
		hfile->subhnd = -1;
        hfile->fileinfo = W_NULL;
        hfile->isdir = isdir;
        hfile->fileinfo = W_NULL;
        hfile->path = wind_salloc(path, HP_ALLOCID_HOSTFS);
        
    }while(0);
    if(err != W_ERR_OK)
    {
        if(hfile->fileinfo != W_NULL)
            hostapi_mem_free(hfile->fileinfo);
        if(hfile)
            hostapi_file_destroy(hfile);
    }
    return hfile;
}

w_err_t hostapi_file_destroy(hfile_s *hfile)
{
    WIND_ASSERT_RETURN(hfile != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(hfile->magic == HFILE_MAGIC,W_ERR_INVALID);
    hfile->magic = 0;
    if(hfile->subhfile != W_NULL)
        hostapi_file_destroy(hfile->subhfile);
    if(hfile->path  != W_NULL)
        hostapi_mem_free(hfile->path);
    hostapi_mem_free(hfile);
    return W_ERR_OK;
}


hfile_s*  hostapi_file_open(char *path,char *mode)
{
    FILE *fd = W_NULL;
    do
    {
        fd = fopen(path,mode);
    }while(0);
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

