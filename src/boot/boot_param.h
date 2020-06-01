/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_param.h
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

//The overall structure of the data stored in the flash parameter area, 
//which is not backed up in memory and read out from flash when necessary
typedef struct 
{
    w_uint32_t magic;//Magic word 
    w_uint32_t lenth;//Length of the structure of this parameter
    
    w_uint32_t version;    //bootloader version
    w_uint8_t debug_mode; //Debug mode flag
    w_uint8_t wait_sec;   //Seconds to wait for keyboard events
    w_uint8_t run_type;   //Running in RAM or Flash
    w_uint8_t encrypt_type;//Program encryption method
    w_uint8_t lock_en;     //Chip lock enable
    w_int8_t part_cnt;     //Partition count
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

