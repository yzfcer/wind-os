#ifndef WIND_CRC32_H__
#define WIND_CRC32_H__

#include "wind_types.h"

#ifdef __cplusplus
extern "C" {
#endif
void wind_create_crc32tb(void);

u32_t wind_crc32(u32_t crc, u8_t *string, u32_t size)


#ifdef __cplusplus
}
#endif

#endif