#ifndef XMODEM_H__
#define XMODEM_H__
#include "wind_config.h"
#include "wind_type.h"  
#if WIND_XMODEM_SUPPORT

void xmodem_recv_start(void);

void xmodem_recv_end(void);

w_int32_t xmodem_recv_data(w_uint8_t *data, w_int32_t size);

#endif
#endif

