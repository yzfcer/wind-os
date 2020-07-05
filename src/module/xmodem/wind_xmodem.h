/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_xmodem.h
** Author      : Jason Zhou
** Last Date   : 2013.11.27
** Description : xmodem protocol function
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
#ifndef WIND_XMODEM_H__
#define WIND_XMODEM_H__
#include "wind_config.h"
#include "wind_type.h"  
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#if WIND_MODULE_XMODEM_SUPPORT
#define XMODEM_BUFF_LEN 1030
#define XMODEM_MIN_LEN 128
typedef enum 
{
    XM_IDLE = 0,
    XM_RECV_WAIT_HEAD,
    XM_RECV_DATA,
    XM_RECV_COPY_DATA,
    XM_RECV_ACK,
    XM_RECV_CAN,
    XM_RECV_EOT,
    XM_SEND_WAIT_START,
    XM_SEND_PACK_DATA,
    XM_SEND_DATA,
    XM_SEND_WAIT_ACK,
    XM_ERROR
}xm_stat_e;

typedef enum 
{
    XM_DIR_INIT = 0,
    XM_DIR_RECV = 1,
    XM_DIR_SEND = 2,
}xm_dir_e;


typedef w_int32_t (*xm_write_fn)(w_uint8_t ch);
typedef w_int32_t (*xm_read_fn)(w_uint8_t *ch,w_uint32_t time_out);

typedef struct 
{
    xm_stat_e stat;//xmodem FSM status
    xm_dir_e dir;//Transmission direction
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
    xm_write_fn write;//Output interface
    xm_read_fn read;//Input interface
}xm_ctx_s;


w_err_t wind_xmodem_init(xm_ctx_s *ctx,xm_dir_e dir,w_uint8_t *frbuff,
                    w_int32_t frbuff_size,
                    xm_write_fn write,xm_read_fn read);

w_int32_t wind_xmodem_recv(xm_ctx_s *ctx,w_uint8_t *data,w_int32_t size);

w_int32_t wind_xmodem_send(xm_ctx_s *ctx,w_uint8_t *data,w_int32_t size);

w_err_t wind_xmodem_end(xm_ctx_s *ctx);

#endif //#if WIND_MODULE_XMODEM_SUPPORT
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_XMODEM_H__

