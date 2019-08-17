/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: listfs�ļ�ϵͳ���幦��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.04.05
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.04.05
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_LISTFS_H__
#define WIND_LISTFS_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#include "listfs_fileinfo.h"
#include "listfs_blkinfo.h"
#include "listfs_bitmap.h"

#if WIND_MODULE_VFS_SUPPORT
//�ļ�����ģʽ
#define LFMODE_R   0x01
#define LFMODE_W   0x02
#define LFMODE_RW  0x03
#define LFMODE_CRT 0x04
#define LFMODE_A   0x08

#define LISTFS_MAGIC 0x49AC7D53

#define LISTFS_DIR_LAYCNT 32 //Ŀ¼���
//#define LISTFS_BLK_SIZE 512  //���С

#define LISTFS_MAX_FILE_SIZE 0x7fffffff //�ļ��������ƣ�2GB

//�ļ�����
#define LFILE_ATTR_DIR    (0x01 << 0)
#define LFILE_ATTR_READ   (0x01 << 1)
#define LFILE_ATTR_WRITE  (0x01 << 2)
#define LFILE_ATTR_HIDE   (0x01 << 3)
#define LFILE_ATTR_VERIFY (0x01 << 4)
#define LFILE_ATTR_COMMAN (LFILE_ATTR_READ | LFILE_ATTR_WRITE)

#define LFILE_IS_DIR(attr) (attr & LFILE_ATTR_DIR)

//�̻��ļ�ϵͳ��Ϣ
typedef struct __lfs_info_s
{
    w_uint32_t magic;        //ħ����
    w_uint32_t blkcount;     //������
    w_uint16_t unit_size;    //�ļ���λ��С
    w_uint16_t blksize;      //���С
    w_uint16_t reserve_blk;  //��������
    w_uint16_t attr;         //�ļ�ϵͳ����
    w_uint32_t bitmap_cnt;   //λͼ����
    w_addr_t   bitmap1_addr; //��λͼ��ַ
    w_addr_t   bitmap2_addr; //����λͼ��ַ
    w_addr_t   root_addr;    //��Ŀ¼λ��
}lfs_info_s;

//����������ļ�ϵͳ��Ϣ
typedef struct __listfs_s
{
    lfs_info_s lfs_info;  //�ļ�ϵͳ��Ϣ
    w_blkdev_s *blkdev;   //�����Ŀ��豸
    lfs_bitmap_s bitmap;  //��ʹ��λͼ
    
    //lfile_info_s root;    //��Ŀ¼��Ϣ
    w_int32_t  file_ref;  //�򿪵��ļ�����
    w_uint32_t blkused;   //�Ѿ�ʹ�õĿ�����

}w_listfs_s;

//����������ļ���Ϣ
typedef struct __listfile_s
{
    lfile_info_s info;
    w_listfs_s *lfs;
    w_uint8_t mode;
    w_int32_t offset;
    lfile_blkinfo_s *blkinfo;
    lfile_info_s *subinfo;
}w_listfile_s;

void lfs_info_be2le(lfs_info_s *info);

void *lfs_malloc(w_int32_t size);
w_err_t lfs_free(void *ptr);

w_err_t listfs_format(w_listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_init(w_listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_deinit(w_listfs_s *lfs);

w_bool_t listfile_existing(w_listfs_s *lfs,const char *path);
w_listfile_s* listfile_open(w_listfs_s *lfs,const char *path,w_uint16_t mode);
w_err_t listfile_set_attr(w_listfile_s* file,w_uint8_t attr);
w_err_t listfile_get_attr(w_listfile_s* file,w_uint8_t *attr);
w_err_t listfile_close(w_listfile_s* file);
w_err_t listfile_remove(w_listfs_s *lfs,const char *path);
w_err_t listfile_seek(w_listfile_s* file,w_int32_t offset);
w_int32_t listfile_ftell(w_listfile_s* file);
w_int32_t listfile_read(w_listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_int32_t listfile_write(w_listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_err_t listfile_fgets(w_listfile_s* file,char *buff, w_int32_t maxlen);
w_err_t listfile_fputs(w_listfile_s* file,char *buff);

w_listfile_s *listfile_readdir(w_listfile_s* file,w_int32_t index);


#endif
#endif
