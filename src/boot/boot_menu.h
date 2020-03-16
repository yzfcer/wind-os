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
#ifndef BOOT_MENU_H__
#define BOOT_MENU_H__
#include "wind_type.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif



typedef w_err_t (*menu_handle_fn)(void);
typedef struct
{
    char key;//����ļ�ֵ
    char prio;//�˵�Ȩ��
    char prio_bak;//�˵�Ȩ�޵ı���
    const char *menu_item;//�˵���
    menu_handle_fn handle;//����ľ��
}w_menu_tb_s;

w_err_t run_menu(void);

#ifdef __cplusplus
}
#endif

#endif
