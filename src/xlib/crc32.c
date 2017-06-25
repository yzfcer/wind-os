#include "wind_types.h"
u32_t crc32_table[256];
void wind_create_crc32tb(void)
{
    u32_t c;
    u16_t i = 0;
    u8_t bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (u32_t)i;
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

u32_t wind_crc32(u32_t crc, u8_t *string, u32_t size)
{
    while(size--)
        crc = (crc >> 8)^(crc32_table[(crc ^ *string++)&0xff]);
    return crc;
}