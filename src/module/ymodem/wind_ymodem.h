/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_ymodem.h
** Author      : Jason Zhou
** Last Date   : 2013.11.27
** Description : ymodem protocol function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.11.27
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.11.27
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_YMODEM_H__
#define WIND_YMODEM_H__
#include "wind_config.h"
#include "wind_type.h"  
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#if WIND_YMODEM_SUPPORT
#define YMODEM_BUFF_LEN 1030
#define YMODEM_MIN_LEN 128
typedef enum 
{
    YM_IDLE = 0,
    YM_RECV_WAIT_HEAD,
    YM_RECV_DATA,
    YM_RECV_COPY_DATA,
    YM_RECV_ACK,
    YM_RECV_CAN,
    YM_RECV_EOT,
    YM_SEND_WAIT_START,
    YM_SEND_PACK_DATA,
    YM_SEND_DATA,
    YM_SEND_WAIT_ACK,
    YM_ERROR
}ym_stat_e;

typedef enum 
{
    YM_DIR_INIT = 0,
    YM_DIR_RECV = 1,
    YM_DIR_SEND = 2,
}ym_dir_e;


typedef w_int32_t (*ym_write_fn)(w_uint8_t ch);
typedef w_int32_t (*ym_read_fn)(w_uint8_t *ch,w_uint32_t time_out);

typedef struct 
{
    ym_stat_e stat;//ymodem FSM status
    ym_dir_e dir;//Transmission direction
    w_uint8_t crcmode;//Check mode, 1 is CRC, 0 is check sum
    w_uint8_t trychar;//Control character
    w_uint8_t  pack_no;//sequence number
    w_uint8_t  ack;//answer
    w_int8_t retry;//retry count
    
    //Frame data parameter
    w_uint8_t *frbuff;//Frame data cache
    w_int16_t frbuffsize;//Data frame cache space size
    w_int16_t frlen;//Frame length
    w_int16_t frdatalen;//Frame data length
    w_int16_t fridx;//Frame data index

    //Effective data cache
    w_uint8_t *buff;//Transmitted data cache
    w_int16_t bufflen;//Data length transmitted
    w_int16_t buffidx;//Transmitted data index

    //Transmission interface
    ym_write_fn write;//Output interface
    ym_read_fn read;//Input interface
}ym_ctx_s;


w_err_t wind_ymodem_init(ym_ctx_s *ctx,ym_dir_e dir,w_uint8_t *frbuff,
                    w_int32_t frbuff_size,
                    ym_write_fn write,ym_read_fn read);

w_int32_t wind_ymodem_recv(ym_ctx_s *ctx,w_uint8_t *data,w_int32_t size);

w_int32_t wind_ymodem_send(ym_ctx_s *ctx,w_uint8_t *data,w_int32_t size);

w_err_t wind_ymodem_end(ym_ctx_s *ctx);

#endif //#if WIND_YMODEM_SUPPORT
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_YMODEM_H__

