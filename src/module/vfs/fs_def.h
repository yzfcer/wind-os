/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: fs_def.h
** ��   ��   ��: Jason Zhou
** ����޸�����: 2019/1/24 20:24:37
** ��        ��: vfs�ṹ����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019/1/24 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef FS_DEF_H__
#define FS_DEF_H__

#include "wind_type.h"
#include "wind_obj.h"

#define WIND_VFS_MAGIC 0x235C79A5
#define WIND_FILE_MAGIC 0x275972D5
#define WIND_FSTYPE_MAGIC 0x3582A6B3
#define WFS_NAME_LEN 12
#define WFILE_NAME_LEN 20
#define FS_MOUNT_PATH_LEN 64
#define FS_CUR_PATH "/"

#define FMODE_R    0x01
#define FMODE_W    0x02
#define FMODE_RW   0x03
#define FMODE_CRT  0x04
#define FMODE_A    0x08

#define F_VFS_POOL (0x01 << 0) //���fs�����Ƿ�ͨ���ڴ�ط���
#define IS_F_VFS_POOL(fs) ((fs->obj.flag & F_VFS_POOL) == F_VFS_POOL)
#define SET_F_VFS_POOL(fs) (fs->obj.flag |= F_VFS_POOL)
#define CLR_F_VFS_POOL(fs) (fs->obj.flag &= (~F_VFS_POOL))

#define F_VFS_MOUNT (0x01 << 1) //���fs�����Ƿ��Ƿ��ѱ�����
#define IS_F_VFS_MOUNT(vfs) ((vfs->obj.flag & F_VFS_MOUNT) == F_VFS_MOUNT)
#define SET_F_VFS_MOUNT(vfs) (vfs->obj.flag |= F_VFS_MOUNT)
#define CLR_F_VFS_MOUNT(vfs) (vfs->obj.flag &= (~F_VFS_MOUNT))


typedef enum 
{
    FTYPE_DIR = 0x01,
    FTYPE_FILE = 0x02,
}w_ftype_e;

typedef struct __w_fsops_s w_fsops_s;
typedef struct __w_vfs_s w_vfs_s;
typedef struct __w_file_s w_file_s;

struct __w_vfs_s
{
    w_obj_s obj;
    char *mount_path;
    char *fstype;
    void *fsobj;
    void *usr_arg;
    w_blkdev_s *blkdev;
    w_fsops_s *ops;
};

struct __w_fsops_s
{
    w_obj_s obj;
    w_err_t (*opsinit)(void);//�ļ�ϵͳ��������ʼ��
    void*   (*init)(w_vfs_s *fs);//�ļ�ϵͳ���ͳ�ʼ��
    w_err_t (*deinit)(w_vfs_s *fs);//�ļ�ϵͳ���ͷ���ʼ��
    w_err_t (*format)(w_vfs_s *fs);//��ʽ��
    w_err_t (*matchfs)(char *devname);//�����豸�ļ�ϵͳ�Ƿ�ƥ��
    
    w_err_t (*open)(w_file_s *file,w_uint8_t fmode);
    w_err_t (*close)(w_file_s* file);
    w_err_t (*remove)(w_file_s* file);
    w_err_t (*readdir)(w_file_s* dir,w_file_s* child);
    w_err_t (*seek)(w_file_s* file,w_uint32_t offset);
    w_err_t (*rename)(w_file_s* file,char *newname);
    w_int32_t (*ftell)(w_file_s* file);
    w_int32_t (*read)(w_file_s* file,w_uint8_t *buff, w_int32_t size);
    w_int32_t (*write)(w_file_s* file,w_uint8_t *buff, w_int32_t size);
};

struct __w_file_s
{
    w_obj_s obj;
    char *fullpath;//�������ļ�ϵͳ·��
    char *realpath;//ʵ�ʵ��ļ�ϵͳ·��
    w_vfs_s *vfs;//�������ļ�ϵͳ
    w_mutex_s *mutex;//�ļ���
    w_file_s *childfile;//���ļ�,����Ҫ����Ŀ¼ʱʹ��
    
    void *fileobj;//ʵ���ļ�����
    w_uint8_t fmode;//��ģʽ
    w_uint8_t isdir;//�Ƿ���Ŀ¼
    w_uint32_t offset;//�ļ�ƫ��λ��
};


#define FS_OPS_DEF(fs) \
w_fsops_s fs##_ops = {\
{WIND_FSTYPE_MAGIC,#fs,{(w_dnode_s*)W_NULL,(w_dnode_s*)W_NULL},0,0},\
fs##_op_opsinit,\
fs##_op_init,\
fs##_op_deinit,\
fs##_op_format,\
fs##_op_matchfs,\
fs##_op_open,\
fs##_op_close,\
fs##_op_remove,\
fs##_op_readdir,\
fs##_op_seek,\
fs##_op_rename,\
fs##_op_ftell,\
fs##_op_read,\
fs##_op_write \
}

#endif
