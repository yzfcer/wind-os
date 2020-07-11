/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_menu.h
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2017/04/08
** Description : 
** Others      : 
** History     :  
** Date        :
** Author      :
** Modify      :
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
    char key;//Command value entered
    char prio;//Menu permissions
    char prio_bak;//Backup of menu permissions
    const char *menu_item;//menu item
    menu_handle_fn handle;//Menu processing handle
}w_menu_tb_s;

w_err_t boot_run_menu(void);

#ifdef __cplusplus
}
#endif

#endif
