/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_pipe.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os pipeline function set
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_PIPE_H__
#define WIND_PIPE_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_PIPE_SUPPORT

#define WIND_PIPE_MAGIC 0x56384c6a
#define PIPE_NAME_LEN 12


//Pipe object structure
typedef struct __w_pipe_s
{
    w_obj_s obj;      //Basic object information
    void *buff;       //Pipeline cache
    w_int32_t buflen; //Pipeline cache length
}w_pipe_s;

w_err_t _wind_pipe_mod_init(void);
w_pipe_s* wind_pipe_get(const char *name);
w_err_t wind_pipe_init(w_pipe_s* pipe,const char *name,void *buff,w_int32_t buflen);
w_pipe_s* wind_pipe_create(const char *name,void *buff,w_int32_t buflen);
w_int32_t wind_pipe_read(w_pipe_s* pipe,w_int8_t *str,w_int32_t len);
w_int32_t wind_pipe_write(w_pipe_s* pipe,w_int8_t *str,w_int32_t len);
w_err_t wind_pipe_destroy(w_pipe_s* pipe);
w_err_t wind_pipe_print_list(void);
w_err_t wind_pipe_print_detail(void);


#endif // #if WIND_PIPE_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_PIPE_H__

