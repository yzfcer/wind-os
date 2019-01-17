#ifndef XMODEM_H__
#define XMODEM_H__
#include "wind_config.h"
#include "wind_type.h"  
#if WIND_XMODEM_SUPPORT
#define XMODEM_BUFF_LEN 1030
typedef enum 
{
    XM_INIT = 0,
    XM_RECV_DATA_FIRST,
    XM_RECV_DATA,
    XM_RECV_END,
    XM_ERROR
}xm_stat_e;

typedef enum 
{
    XM_RECV = 0,
    XM_SEND = 1,
}xm_dir_e;


typedef w_int32_t (*xm_write_fn)(w_uint8_t trychar);
typedef w_int32_t (*xm_read_fn)(w_uint8_t *ch,w_uint32_t time_out);

typedef struct 
{
    xm_stat_e stat;
    xm_dir_e dir;
    w_uint8_t trychar;
    w_uint8_t *buff;
    w_int16_t bufsz;
    w_int16_t idx;
    w_int8_t  pack_no;
    w_int32_t crcmode;
    w_int32_t retry;
    xm_write_fn write;
    xm_read_fn read;
}xm_ctx_s;  

w_err_t xmodem_init_port(xm_ctx_s *ctx,xm_write_fn write,xm_read_fn read);

w_err_t xmodem_start(xm_ctx_s *ctx,xm_dir_e dir,w_uint8_t *buff,w_int32_t buflen);

w_int32_t xmodem_recv(xm_ctx_s *ctx,w_uint8_t *data, w_int32_t size);

w_int32_t xmodem_send(xm_ctx_s *ctx,w_uint8_t *data, w_int32_t size);

w_err_t xmodem_end(xm_ctx_s *ctx);

#endif
#endif

