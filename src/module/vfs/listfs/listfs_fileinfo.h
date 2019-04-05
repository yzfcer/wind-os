#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"

#define LISTFS_MAGIC 0x49AC7D5349AC7D64
#define LISTFILE_MAGIC 0x7D5349AC
#define LISTFILE_BLK_MAGIC 0x725A4967

#define LFILE_NAME_LEN 64    //�ļ�������

#define LFILE_LBLK_CNT 64    //ÿ���������¼�Ĺ���������


//�̻��ļ�ϵͳ��Ϣ
typedef struct __lfs_info_s
{
    w_uint64_t magic;      //ħ����
    w_uint32_t blkcount;   //������
    w_uint16_t unit_size;  //�ļ���λ��С
    w_uint16_t blksize;    //���С
    w_uint16_t reserve_blk;//��������
    w_uint16_t attr;       //�ļ�ϵͳ����
    w_uint32_t bitmap_cnt; //λͼ����
    w_addr_t   bitmap1;    //λͼ����
    w_addr_t   bitmap2;    //λͼ����
    w_addr_t   root_addr;  //��Ŀ¼λ��
}lfs_info_s;


//�ļ����ݿ�ͷ����Ϣ
typedef struct __lfile_blkinfo_s
{
    w_uint32_t magic;     //����ħ����
    w_addr_t   self_addr;    //��ǰ��ַ
    w_addr_t   prevblk_addr; //�Ͽ��ַ
    w_addr_t   nextblk_addr; //�¿��ַ
    w_int32_t  offset;       //��ǰ���Ӧ���ļ�����ʼƫ����
    w_int32_t  blkused;      //��ǰ���Ѿ�ʹ�õ�����
    w_int32_t  byteused;     //��ǰ���Ѿ�ʹ�õ��ֽ�����
}lfile_blkinfo_s;

//�̻��ļ�ͷ����Ϣ
typedef struct __lfile_info_s
{
    w_uint32_t magic;                //ħ����
    char       name[LFILE_NAME_LEN]; //�ļ���
    w_addr_t   parent_addr;          //����ַ
    w_addr_t   prevfile_addr;        //��һ���ļ���ַ
    w_addr_t   nextfile_addr;        //��һ���ļ���ַ
    w_addr_t   headchild_addr;       //��һ�����ļ���ַ
    w_addr_t   tailchild_addr;       //���һ���ļ���ַ
    w_uint8_t  attr;                 //�Ƿ�Ŀ¼���ɶ�����д�����أ�У��
    lfile_blkinfo_s blkinfo;
}lfile_info_s;

w_err_t listfs_get_fsinfo(lfs_info_s *fsinfo,w_blkdev_s *blkdev);

w_err_t listfs_fileinfo_init(lfile_info_s *info,char *name,
    w_addr_t self_addr,w_addr_t parent_addr,w_addr_t prev_addr,w_uint8_t attr);

w_err_t listfs_read_block(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t **blk);

w_err_t listfs_get_fileinfo(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr);

w_err_t listfs_set_fileinfo(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr);

lfile_info_s *fileinfo_parent(lfile_info_s *info,w_blkdev_s *blkdev);

lfile_info_s *fileinfo_next(lfile_info_s *info,w_blkdev_s *blkdev);

lfile_info_s *fileinfo_headchild(lfile_info_s *info,w_blkdev_s *blkdev);

lfile_info_s *fileinfo_tailchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_next(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

#endif

