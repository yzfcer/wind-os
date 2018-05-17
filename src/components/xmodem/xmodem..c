#include "string.h"  

#include "wind_type.h"  
#include "wind_debug.h"  
#include "wind_std.h"  
#include "wind_thread.h"  
#include "wind_crc16.h"  
#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 25

#define STAT_WAIT 0x00
#define STAT_DATA 0x01
#define STAT_DATA 0x01


static w_int32_t last_error = 0;
static w_uint8_t xbuff[1030];
w_uint32_t recv_cnt = 0;
void port_write(w_uint8_t trychar)
{
    wind_std_output(&trychar,1);
}  
  
w_uint8_t port_read(w_uint32_t time_out)
{
    w_int32_t cnt;
    w_uint8_t ch;
    w_uint32_t i,tick = time_out / 10;
    last_error = 0;
    for(i = 0;i < tick;i ++)
    {
        cnt = wind_std_input(&ch,1);
        if(cnt >= 1)
        {
            recv_cnt ++;
            return ch;
        }
            
        wind_thread_sleep(10);
    }
    last_error = 1;
    return 0;
}
  
static w_int32_t xmodem_check(w_int32_t crcmode, const w_uint8_t *buf, w_int32_t sz)
{
    w_uint16_t crc,tcrc;
    w_int32_t i;
    w_uint8_t cks = 0;
    if(crcmode)
    {
        crc = wind_crc16(buf, sz);
        tcrc =(buf[sz]<<8)+buf[sz+1];
        if(crc == tcrc)
            return 1;
    }
    else
    {
        for(i = 0;i < sz;++i)
        {
            cks += buf[i];
        }
        if(cks == buf[sz])
            return 1;
    }  
    return 0;
}  
  
static void flush_data(void)
{
    static w_uint16_t flush_cnt = 0;
    flush_cnt ++;
}
  
w_int32_t xmodem_recv(w_uint8_t *dest, w_int32_t destsz)
{
    
    w_uint8_t *p;
    w_int32_t bufsz, crcmode = 0;
    w_uint8_t trychar = 'C';
    w_uint8_t packetno = 1;
    w_int32_t i, c, len = 0;
    w_int32_t retry, retrans = MAXRETRANS;
    recv_cnt = 0;
    for(;;)
    {
        for(retry = 0;retry < 16;++retry)
        {
            if(trychar)
                port_write(trychar);
            c = port_read((DLY_1S)<<1);
            //c = port_read((DLY_1S)<<1);
            if(last_error == 0)
            {
                switch(c)
                {
                case SOH:
                    bufsz = 128;
                    goto start_recv;
                case STX:
                    bufsz = 1024;
                    goto start_recv;
                case EOT:
                    flush_data();
                    port_write(ACK);
                    return len;
                case CAN:
                    c = port_read(DLY_1S);
                    if(c == CAN)
                    {
                        flush_data();
                        port_write(ACK);
                        return -1;
                    }  
                    break;
                default:
                    break;
                }  
            }  
        }  
        if(trychar == 'C')
        {
            trychar = NAK;
            continue;
        }  
        flush_data();
        port_write(CAN);
        port_write(CAN);
        port_write(CAN);
        return -2;
start_recv:
        if(trychar == 'C') crcmode = 1;
            trychar = 0;
        p = xbuff;
        *p++ = c;
        for(i = 0;i <(bufsz+(crcmode?1:0)+3);++i)
        {
            c = port_read(DLY_1S);
  
            if(last_error != 0)
                goto reject;
            *p++ = c;
        }  
  
        if(xbuff[1] ==(w_uint8_t)(~xbuff[2]) &&  
           (xbuff[1] == packetno || xbuff[1] ==(w_uint8_t)packetno-1) &&  
            xmodem_check(crcmode, &xbuff[3], bufsz))
        {
            if(xbuff[1] == packetno) 
            {
                w_int32_t count = destsz - len;
                if(count > bufsz)
                    count = bufsz;
                if(count > 0)
                {
                    memcpy(&dest[len], &xbuff[3], count);
                    len += count;
                }  
                ++packetno;
                retrans = MAXRETRANS+1;
            }
            if(--retrans <= 0)
            {
                flush_data();
                port_write(CAN);
                port_write(CAN);
                port_write(CAN);
                return -3;
            }  
            port_write(ACK);
            continue;
        }  
reject:
        flush_data();
        port_write(NAK);
    }
}  
  
w_int32_t xmodem_send(w_uint8_t *src, w_int32_t srcsz)
{
    w_int32_t bufsz, crcmode = -1;
    w_uint8_t packetno = 1;
    w_int32_t i, c, len = 0;
    w_int32_t retry;
  
    for(;;) 
    {
        for( retry = 0;retry < 16;++retry)
        {
            c = port_read((DLY_1S)<<1);
            if(last_error == 0)
            {
                switch(c)
                {
                case 'C':
                    crcmode = 1;
                    goto start_trans;
                case NAK:
                    crcmode = 0;
                    goto start_trans;
                case CAN:
                    c = port_read(DLY_1S);
                    if(c == CAN)
                    {
                        port_write(ACK);
                        flush_data();
                        return -1;
                    }  
                    break;
                default:
                    break;
                }  
            }  
        }  
        port_write(CAN);
        port_write(CAN);
        port_write(CAN);
        flush_data();
        return -2;
  
        for(;;)
        {
start_trans:
            xbuff[0] = SOH;bufsz = 128;
            xbuff[1] = packetno;
            xbuff[2] = ~packetno;
            c = srcsz - len;
            if(c > bufsz) 
                c = bufsz;
            if(c >= 0)
            {
                memset(&xbuff[3], 0, bufsz);
                if(c == 0)
                {
                    xbuff[3] = CTRLZ;
                }  
                else  
                {
                    memcpy(&xbuff[3], &src[len], c);
                    if(c < bufsz) 
                        xbuff[3+c] = CTRLZ;
                }  
                if(crcmode)
                {
                    w_uint16_t ccrc = wind_crc16(&xbuff[3], bufsz);
                    xbuff[bufsz+3] =(ccrc>>8) & 0xFF;
                    xbuff[bufsz+4] = ccrc & 0xFF;
                }  
                else  
                {
                    w_uint8_t ccks = 0;
                    for(i = 3;i < bufsz+3;++i)
                    {
                        ccks += xbuff[i];
                    }  
                    xbuff[bufsz+3] = ccks;
                }  
                for(retry = 0;retry < MAXRETRANS;++retry)
                {
                    for(i = 0;i < bufsz+4+(crcmode?1:0);++i)
                    {
                        port_write(xbuff[i]);
                    }  
                    c = port_read(DLY_1S);
                    if(last_error == 0 )
                    {
                        switch(c)
                        {
                        case ACK:
                            ++packetno;
                            len += bufsz;
                            goto start_trans;
                        case CAN:
                            c = port_read(DLY_1S);
                            if( c == CAN)
                            {
                                port_write(ACK);
                                flush_data();
                                return -1;
                            }  
                            break;
                        case NAK:
                        default:
                            break;
                        }  
                    }  
                }  
                port_write(CAN);
                port_write(CAN);
                port_write(CAN);
                flush_data();
                return -4;
            }  
            else  
            {
                for(retry = 0;retry < 10;++retry)
                {
                    port_write(EOT);
                    c = port_read((DLY_1S)<<1);
                    if(c == ACK) 
                        break;
                }  
                flush_data();
                return(c == ACK)?len:-5;
            }  
        }  
    }  
}  
