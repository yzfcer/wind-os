#include "wind_type.h"
w_uint32_t crc32_table[256];
void wind_create_crc32tb(void)
{
    w_uint32_t c;
    w_uint16_t i = 0;
    w_uint8_t bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (w_uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c >> 1)^(0xEDB88320);
            }
            else
            {
                c =c >> 1;
            }
        }
        crc32_table[i] = c;
    }
}

w_uint32_t wind_crc32(w_uint32_t crc, w_uint8_t *string, w_uint32_t size)
{
    while(size--)
        crc = (crc >> 8)^(crc32_table[(crc ^ *string++)&0xff]);
    return crc;
}
