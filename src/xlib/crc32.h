#ifndef WIND_CRC32_H__
#define WIND_CRC32_H__

#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif
void wind_create_crc32tb(void);

w_uint32_t wind_crc32(w_uint32_t crc, w_uint8_t *string, w_uint32_t size)


#ifdef __cplusplus
}
#endif

#endif