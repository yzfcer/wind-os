/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_xmodem.c
** Author      : Jason Zhou
** Last Date   : 2013.11.27
** Description : xmodem protocol function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.11.27
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.11.27
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_xmodem.h"  
#include "wind_string.h"  
#include "wind_debug.h"  
#include "wind_crc16.h"  
#include "wind_log.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#if WIND_XMODEM_SUPPORT
#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 32
#define WAIT_START_SEC 60


static w_int32_t xm_check_verify(w_uint8_t *data,w_int32_t count,w_int32_t crcmode)
{
    w_int32_t i;
    w_uint16_t crc,frcrc;
    w_uint8_t cks = 0;
    if(crcmode)
    {
        crc = wind_crc16(data,count,0);
        frcrc  = data[count];
        frcrc = (frcrc<<8)+data[count+1];
        if(crc == frcrc)
            return 1;
    }
    else
    {
        for(i = 0;i < count;++i)
            cks += data[i];
        if(cks == data[count])
            return 1;
    }
    return 0;
}


static w_err_t xm_check_recv_data(xm_ctx_s *ctx)
{
    if(ctx->frbuff[1] + ctx->frbuff[2] != 255)
    {
        wind_log_error("pack_no error,%d,%d",ctx->frbuff[1],ctx->frbuff[2]);
        return W_ERR_FAIL;
    }

    if((ctx->frbuff[1] != ctx->pack_no) && (ctx->frbuff[1] !=(w_uint8_t)ctx->pack_no - 1))
    {
        wind_log_error("pack_no error,frbuff %d,ctx->pack_no %d",ctx->frbuff[1],ctx->pack_no);
        return W_ERR_FAIL;
    }
        
    if(!xm_check_verify(&ctx->frbuff[3],ctx->frdatalen,ctx->crcmode))
    {
        wind_log_error("verify error.");
        return W_ERR_FAIL;
    }
    return W_ERR_OK;
}

static void xm_write_cancel(xm_ctx_s *ctx)
{
    ctx->write(CAN);
    ctx->write(CAN);
    ctx->write(CAN);
}

static w_err_t xm_handle_head(xm_ctx_s *ctx)
{
    switch(ctx->frbuff[0])
    {
    case SOH:
        wind_log_notice("head:SOH");
        ctx->frdatalen = 128;
        ctx->frlen = ctx->frdatalen + 4;
        if(ctx->crcmode)
            ctx->frlen ++;
        ctx->fridx = 1;
        ctx->stat = XM_RECV_DATA;
        return W_ERR_OK;
    case STX:
        wind_log_notice("head:STX");
        ctx->frdatalen = 1024;
        ctx->frlen = ctx->frdatalen + 4;
        if(ctx->crcmode)
            ctx->frlen ++;
        ctx->fridx = 1;
        ctx->stat = XM_RECV_DATA;
        return W_ERR_OK;
    case EOT:
        wind_log_notice("head:EOT");
        ctx->stat = XM_RECV_EOT;
        return W_ERR_OK;
    case CAN:
        wind_log_notice("head:CAN");
        ctx->stat = XM_RECV_CAN;
        return W_ERR_FAIL;
    default:
        wind_log_notice("head:unknown");
        ctx->stat = XM_ERROR;
        return W_ERR_FAIL;
    }
}
 
static w_err_t xm_recv_wait_head(xm_ctx_s *ctx)
{
    w_int32_t i;
    wind_log_notice("XM_RECV_WAIT_HEAD");
    wind_memset(ctx->frbuff,0,XMODEM_BUFF_LEN);
    for(i = 0;i < WAIT_START_SEC;i ++)
    {
        if(i >= WAIT_START_SEC/2)
        {
            ctx->trychar = NAK;
            ctx->crcmode = 0;
        }
        ctx->write(ctx->trychar);
        if(ctx->read(&ctx->frbuff[0],(DLY_1S)<<1) <= 0)
            continue;
        wind_log_notice("get head:%d",ctx->frbuff[0]);
        return xm_handle_head(ctx);
    }
    wind_log_error("wait head timeout");
    ctx->stat = XM_ERROR;
    return W_ERR_FAIL;
}




