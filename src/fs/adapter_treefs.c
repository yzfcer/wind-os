#include "wind_config.h"
#include "wind_type.h"
//#include "adapter_treefs.h"
#include "wind_file.h"
#include "treefs.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#if WIND_FS_SUPPORT
static w_err_t treefs_op_init(fs_s *fs)
{
    treefs_format();
    return ERR_OK;
}

static w_err_t treefs_op_format(fs_s *fs)
{
    return ERR_OK;
}

static w_err_t treefs_op_mkdir(const char *path)
{
    WIND_ASSERT_RETURN(path != NULL,ERR_NULL_POINTER);
    return ERR_FAIL;
}

static w_err_t treefs_op_open(file_s *file,fmode_e fmode)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    file->fileobj = treefile_open(file->path,(tf_fmode_e)fmode);
    if(file->fileobj == NULL)
        return ERR_FAIL;
    return ERR_OK;
}

static w_err_t treefs_op_close(file_s* file)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    return treefile_close((treefile_s *)file->fileobj);
}

static w_err_t treefs_op_rmfile(file_s* file)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    return treefile_rm((treefile_s *)file->fileobj);
}

static file_s *treefs_op_subfile(file_s* file,w_int32_t index)
{
    file_s *nfile;
    treefile_s *tfile;
    tfile = treefile_readdir((treefile_s *)file->fileobj,index);
    WIND_ASSERT_RETURN(tfile != NULL,NULL);
    nfile = _file_malloc();
    WIND_ASSERT_RETURN(nfile != NULL,NULL);
    nfile->fileobj = tfile;
    //nf
    return NULL;
}

static w_err_t treefs_op_seek(file_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    return treefile_seek((treefile_s *)file->fileobj,offset);
}

static w_err_t treefs_op_rename(file_s* file,char *newname)
{
    treefile_s *tfile;
    char *name;
    w_int32_t len;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(newname != NULL,ERR_NULL_POINTER);
    tfile = (treefile_s *)file->fileobj;
    len = wind_strlen(newname) + 1;
    name = wind_malloc(len);
    WIND_ASSERT_RETURN(name != NULL,ERR_MEM);
    wind_free(tfile->filename);
    tfile->filename = name;
    return ERR_OK;
}

static w_int32_t treefs_op_ftell(file_s* file)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    return treefile_ftell((treefile_s *)file->fileobj);
}

static w_int32_t treefs_op_read(file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    return treefile_read((treefile_s *)file->fileobj,buff,size);
}

static w_int32_t treefs_op_write(file_s* file,w_uint8_t *buff, w_int32_t size)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    return treefile_write((treefile_s *)file->fileobj,buff,size);
}
static w_err_t treefs_op_fgets(file_s* file,char *buff, w_int32_t maxlen)
{
    return ERR_FAIL;
}
static w_err_t treefs_op_fputs(file_s* file,char *buff)
{
    return ERR_FAIL;
}

FS_OPS_DEF(treefs);

fs_s fs_treefs[1] = 
{
    WIND_FS_DEF(treefs,FSTYPE_TREEFS,fs_ops),
};



#endif
