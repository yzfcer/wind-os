/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_framework.h
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2017/04/08
** Description : 
** Others      : 
** Histor      :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#ifndef MAIN_H__
#define MAIN_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    BOOT_INIT = 0,
    BOOT_CHK_FIRST,
    BOOT_CHK_DEBUG,
    BOOT_CHK_IMG,
    BOOT_CHK_LOCK,
    BOOT_CHK_UPG,
    BOOT_WAIT_INPUT,
    BOOT_MENU_LIST,
    BOOT_LOAD_IMG,
    BOOT_SET_SHARE,
    BOOT_RUN_SYSTEM,
    BOOT_ERROR,
    BOOT_STEP_CNT
}boot_status_e;

typedef w_err_t (*boot_handle_fn)(void);
typedef struct 
{
    boot_status_e status;
    char *stepname;
    boot_handle_fn handle;
}boot_step_s;


#ifdef __cplusplus
}
#endif
#endif

