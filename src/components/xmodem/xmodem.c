#include "xmodem.h"  
#include "wind_string.h"  
#include "wind_debug.h"  
#include "wind_crc16.h"  
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




static w_int32_t xmodem_check(w_int32_t crcmode, const w_uint8_t *buf, w_int32_t sz)
{
    w_uint16_t crc,tcrc;
    w_int32_t i;
    w_uint8_t cks = 0;
    if(crcmode)
    {
        crc = wind_crc16(buf, sz,0);
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
  
static void xm_flush_data(void)
{
    static w_uint16_t flush_cnt = 0;
    flush_cnt ++;
}






static w_err_t xm_wait_data(xm_ctx_s *ctx)
{
    w_int32_t i;
    ctx->idx = 0;
    wind_memset(ctx->buff,0,1030);
    for(i = 0;i < 30;i ++)
    {
        ctx->write(ctx->trychar);
        if(i >= 15)
        {
            ctx->trychar = NAK;
            ctx->crcmode = 0;
        }
            
        if(ctx->read(&ctx->buff[0],(DLY_1S)<<1) <= 0)
            continue;
        switch(ctx->buff[0])
        {
        case SOH:
            ctx->bufsz = 128;
            ctx->stat = XM_RECV_DATA;
            ctx->idx = 1;
            return W_ERR_OK;
        case STX:
            ctx->bufsz = 1024;
            ctx->stat = XM_RECV_DATA;
            ctx->idx = 1;
            return W_ERR_OK;
        case EOT:
            ctx->stat = XM_RECV_END;
            return W_ERR_OK;
        case CAN:
            ctx->stat = XM_ERROR;
            return W_ERR_FAIL;
        default:
            ctx->stat = XM_ERROR;
            return W_ERR_FAIL;
        }  
    }  
    ctx->stat = XM_ERROR;
    return W_ERR_FAIL;
}

static w_err_t xm_check_data_recv(xm_ctx_s *ctx)
{
    if(ctx->buff[1] != (w_uint8_t)(~ctx->buff[2]))
        return W_ERR_FAIL;
    if((ctx->buff[1] != ctx->pack_no) && (ctx->buff[1] !=(w_uint8_t)ctx->pack_no - 1))
        return W_ERR_FAIL;
    if(!xmodem_check(ctx->crcmode, &ctx->buff[3], ctx->bufsz))
        return W_ERR_FAIL;
    return W_ERR_OK;
}

static w_int32_t xm_read_and_check_data(xm_ctx_s *ctx,w_uint8_t *buff,w_int32_t len)
{
    w_int32_t i;
    w_int32_t buflen;
    buflen = (ctx->bufsz+(ctx->crcmode?1:0)+3);
    for(i = 0;i < buflen;i++)
    {
        if(ctx->read(&ctx->buff[ctx->idx],(DLY_1S)<<1) <= 0)
        {
            ctx->stat = XM_ERROR;
            return -1;
        }
        ctx->idx ++;
    }
    
    if(xm_check_data_recv(ctx) == W_ERR_OK)
    {
        buflen = len < ctx->bufsz?len:ctx->bufsz;
        wind_memcpy(buff, &ctx->buff[3], buflen);
        ctx->pack_no ++;
        ctx->retry = MAXRETRANS;
        ctx->stat = XM_RECV_DATA_FIRST;
        return buflen;
    }
    ctx->retry --;
    if(ctx->retry <= 0)
    {
        ctx->stat = XM_ERROR;
        return -1;
    }
    ctx->stat = XM_RECV_DATA_FIRST;
    ctx->trychar = NAK;
    return 0;
}

w_err_t xmodem_init_port(xm_ctx_s *ctx,xm_write_fn write,xm_read_fn read)
{
    WIND_ASSERT_RETURN(write != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(read != W_NULL,W_ERR_PTR_NULL);
    ctx->read = read;
    ctx->write = write;
    return W_ERR_OK;
}


w_err_t xmodem_start(xm_ctx_s *ctx,xm_dir_e dir,w_uint8_t *buff,w_int32_t buflen)
{
    WIND_ASSERT_RETURN(ctx != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buflen >= XMODEM_BUFF_LEN,W_ERR_INVALID);
    
    ctx->stat = XM_RECV_DATA_FIRST;
    ctx->dir = dir;
    ctx->trychar = 'C';
    ctx->crcmode = 1;
    ctx->buff = buff;
    ctx->bufsz = 0;
    ctx->idx = 0;
    ctx->pack_no = 1;
    ctx->retry = MAXRETRANS;
    ctx->write(CAN);
    return W_ERR_OK;
}

w_err_t xmodem_end(xm_ctx_s *ctx)
{
    WIND_ASSERT_RETURN(ctx != W_NULL,W_ERR_PTR_NULL);
    ctx->buff = W_NULL;
    ctx->stat = XM_INIT;
    return W_ERR_OK;
}

w_int32_t xmodem_recv(xm_ctx_s *ctx,w_uint8_t *data, w_int32_t size)
{
    w_int32_t idx = 0,len;
    for(;;)
    {
        switch(ctx->stat)
        {
        case XM_RECV_DATA_FIRST:
            xm_wait_data(ctx);
            break;
        case XM_RECV_DATA:
            len = xm_read_and_check_data(ctx,&data[idx],size - idx);
            if(len < 0)
                break;
            if(len == 0)
            {
                ctx->trychar = NAK;
                ctx->stat = XM_RECV_DATA_FIRST;
                break;
            }
            ctx->stat = XM_RECV_DATA_FIRST;
            ctx->trychar = ACK;
            idx += len;
            if(idx >= size)
                return idx;
            break;
        case XM_RECV_END:
            //xmodem_end();
            return idx;
        case XM_ERROR:
            //xmodem_end(ctx);
            //wind_error("xmodem error.");
            return -1;
        default:
            //xmodem_end();
            //wind_error("unknown xmodem status.");
            return 0;
        }
    }
    //return -1;
}
 
  
w_int32_t xmodem_send(xm_ctx_s *ctx,w_uint8_t *src, w_int32_t size)
{
    w_int32_t rest;
    w_uint8_t ch;
    w_int32_t i,len = 0;
    w_int32_t cnt;
    w_uint16_t ccrc;
    for(;;)
    {
        for(ctx->retry = 0;ctx->retry < 16;++ctx->retry)
        {
            cnt = ctx->read(&ch,(DLY_1S)<<1);
            if(cnt > 0) 
            {
                switch(ch)
                {
                case 'C':
                    ctx->crcmode = 1;
                    goto start_trans;
                case NAK:
                    ctx->crcmode = 0;
                    goto start_trans;
                case CAN:
                    cnt = ctx->read(&ch,DLY_1S);
                    if((cnt > 0)&&(ch == CAN))
                    {
                        ctx->write(ACK);
                        xm_flush_data();
                        return -1;
                    }  
                    break;
                default:
                    break;
                }  
            }  
        }  
        ctx->write(CAN);
        ctx->write(CAN);
        ctx->write(CAN);
        xm_flush_data();
        return -2;
  
        for(;;)
        {
start_trans:
            ctx->buff[0] = SOH;ctx->bufsz = 128;
            ctx->buff[1] = ctx->pack_no;
            ctx->buff[2] = ~ctx->pack_no;
            rest = size - len;
            if(rest > ctx->bufsz) 
                rest = ctx->bufsz;
            if(rest >= 0)
            {
                wind_memset(&ctx->buff[3], 0, ctx->bufsz);
                if(rest == 0)
                {
                    ctx->buff[3] = CTRLZ;
                }  
                else  
                {
                    wind_memcpy(&ctx->buff[3], &src[len], rest);
                    if(rest < ctx->bufsz) 
                        ctx->buff[3+rest] = CTRLZ;
                }  
                if(ctx->crcmode)
                {
                    ccrc = wind_crc16(&ctx->buff[3], ctx->bufsz,0);
                    ctx->buff[ctx->bufsz+3] =(ccrc>>8) & 0xFF;
                    ctx->buff[ctx->bufsz+4] = ccrc & 0xFF;
                }  
                else  
                {
                    w_uint8_t ccks = 0;
                    for(i = 3;i < ctx->bufsz+3;++i)
                    {
                        ccks += ctx->buff[i];
                    }  
                    ctx->buff[ctx->bufsz+3] = ccks;
                }  
                for(ctx->retry = 0;ctx->retry < MAXRETRANS;ctx->retry++)
                {
                    for(i = 0;i < ctx->bufsz+4+(ctx->crcmode?1:0);++i)
                    {
                        ctx->write(ctx->buff[i]);
                    }  
                    cnt = ctx->read(&ch,DLY_1S);
                    if(cnt > 0)
                    {
                        switch(ch)
                        {
                        case ACK:
                            ++ctx->pack_no;
                            len += ctx->bufsz;
                            goto start_trans;
                        case CAN:
                            cnt = ctx->read(&ch,DLY_1S);
                            if((cnt > 0)&&(ch == CAN))
                            {
                                ctx->write(ACK);
                                xm_flush_data();
                                return -1;
                            }  
                            break;
                        case NAK:
                        default:
                            break;
                        }  
                    }  
                }  
                ctx->write(CAN);
                ctx->write(CAN);
                ctx->write(CAN);
                xm_flush_data();
                return -4;
            }  
            else  
            {
                for(ctx->retry = 0;ctx->retry < 10;++ctx->retry)
                {
                    ctx->write(EOT);
                    cnt = ctx->read(&ch,(DLY_1S)<<1);
                    if((cnt > 0)&&(ch == ACK))
                    if(ch == ACK) 
                        break;
                }  
                xm_flush_data();
                return(ch == ACK)?len:-5;
            }  
        }  
    }  
}  

#endif

