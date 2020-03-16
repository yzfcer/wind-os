#ifndef WIND_XMODEM_H__
#define WIND_XMODEM_H__
#include "wind_config.h"
#include "wind_type.h"  
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#if WIND_XMODEM_SUPPORT
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
    xm_stat_e stat;//xmodem状态几状态
    xm_dir_e dir;//传输方向
    w_uint8_t crcmode;//校验模式，1为crc，0为校验和
    w_uint8_t trychar;//控制字符
    w_uint8_t  pack_no;//正序号
    w_uint8_t  ack;//应答
    w_int8_t retry;//重试次数
    
    //帧数据缓存
    w_uint8_t *frbuff;//帧数据缓存
    w_int16_t frbuffsize;//数据帧缓存空间大小
    w_int16_t frlen;//数据帧空间长度
    w_int16_t frdatalen;//每帧中的数据部分长度
    w_int16_t fridx;//帧数据下标

    //有效数据缓存
    w_uint8_t *buff;//传输的数据缓存
    w_int16_t bufflen;//传输的数据长度
    w_int16_t buffidx;//传输的数据长度

    //传输接口
    xm_write_fn write;//输出接口
    xm_read_fn read;//输入接口
}xm_ctx_s;


w_err_t xmodem_init(xm_ctx_s *ctx,xm_dir_e dir,w_uint8_t *frbuff,
                    w_int32_t frbuff_size,
                    xm_write_fn write,xm_read_fn read);

w_int32_t xmodem_recv(xm_ctx_s *ctx,w_uint8_t *data,w_int32_t size);

w_int32_t xmodem_send(xm_ctx_s *ctx,w_uint8_t *data,w_int32_t size);

w_err_t xmodem_end(xm_ctx_s *ctx);

#endif //#if WIND_XMODEM_SUPPORT
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_XMODEM_H__

