/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_stack.h
** Author      : Jason Zhou
** Last Date   : 2013.10.05
** Description : wind-os filo stack structure model
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.10.05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.10.05
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef    WIND_STACK_H_
#define WIND_STACK_H_

#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_STACK_SUPPORT

#ifndef NOT_OK
#define NOT_OK              -3

#define STACK_OK            0
#define STACK_ERR           -1
#define STACK_FULL          -2
#define STACK_EMPTY         -3

#ifndef STACK_DATA_TYPE
#define STACK_DATA_TYPE     w_uint8_t
#endif

typedef struct __w_stack_s w_stack_s;
struct __w_stack_s
{
    STACK_DATA_TYPE     *out;
    STACK_DATA_TYPE     *in;
    STACK_DATA_TYPE     *top;
    w_uint16_t              item_cnt;
    w_uint16_t              item_max;
    w_uint8_t                item_size;
    w_uint32_t              emptycnt;
    w_uint32_t              fullcnt;
    w_err_t               (*read_empty)(w_stack_s *pstk,void *data);
    w_err_t               (*write_full)(w_stack_s *pstk,void *data);
    STACK_DATA_TYPE     buff[1];
};


w_err_t wind_stack_create(void *mem,
                          w_uint32_t size,
                          w_uint16_t item_size,
                          w_err_t (* ReadEmpty)(),
                          w_err_t (* WriteFull)());
                          


w_err_t wind_stack_read(void *mem,void *Ret);

w_err_t wind_stack_write(void *mem, void *Data);

w_uint16_t wind_stack_datalen(void *mem);

w_uint16_t wind_stack_size(void *mem);

void wind_stack_flush(void *mem);

#endif

#ifdef __cplusplus
}
#endif

#endif  //WIND_STACK_H_

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

