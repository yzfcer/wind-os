/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: hostfs.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.09.08
**��        ��: hostfs�������ļ�ϵͳ���⹦��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.09.08
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.09.08
** �衡��: 
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
#define HFILE_NAME_LEN 64    //�ļ�������
#define HOSTFS_DIR_LAYCNT 32 //Ŀ¼���
#define HOSTFS_MAX_FILE_SIZE 0x7fffffff //�ļ��������ƣ�2GB

#define HFMODE_R   0x01
#define HFMODE_W   0x02
#define HFMODE_RW  0x03
#define HFMODE_CRT 0x04


//�ļ�����
#define HFILE_ATTR_DIR    (0x01 << 0) //�Ƿ�Ŀ¼
#define HFILE_ATTR_RDEN   (0x01 << 1) //�Ƿ�ɶ�
#define HFILE_ATTR_WREN   (0x01 << 2) //�Ƿ��д
#define HFILE_ATTR_HIDE   (0x01 << 3) //�Ƿ�����
#define HFILE_ATTR_VERIFY (0x01 << 4) //�Ƿ�У��
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

//����������ļ�ϵͳ��Ϣ
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

//����������ļ���Ϣ
struct __hostfile_s
{
    w_uint32_t magic;  //ħ����
    w_hostfs_s *hfs;   //�����ļ�ϵͳ����
    char *path;        //�ļ�·��������ڹ��ص�ַ
    char *name;        //�ļ���
    w_uint8_t mode;    //��ģʽ
    w_uint8_t attr;    //�ļ�����
    w_uint8_t isdir;   //�Ƿ�Ŀ¼
    //w_uint8_t has_sub;   //�Ƿ������ļ�
    w_hostfile_s *subhfile;//���ļ�(��Ŀ¼)
    FILE* fd;          //�ļ����
    w_subinfo_s subinfo;
#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
    intptr_t handle;  //���ļ����(windowsϵͳ)
    _finddata_t finddata;//���ļ���Ϣ
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
