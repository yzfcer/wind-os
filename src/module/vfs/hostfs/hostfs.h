/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : hostfs.h
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
#ifndef WIND_HOSTFS_H__
#define WIND_HOSTFS_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"

#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
typedef struct _finddata_t _finddata_t;
#endif

#if (HOST_OS_TYPE == HOST_OS_LINUX)
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
//typedef int errno_t ;
#endif

#if WIND_HOSTFS_SUPPORT

#define HOSTFS_MAGIC   0x49AC7D53
#define HOSTFILE_MAGIC 0x2576DA83
#define HFILE_NAME_LEN 64    //File name length
#define HOSTFS_DIR_LAYCNT 32 //Catalog depth
#define HOSTFS_MAX_FILE_SIZE 0x7fffffff //File length limit，2GB

#define HFMODE_R   0x01
#define HFMODE_W   0x02
#define HFMODE_RW  0x03
#define HFMODE_CRT 0x04


//File properties
#define HFILE_ATTR_DIR    (0x01 << 0) //Directory or not
#define HFILE_ATTR_RDEN   (0x01 << 1) //Readable or not
#define HFILE_ATTR_WREN   (0x01 << 2) //Writable or not
#define HFILE_ATTR_HIDE   (0x01 << 3) //Hide or not
#define HFILE_ATTR_VERIFY (0x01 << 4) //Check or not
#define HFILE_ATTR_COMMAN (HFILE_ATTR_RDEN | HFILE_ATTR_WREN)

#define IS_HFILE_ATTR_DIR(attr) (attr & HFILE_ATTR_DIR)
#define SET_HFILE_ATTR_DIR(attr) (attr |= HFILE_ATTR_DIR)
#define CLR_HFILE_ATTR_DIR(attr) (attr &= (~HFILE_ATTR_DIR))

#define IS_HFILE_ATTR_RDEN(attr) (attr & HFILE_ATTR_RDEN)
#define SET_HFILE_ATTR_RDEN(attr) (attr |= HFILE_ATTR_RDEN)
#define CLR_HFILE_ATTR_RDEN(attr) (attr &= (~HFILE_ATTR_RDEN))

#define IS_HFILE_ATTR_WREN(attr) (attr & HFILE_ATTR_WREN)
#define SET_HFILE_ATTR_WREN(attr) (attr |= HFILE_ATTR_WREN)
#define CLR_HFILE_ATTR_WREN(attr) (attr &= (~HFILE_ATTR_WREN))

#define IS_HFILE_ATTR_HIDE(attr) (attr & HFILE_ATTR_HIDE)
#define SET_HFILE_ATTR_HIDE(attr) (attr |= HFILE_ATTR_HIDE)
#define CLR_HFILE_ATTR_HIDE(attr) (attr &= (~HFILE_ATTR_HIDE))

#define IS_HFILE_ATTR_VERIFY(attr) (attr & HFILE_ATTR_VERIFY)
#define SET_HFILE_ATTR_VERIFY(attr) (attr |= HFILE_ATTR_VERIFY)
#define CLR_HFILE_ATTR_VERIFY(attr) (attr &= (~HFILE_ATTR_VERIFY))

typedef struct __hostfile_s w_hostfile_s;

typedef enum
{
    HFILE_TYPE_ERROR = 0,
    HFILE_TYPE_DIR = 1,
    HFILE_TYPE_FILE = 2,
}hfileattr_e;

//Host associated file system information
typedef struct __hostfs_s
{
    w_uint32_t magic;
    char *dir_prefix;
}w_hostfs_s;

typedef struct 
{
    w_uint8_t has_sub;
    w_uint8_t attr;
    char *name;
 }w_subinfo_s;

//Host associated File infomation
struct __hostfile_s
{
    w_uint32_t magic;  //Magic word
    w_hostfs_s *hfs;   //Host file system object
    char *path;        //File path, relative to mount address
    char *name;        //file name
    w_uint8_t mode;    //Open mode
    w_uint8_t attr;    //File properties
    w_uint8_t isdir;   //Directory or not
    //w_uint8_t has_sub;   //Whether there are sub files
    w_hostfile_s *subhfile;//Subfile (or subdirectory)
    FILE* fd;          //File handle
    w_subinfo_s subinfo;
#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
    intptr_t handle;  //Subfile handle (Windows system)
    _finddata_t finddata;//Subfile information
#endif
#if (HOST_OS_TYPE == HOST_OS_LINUX)
    DIR *dir;
    struct dirent *dirinfo;
#endif
};

hfileattr_e host_file_type(char *path);
w_err_t _wind_hostfs_mod_init(void);
void *hostfs_mem_malloc(w_int32_t size);
w_err_t hostfs_mem_free(void *ptr);
w_err_t hostfs_init(w_hostfs_s *hfs,char *dir_prefix);
w_err_t hostfs_deinit(w_hostfs_s *hfs);
w_err_t hostfs_match(w_blkdev_s *blkdev);

w_bool_t hostfile_existing(w_hostfs_s *hfs,const char *path);
w_hostfile_s* hostfile_open(w_hostfs_s *hfs,const char *path,w_uint8_t mode);
w_err_t hostfile_set_attr(w_hostfile_s* hfile,w_uint8_t attr);
w_err_t hostfile_get_attr(w_hostfile_s* hfile,w_uint8_t *attr);
w_err_t hostfile_close(w_hostfile_s* hfile);
w_err_t hostfile_remove(w_hostfs_s *hfs,const char *path);
w_err_t hostfile_seek(w_hostfile_s* hfile,w_int32_t offset);
w_int32_t hostfile_ftell(w_hostfile_s* hfile);
w_int32_t hostfile_read(w_hostfile_s* hfile,w_uint8_t *buff, w_int32_t size);
w_int32_t hostfile_write(w_hostfile_s* hfile,w_uint8_t *buff, w_int32_t size);

w_err_t hostfile_readdir(w_hostfile_s* hfile,w_hostfile_s** sub);
w_uint32_t host_file_size(w_hostfile_s *hfile);


#endif
#endif
