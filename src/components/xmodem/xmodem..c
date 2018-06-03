#include "wind_config.h"
#include "wind_type.h"  
#include "wind_string.h"  
#include "wind_debug.h"  
#include "wind_std.h"  
#include "wind_thread.h"  
#include "wind_crc16.h"  
#include "wind_heap.h"  
#if WIND_XMODEM_SUPPORT

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


typedef enum 
{
    XM_RECV_INIT = 0,
    XM_RECV_DATA_FIRST,
    XM_RECV_DATA,
    XM_RECV_END,
    XM_ERROR
}xm_stat_e;

typedef struct 
{
    xm_stat_e stat;
    w_uint8_t trychar;
    w_uint8_t *buff;
    w_int16_t bufsz;
    w_int16_t idx;
    w_int8_t  pack_no;
    w_int32_t crcmode;
    w_int32_t retry;
}xmodem_info_s;

xmodem_info_s xm_info;


static w_int32_t last_error = 0;
static w_uint8_t xbuff[1030];
void xm_write(w_uint8_t trychar)
{
    wind_std_output(&trychar,1);
}

static w_int32_t xm_read(w_uint8_t *ch,w_uint32_t time_out)
{
    w_int32_t cnt;
    w_uint32_t i;
    w_uint32_t tick = time_out / 10;
    for(i = 0;i < tick;i ++)
    {
        cnt = wind_std_input(ch,1);
        if(cnt >= 1)
            return 1;
        wind_thread_sleep(10);
    }
    return 0;
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



void xmodem_recv_start(void)
{
    xmodem_info_s *info = &xm_info;
    info->stat = XM_RECV_DATA_FIRST;
    info->trychar = 'C';
    info->crcmode = 1;
    if(info->buff == NULL)
    info->buff = wind_malloc(1030);
    info->bufsz = 0;
    info->idx = 0;
    info->pack_no = 1;
    info->retry = MAXRETRANS;
    xm_write(CAN);
}

void xmodem_recv_end(void)
{
    xmodem_info_s *info = &xm_info;
    if(info->buff != NULL)
        wind_free(info->buff);
    info->buff = NULL;
    info->stat = XM_RECV_INIT;
}



static w_err_t wait_data(void)
{
    w_int32_t i;
    xmodem_info_s *info = &xm_info;
    info->idx = 0;
    wind_memset(info->buff,0,1030);
    for(i = 0;i < 30;i ++)
    {
        xm_write(info->trychar);
        if(i >= 15)
        {
            info->trychar = NAK;
            info->crcmode = 0;
        }
            
        if(xm_read(&info->buff[0],(DLY_1S)<<1) <= 0)
            continue;
        switch(info->buff[0])
        {
        case SOH:
            info->bufsz = 128;
            info->stat = XM_RECV_DATA;
            info->idx = 1;
            return ERR_OK;
        case STX:
            info->bufsz = 1024;
            info->stat = XM_RECV_DATA;
            info->idx = 1;
            return ERR_OK;
        case EOT:
            info->stat = XM_RECV_END;
            return ERR_OK;
        case CAN:
            info->stat = XM_ERROR;
            return ERR_FAIL;
        default:
            info->stat = XM_ERROR;
            return ERR_FAIL;
        }  
    }  
    info->stat = XM_ERROR;
    return ERR_FAIL;
}

static w_err_t check_data_recv(void)
{
    xmodem_info_s *info = &xm_info;
    if(info->buff[1] != (w_uint8_t)(~info->buff[2]))
        return ERR_FAIL;
    if((info->buff[1] != info->pack_no) && (info->buff[1] !=(w_uint8_t)info->pack_no - 1))
        return ERR_FAIL;
    if(!xmodem_check(info->crcmode, &info->buff[3], info->bufsz))
        return ERR_FAIL;
    return ERR_OK;
}

w_int32_t read_and_check_data(w_uint8_t *buff,w_int32_t len)
{
    w_int32_t i;
    w_int32_t buflen;
    xmodem_info_s *info = &xm_info;    
    buflen = (info->bufsz+(info->crcmode?1:0)+3);
    for(i = 0;i < buflen;i++)
    {
        if(xm_read(&info->buff[info->idx],(DLY_1S)<<1) <= 0)
        {
            info->stat = XM_ERROR;
            return -1;
        }
            
        info->idx ++;
    }
    
    if(check_data_recv() == ERR_OK)
    {
        buflen = len < info->bufsz?len:info->bufsz;
        wind_memcpy(buff, &info->buff[3], buflen);
        info->pack_no ++;
        info->retry = MAXRETRANS;
        info->stat = XM_RECV_DATA_FIRST;
        return buflen;
    }
    info->retry --;
    if(info->retry <= 0)
    {
        info->stat = XM_ERROR;
        return -1;
    }
    info->stat = XM_RECV_DATA_FIRST;
    info->trychar = NAK;
    return 0;
}


w_int32_t xmodem_recv_data(w_uint8_t *data, w_int32_t size)
{
    //w_int32_t i;
    //w_err_t err;
    w_int32_t idx = 0,len;    
    xmodem_info_s *info = &xm_info;
    
    for(;;)
    {
        switch(info->stat)
        {
        case XM_RECV_DATA_FIRST:
            wait_data();
            break;
        case XM_RECV_DATA:
            len = read_and_check_data(&data[idx],size - idx);
            if(len < 0)
                break;
            if(len == 0)
            {
                info->trychar = NAK;
                info->stat = XM_RECV_DATA_FIRST;
                break;
            }
            info->stat = XM_RECV_DATA_FIRST;
            info->trychar = ACK;
            idx += len;
            if(idx >= size)
                return idx;
            break;
        case XM_RECV_END:
            //xmodem_recv_end();
            return idx;
        case XM_ERROR:
            xmodem_recv_end();
            wind_error("xmodem error.");
            return -1;
        default:
            //xmodem_recv_end();
            //wind_error("unknown xmodem status.");
            return 0;
        }
    }
    //return -1;
}
 
  
w_int32_t xmodem_send(w_uint8_t *src, w_int32_t srcsz)
{
    w_int32_t bufsz, crcmode = -1;
    w_uint8_t packetno = 1,c;
    w_int32_t i,len = 0;
    w_int32_t retry;
    w_int32_t cnt;
  
    for(;;) 
    {
        for( retry = 0;retry < 16;++retry)
        {
            cnt = xm_read(&c,(DLY_1S)<<1);
            if(cnt > 0)
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
                    cnt = xm_read(&c,DLY_1S);
                    if((cnt > 0)&&(c == CAN))
                    {
                        xm_write(ACK);
                        flush_data();
                        return -1;
                    }  
                    break;
                default:
                    break;
                }  
            }  
        }  
        xm_write(CAN);
        xm_write(CAN);
        xm_write(CAN);
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
                wind_memset(&xbuff[3], 0, bufsz);
                if(c == 0)
                {
                    xbuff[3] = CTRLZ;
                }  
                else  
                {
                    wind_memcpy(&xbuff[3], &src[len], c);
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
                        xm_write(xbuff[i]);
                    }  
                    cnt = xm_read(&c,DLY_1S);
                    if(cnt > 0)
                    {
                        switch(c)
                        {
                        case ACK:
                            ++packetno;
                            len += bufsz;
                            goto start_trans;
                        case CAN:
                            cnt = xm_read(&c,DLY_1S);
                            if((cnt > 0)&&(c == CAN))
                            {
                                xm_write(ACK);
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
                xm_write(CAN);
                xm_write(CAN);
                xm_write(CAN);
                flush_data();
                return -4;
            }  
            else  
            {
                for(retry = 0;retry < 10;++retry)
                {
                    xm_write(EOT);
                    cnt = xm_read(&c,(DLY_1S)<<1);
                    if((cnt > 0)&&(c == ACK))
                    if(c == ACK) 
                        break;
                }  
                flush_data();
                return(c == ACK)?len:-5;
            }  
        }  
    }  
}  

#endif

