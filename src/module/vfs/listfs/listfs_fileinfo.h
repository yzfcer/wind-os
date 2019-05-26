/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_fileinfo.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: �ļ�ϵͳ�Կ��豸�Ĳ����ӿ�
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
#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"

#define LISTFS_MAGIC 0x49AC7D5349AC7D64
#define LISTFILE_MAGIC 0x7D5349AC
#define LISTFILE_BLK_MAGIC 0x725A4967

#define LFILE_NAME_LEN 64    //�ļ�������

//��fileinfo���ݿ���ȡ��blkinfo
#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]

#define BLKINFO_HAS_OFFSET(info,ofst,blksize) ((ofst >= info->offset)&&(ofst < info->offset + info->blkused * blksize))

//�̻��ļ�ϵͳ��Ϣ
typedef struct __lfs_info_s
{
    w_uint64_t magic;        //ħ����
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


//�̻��ļ�ͷ����Ϣ
typedef struct __lfile_info_s
{
    w_uint32_t magic;                //ħ����
    char       name[LFILE_NAME_LEN]; //�ļ���
    w_int32_t  filesize;             //�ļ���С
    w_int32_t  spacesize;            //�ļ��ռ��С
    w_addr_t   parent_addr;          //����ַ
    w_addr_t   self_addr;            //��ǰ��ַ
    w_addr_t   last_addr;            //���һ������Ϣ��ַ
    w_addr_t   prevfile_addr;        //��һ���ļ���ַ
    w_addr_t   nextfile_addr;        //��һ���ļ���ַ
    w_addr_t   headchild_addr;       //��һ�����ļ���ַ
    w_addr_t   tailchild_addr;       //���һ���ļ���ַ
    w_uint8_t  attr;                 //�Ƿ�Ŀ¼���ɶ�����д�����أ�У��
}lfile_info_s;


w_err_t listfs_fileinfo_init(lfile_info_s *info,char *name,
    w_addr_t self_addr,w_addr_t parent_addr,w_addr_t prev_addr,w_uint8_t attr);

w_err_t fileinfo_read(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr);

w_err_t fileinfo_write(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_next(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_headchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_tailchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

#endif

