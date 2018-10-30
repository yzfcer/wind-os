/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef BOOT_PARAM_H__
#define BOOT_PARAM_H__
#include "wind_config.h"
#include "wind_type.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_PARAM_MAGIC 0x4A586D32
#define BOOT_VERSION 0x0102

#define ARCH_NAME_LEN 24
#define CPU_NAME_LEN 24
#define BOARD_NAME_LEN 24

//�洢��flash�����������ݵ�����ṹ��
//�ⲿ���������ڴ���û�б��ݣ�����Ҫʱ��flash����
typedef struct 
{
    w_uint32_t magic;//ħ���֣����ڲ���ͷ��У��
    w_uint32_t lenth;//�������ṹ��ĳ���
    
    w_uint32_t version;    //bootloader`
    w_uint8_t debug_mode; //����ģʽ
    w_uint8_t wait_sec;   //�ȴ������¼�����
    w_uint8_t run_type;   //������RAM��Flash
    w_uint8_t encrypt_type;//�������ʹ��
    w_uint8_t lock_en;     //оƬ����ʹ��
    w_int8_t part_cnt;
    char arch_name[ARCH_NAME_LEN];
    char cpu_name[CPU_NAME_LEN];
    char board_name[BOARD_NAME_LEN];
    char run_part[PART_NAME_LEN];
    w_part_s *part;
}boot_param_s;

boot_param_s *boot_param_get(void);
boot_param_s *boot_param_from_rom(void);
void    boot_param_init(void);
w_err_t boot_param_reset(void);
w_err_t boot_param_check_valid(w_uint8_t *prmbuf);
w_err_t boot_param_read(void);
w_err_t boot_param_flush(void);

#ifdef __cplusplus
}
#endif

#endif