static w_int32_t xm_recv_data(xm_ctx_s *ctx)
{
    w_int32_t i;
    w_int32_t buflen;
    wind_log_notice("XM_RECV_DATA");
    buflen = ctx->frlen;
    wind_log_notice("read data:%d",ctx->frlen);
    for(i = 1;i < buflen;i++)
    {
        if(ctx->read(&ctx->frbuff[i],(DLY_1S)<<1) <= 0)
        {
            ctx->stat = XM_ERROR;
            wind_log_error("read data error,count %d",ctx->fridx);
            return -1;
        }
        ctx->fridx ++;
    }
    wind_log_notice("read data OK");
    ctx->stat = XM_RECV_COPY_DATA;
    return ctx->frlen;
}

static w_err_t xm_recv_copy_data(xm_ctx_s *ctx)
{
    wind_log_notice("XM_RECV_COPY_DATA");
    if(xm_check_recv_data(ctx) == W_ERR_OK)
    {
        wind_log_notice("check recv data OK");
        wind_memcpy(&ctx->buff[ctx->buffidx],&ctx->frbuff[3],ctx->frdatalen);
        ctx->buffidx += ctx->frdatalen;
        ctx->pack_no ++;
        ctx->retry = 0;
        ctx->stat = XM_RECV_ACK;
        ctx->ack = ACK;
        return W_ERR_OK;
    }
    ctx->retry ++;
    if(ctx->retry <= MAXRETRANS)
    {
        wind_log_notice("retry times:%d",ctx->retry);
        ctx->ack = NAK;
        ctx->stat = XM_RECV_ACK;
        return W_ERR_FAIL;
    }
    wind_log_error("copy data failed");
    ctx->retry = 0;
    ctx->stat = XM_ERROR;
    return W_ERR_FAIL;
}

static w_err_t xm_recv_ack(xm_ctx_s *ctx)
{
    wind_log_notice("XM_RECV_ACK");
    ctx->write(ctx->ack);
    if(ctx->read(&ctx->frbuff[0],(DLY_1S)<<1) <= 0)
    {
        wind_log_error("read head failed");
        ctx->stat = XM_ERROR;
        return W_ERR_FAIL;
    }
    return xm_handle_head(ctx);
}

static w_err_t xm_recv_eot(xm_ctx_s *ctx)
{
    wind_log_notice("XM_RECV_EOT");
    ctx->write(ACK);
    ctx->write(ACK);
    ctx->stat = XM_IDLE;
    wind_log_notice("recv lenth:%d",ctx->buffidx);
    return W_ERR_OK;
}


