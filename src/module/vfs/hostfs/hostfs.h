/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: hostfs.h
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
#endif

#if (HOST_OS_TYPE == HOST_OS_LINUX)
#include <stdio.h>
#include <sys/stat.h>
#endif

#if WIND_HOSTFS_SUPPORT

#define HOSTFS_MAGIC   0x49AC7D53
#define HOSTFILE_MAGIC 0x2576DA83
#define HFILE_NAME_LEN 64    //文件名长度
#define HOSTFS_DIR_LAYCNT 32 //目录深度
#define HOSTFS_MAX_FILE_SIZE 0x7fffffff //文件长度限制，2GB

#define HFMODE_R   0x01
#define HFMODE_W   0x02
#define HFMODE_RW  0x03
#define HFMODE_CRT 0x04


//文件属性
#define HFILE_ATTR_DIR    (0x01 << 0) //是否目录
#define HFILE_ATTR_RDEN   (0x01 << 1) //是否可读
#define HFILE_ATTR_WREN   (0x01 << 2) //是否可写
#define HFILE_ATTR_HIDE   (0x01 << 3) //是否隐藏
#define HFILE_ATTR_VERIFY (0x01 << 4) //是否校验
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

#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
typedef struct _finddata_t _finddata_t;
#endif
typedef struct __hostfile_s w_hostfile_s;

typedef enum
{
    HFILE_TYPE_ERROR = 0,
    HFILE_TYPE_DIR = 1,
    HFILE_TYPE_FILE = 2,
}hfileattr_e;

//程序关联的文件系统信息
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

//程序关联的文件信息
struct __hostfile_s
{
    w_uint32_t magic;  //魔术字
    w_hostfs_s *hfs;   //主机文件系统对象
    char *path;        //文件路径，相对于挂载地址
    char *name;        //文件名
    w_uint8_t mode;    //打开模式
    w_uint8_t attr;    //文件属性
    w_uint8_t isdir;   //是否目录
    //w_uint8_t has_sub;   //是否有子文件
    w_hostfile_s *subhfile;//子文件(或目录)
    FILE* fd;          //文件句柄
    w_subinfo_s subinfo;
#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
    intptr_t handle;  //子文件句柄(windows系统)
    _finddata_t finddata;//子文件信息
#endif
};

hfileattr_e hostfs_file_type(char *path);
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
