#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#include <stdio.h>
#include <direct.h>
#include <io.h>

#if WIND_HOSTFS_SUPPORT
#define HFILE_MAGIC 0x2576DA83
typedef struct _finddata_t hfileinfo_s;
typedef struct _hfile_s hfile_s;

#define HFMDOE_R   0x01
#define HFMDOE_W   0x02
#define HFMDOE_RW  0x03
#define HFMDOE_CRT 0x04

typedef enum
{
    HFILE_TYPE_ERROR = 0,
    HFILE_TYPE_DIR = 1,
    HFILE_TYPE_FILE = 2,
}hfileattr_e;

struct _hfile_s
{
    w_uint32_t magic;
    char *name;
    FILE* fd;
    w_int32_t subhnd;
    hfileinfo_s* fileinfo;
    hfile_s *subhfile;
    char *path;
    w_uint8_t isdir;
    w_uint8_t mode;
};


hfileattr_e hostapi_file_type(char *path);

void *hostapi_mem_malloc(w_int32_t size);

w_err_t hostapi_mem_free(void *ptr);

w_err_t hostapi_file_exist(char *path);

//hfile_s* hostapi_file_create(char *path);

//w_err_t hostapi_file_destroy(hfile_s *hfile);

hfile_s*  hostapi_file_open(char *path,w_uint8_t mode);

w_int32_t hostapi_file_read(hfile_s *hfile,w_uint8_t *buff,w_int32_t len);

w_int32_t hostapi_file_write(hfile_s *hfile,w_uint8_t *buff,w_int32_t len);

hfile_s *hostapi_file_readdir(hfile_s *hfile);

w_err_t hostapi_file_close(hfile_s *hfile);

w_err_t hostapi_file_remove(char *path);

w_err_t hostapi_file_seek(hfile_s *hfile,w_uint32_t offset);

w_uint32_t hostapi_file_tell(hfile_s *hfile);

w_uint32_t hostapi_file_size(hfile_s *hfile);


#endif