w_err_t wind_xmodem_init(xm_ctx_s *ctx,xm_dir_e dir,
                    w_uint8_t *frbuff,w_int32_t frbuffsize,
                    xm_write_fn write,xm_read_fn read)
{
    WIND_ASSERT_RETURN(ctx != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(frbuff != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(frbuffsize >= XMODEM_BUFF_LEN,W_ERR_INVALID);
    WIND_ASSERT_RETURN(write != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(read != W_NULL,W_ERR_NULL_PTR);
    wind_log_open();
    wind_log_notice("--------------------------");
    wind_memset(ctx,0,sizeof(xm_ctx_s));
    if(dir == XM_DIR_RECV)
        ctx->stat = XM_RECV_WAIT_HEAD;
    else
        ctx->stat = XM_SEND_WAIT_START;
    ctx->dir = dir;
    ctx->crcmode = 1;
    ctx->trychar = 'C';
    ctx->pack_no = 1;
    
    ctx->frbuff = frbuff;
	ctx->frbuffsize = (w_int16_t)frbuffsize;
    
    ctx->read = read;
    ctx->write = write;
    ctx->write(CAN);
    return W_ERR_OK;
}




w_int32_t wind_xmodem_recv(xm_ctx_s *ctx,w_uint8_t *data,w_int32_t size)
{
    WIND_ASSERT_RETURN(ctx != W_NULL,-1);
    WIND_ASSERT_RETURN(data != W_NULL,-1);
    WIND_ASSERT_RETURN(size >= XMODEM_MIN_LEN,-1);
    ctx->buff = data;
    ctx->bufflen = (w_int16_t)size;
    ctx->buffidx = 0;
    
    for(;;)
    {
        switch(ctx->stat)
        {
        case XM_RECV_WAIT_HEAD:
            
            xm_recv_wait_head(ctx);
            break;
        case XM_RECV_DATA:
            xm_recv_data(ctx);
            break;
        case XM_RECV_COPY_DATA:
            xm_recv_copy_data(ctx);
            if(ctx->buffidx >= ctx->bufflen)
            {
                wind_log_notice("recv full lenth:%d",ctx->buffidx);
                return ctx->buffidx;
            }
            break;
        case XM_RECV_ACK:
            xm_recv_ack(ctx);
            break;
        case XM_RECV_EOT:
            xm_recv_eot(ctx);
            return ctx->buffidx;
        case XM_ERROR:
            wind_xmodem_end(ctx);
            return -1;
        case XM_IDLE:
        default:
            wind_log_notice("XM_IDLE");
            return 0;
        }
    }
}

static w_err_t xm_send_wait_start(xm_ctx_s *ctx)
{
    w_int32_t i,cnt;
    w_uint8_t ch;
    wind_log_notice("XM_SEND_WAIT_START");
    for(i = 0;i < 60;i++)
    {
        cnt = ctx->read(&ch,(DLY_1S)<<1);
        if(cnt <= 0)
            continue;
        switch(ch)
        {
        case 'C':
            wind_log_notice("receive C,crc mode");
            ctx->crcmode = 1;
            ctx->stat = XM_SEND_PACK_DATA;
            return W_ERR_OK;
        case NAK:
            wind_log_notice("receive NAK,sum mode");
            ctx->crcmode = 0;
            ctx->stat = XM_SEND_PACK_DATA;
            return W_ERR_OK;
        default:
            wind_log_notice("unexepcted req:0x%x",ch);
            break;    
        }
    }
    wind_log_error("wait send req failed");
    ctx->stat = XM_ERROR;
    return W_ERR_FAIL;
}

static w_err_t xm_send_wait_ack(xm_ctx_s *ctx)
{
    w_int32_t i,cnt;
    w_uint8_t ch;
    wind_log_notice("XM_SEND_WAIT_ACK");
    for(i = 0;i < 10;i++)
    {
        cnt = ctx->read(&ch,(DLY_1S)<<1);
        if(cnt <= 0)
            continue;
        switch(ch)
        {
        case ACK:
            wind_log_notice("receive ACK");
            ctx->pack_no ++;
            ctx->buffidx += ctx->frdatalen;
            ctx->stat = XM_SEND_PACK_DATA;
            return W_ERR_OK;
        case NAK:
            wind_log_notice("receive NAK,retry");
            ctx->retry ++;
            if(ctx->retry >= MAXRETRANS)
                ctx->stat = XM_ERROR;
            else
                ctx->stat = XM_SEND_PACK_DATA;
            return W_ERR_OK;
        case CAN:
            wind_log_notice("receive CAN,answer and stop");
            ctx->write(ACK);
            ctx->stat = XM_IDLE;
            return W_ERR_FAIL;
        default:
            wind_log_notice("unexepcted req:0x%x",ch);
            break;
        }
    }
    ctx->stat = XM_ERROR;
    return W_ERR_FAIL;
}


static w_int32_t xm_send_set_data_verify(xm_ctx_s *ctx)
{
    w_int32_t i;
    w_uint8_t sum = 0;
    w_uint16_t ccrc;
    if(ctx->crcmode)
    {
        ccrc = wind_crc16(&ctx->frbuff[3],ctx->frdatalen,0);
        ctx->frbuff[ctx->frdatalen+3] =((ccrc>>8) & 0xFF);
        ctx->frbuff[ctx->frdatalen+4] = (ccrc & 0xFF);
        ctx->frlen = ctx->frdatalen+5;
        return ctx->frlen;
    }
    else  
    {
        for(i = 0;i < ctx->frdatalen;i++)
        {
            sum += ctx->frbuff[i+3];
        }
        ctx->frbuff[ctx->frdatalen+3] = sum;
        ctx->frlen = ctx->frdatalen+4;
        return ctx->frlen;
    }

}

static w_int32_t xm_send_pack_data(xm_ctx_s *ctx)
{
    w_int32_t len;
    wind_log_notice("XM_SEND_PACK_DATA");
    if(ctx->buffidx >= ctx->bufflen)
    {
        wind_log_error("pack no data");
        ctx->stat = XM_ERROR;
        return -1;
    }
    wind_log_notice("pack no:%d",ctx->pack_no);
    if(ctx->bufflen >= ctx->buffidx + 128)
        len = 128;
    else
        len = ctx->bufflen - ctx->buffidx;
    ctx->frdatalen = 128;
    ctx->frlen = 128 + 4;
    if(ctx->crcmode)
        ctx->frlen ++;
    ctx->frbuff[0] = SOH;
    ctx->frbuff[1] = ctx->pack_no;
    ctx->frbuff[2] = ~ctx->pack_no;
    wind_memset(&ctx->frbuff[3],CTRLZ,ctx->frdatalen);
    wind_memcpy(&ctx->frbuff[3],&ctx->buff[ctx->buffidx],len);
    xm_send_set_data_verify(ctx);
    ctx->stat = XM_SEND_DATA;
    return ctx->frlen;
}



static void xm_send_data(xm_ctx_s *ctx)
{
    w_int32_t i;
    wind_log_notice("XM_SEND_DATA");
    wind_log_notice("xm_send_data %d",ctx->frlen);
    for(i = 0;i < ctx->frlen;i ++)
        ctx->write(ctx->frbuff[i]);
    ctx->stat = XM_SEND_WAIT_ACK;
}

static w_err_t xm_send_eot(xm_ctx_s *ctx)
{
    w_int32_t i,cnt;
    w_uint8_t ch = 0;
    for(i = 0;i < 10;i++)
    {
        ctx->write(EOT);
        cnt = ctx->read(&ch,(DLY_1S)<<1);
        if(cnt <= 0)
            continue;
        wind_log_notice("xm_send_eot recv 0x%x",ch);
        if(ch == ACK)
            return W_ERR_OK;
    }
    wind_log_error("xm_send_eot failed");
    return W_ERR_FAIL;
}

w_int32_t wind_xmodem_send(xm_ctx_s *ctx,w_uint8_t *data,w_int32_t size)
{
    WIND_ASSERT_RETURN(ctx != W_NULL,-1);
    WIND_ASSERT_RETURN(data != W_NULL,-1);
    WIND_ASSERT_RETURN(size > 0,-1);
    ctx->buff = data;
    ctx->bufflen = (w_int16_t)size;
    ctx->buffidx = 0;
    for(;;)
    {
        switch(ctx->stat)
        {
        case XM_SEND_WAIT_START:
            xm_send_wait_start(ctx);
            break;
        case XM_SEND_PACK_DATA:
            xm_send_pack_data(ctx);
            break;
        case XM_SEND_DATA:
            xm_send_data(ctx);
            break;
        case XM_SEND_WAIT_ACK:
            xm_send_wait_ack(ctx);
            if(ctx->buffidx >= size)
            {
                wind_log_notice("send block:%d",size);
                return size;
            }
            break;
        case XM_ERROR:
            wind_xmodem_end(ctx);
            return -1;
        case XM_IDLE:
        default:
            return 0;
        }
    }
}

w_err_t wind_xmodem_end(xm_ctx_s *ctx)
{
    wind_log_notice("XM_ERROR");
    WIND_ASSERT_RETURN(ctx != W_NULL,W_ERR_NULL_PTR);
    wind_log_notice("wind_xmodem_end");
    if((ctx->stat == XM_SEND_PACK_DATA)&&(ctx->dir == XM_DIR_SEND))
    {
        wind_log_notice("send eot req");
        xm_send_eot(ctx);
    }
    else if(ctx->stat != XM_IDLE)
    {
        wind_log_notice("send cancel req");
        xm_write_cancel(ctx);
    }
    ctx->stat = XM_IDLE;
    ctx->frbuff = W_NULL;
    ctx->dir = XM_DIR_INIT;
    wind_log_close();
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#if WIND_XMODEM_SUPPORT

