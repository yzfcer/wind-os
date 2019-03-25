#ifndef WIND_LISTFS_H__
#define WIND_LISTFS_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"

#if WIND_FS_SUPPORT

#define LF_FMODE_R   0x01
#define LF_FMODE_W   0x02
#define LF_FMODE_RW  0x03
#define LF_FMODE_CRT 0x04
#define LF_FMODE_A   0x08

#define LISTFS_MAGIC 0x49AC7D5349AC7D64
#define LISTFS_DIR_LAYCNT 32

#define LISTFS_BLK_SIZE 512

#define LFILE_NAME_LEN 64

#define LFILE_READ   (0x01 << 0)
#define LFILE_WRITE  (0x01 << 1)
#define LFILE_DIR    (0x01 << 2)
#define LFILE_HIDE   (0x01 << 3)
#define LFILE_VERIFY (0x01 << 4)

//�̻��ļ�ϵͳ��Ϣ
typedef struct __lfs_info_s
{
    w_uint64_t magic;//
    w_uint32_t blkcount;//������
    w_uint16_t unit_size;//�ļ���λ��С
    w_uint16_t blksize;//���С
    w_uint16_t reserve_blk;//��������
    w_uint16_t attr;//�ļ�ϵͳ����
    w_uint32_t bitmap_cnt;//λͼ����
    w_addr_t   bitmap1;//λͼ����
    w_addr_t   bitmap2;//λͼ����
    w_addr_t   root_addr;//��Ŀ¼λ��
}lfs_info_s;

//�̻��ļ�ͷ����Ϣ
typedef struct __lfile_info_s
{
    w_uint32_t magic;//ħ����
    char       name[LFILE_NAME_LEN];//�ļ���
    w_addr_t   next_addr;//�¿��ַ
    w_addr_t   brother_addr;//��һ���ļ���ַ
    w_addr_t   child_addr;//���ļ���ַ
    w_uint8_t  verify;//У��ֵ
    w_uint8_t  attr;//�Ƿ�Ŀ¼���ɶ�����д�����أ�У��
    w_uint16_t blkused;//��ǰ���ռ����
}lfile_info_s;

//�ļ����ݿ�ͷ����Ϣ
typedef struct __lfile_blkinfo_s
{
    w_uint32_t magic;//ħ����
    w_addr_t   prev_addr;//�Ͽ��ַ
    w_addr_t   next_addr;//�¿��ַ
    w_uint8_t  verify;//У��ֵ
    w_uint8_t  attr;//�Ƿ�Ŀ¼���ɶ�����д�����أ�У��
    w_uint16_t blkused;//��ǰ���ռ����
}lfile_blkinfo_s;



//����������ļ�ϵͳ��Ϣ
typedef struct __listfs_s
{
    lfs_info_s lfs_info;
    w_blkdev_s *blkdev;
    w_int32_t  free_blkidx;
    w_int32_t  free_byteidx;
}listfs_s;

//����������ļ���Ϣ
typedef struct __listfile_s
{
    lfile_blkinfo_s blkinfo;
    listfs_s *lfs;
    w_int32_t offset;
}listfile_s;


void *listfs_malloc(w_int32_t size);
w_err_t listfs_free(void *ptr);

w_err_t listfs_format(w_blkdev_s *blkdev);
w_err_t listfs_mount(listfs_s *lfs,w_blkdev_s *blkdev);


listfile_s *listfile_create(const char *path);
w_err_t listfile_rm(listfile_s *file);


listfile_s* listfile_open(const char *path,w_uint16_t mode);

w_err_t listfile_close(listfile_s* file);

w_bool_t listfile_existing(const char *path);

w_err_t listfile_seek(listfile_s* file,w_int32_t offset);

w_int32_t listfile_ftell(listfile_s* file);

w_int32_t listfile_read(listfile_s* file,w_uint8_t *buff, w_int32_t size);

w_int32_t listfile_write(listfile_s* file,w_uint8_t *buff, w_int32_t size);


listfile_s *listfile_readdir(listfile_s* file,w_int32_t index);

w_err_t listfile_fgets(listfile_s* file,char *buff, w_int32_t maxlen);

w_err_t listfile_fputs(listfile_s* file,char *buff);

#endif
#endif
