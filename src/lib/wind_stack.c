/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_stack.c
** Author      : Jason Zhou
** Last Date   : 2013.10.05
** Description : wind-os filo stack structure model
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.10.05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.10.05
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_stack.h"
#include "wind_core.h"

#if WIND_STACK_SUPPORT

static w_err_t defaultreadempty(w_stack_s* pstk,void *data)
{
    pstk->emptycnt ++;
    return STACK_EMPTY;
}

static w_err_t defaultwritefull(w_stack_s* pstk,void *data)
{
    pstk->fullcnt ++;
    return STACK_FULL;
}

w_err_t wind_stack_create(void *mem,
                          w_uint32_t size,
                          w_uint16_t item_size,
                          w_err_t (* ReadEmpty)(w_stack_s* pstk,void *data),
                          w_err_t (* WriteFull)(w_stack_s* pstk,void *data)
                          )
{
    w_stack_s* pstk;
    if ((mem != W_NULL) && (size >= sizeof(w_stack_s)))
    {
        pstk = (w_stack_s* )mem;

        wind_disable_interrupt();
        if(ReadEmpty)
            pstk->read_empty = ReadEmpty;
        else
            pstk->read_empty = defaultreadempty;
        if(WriteFull)
            pstk->write_full = WriteFull;
        else
            pstk->write_full = defaultwritefull;

        pstk->item_max = (size - (w_uint32_t)(((w_stack_s*)0)->buff)) / sizeof(STACK_DATA_TYPE);
                                       
        pstk->top = pstk->buff + pstk->item_max;
        pstk->out = pstk->buff;
        pstk->in = pstk->buff;
        pstk->item_size = item_size;
        pstk->item_cnt = 0;
        pstk->emptycnt = 0;
        pstk->fullcnt = 0;
        wind_enable_interrupt();
        return STACK_OK;
    }
    else
    {
        return STACK_ERR;
    }
}


w_err_t wind_stack_read(void *mem,void *Ret)
{
    w_err_t err;
    w_int8_t i;
    w_stack_s* pstk;
    //w_uint8_t *pEnd;
    w_uint8_t *ret = (w_uint8_t *)Ret;
    err = STACK_ERR;
    if (mem != W_NULL)
    {
        pstk = (w_stack_s*)mem;
        
        wind_disable_interrupt();
        if (pstk->item_cnt >= pstk->item_size)
        {
            pstk->in = pstk->out;
            for(i = 0;i < pstk->item_size;i ++)
            {
                *(ret + i) = pstk->out[0];
                pstk->out++;
                pstk->item_cnt--;
            }
            pstk->out -= pstk->item_size;
            err = STACK_OK;
        }
        else
        {
               err = pstk->read_empty(pstk,Ret);
        }
        wind_enable_interrupt();
    }
    return err;
}


w_err_t wind_stack_write(void *mem, void *Data)
{
    w_err_t err;
    w_int8_t i;
    w_stack_s* pstk;
    //w_uint8_t *pEnd;
    w_uint8_t *data = (w_uint8_t *)Data;
    err = STACK_ERR;
    if (mem != W_NULL)
    {
        pstk = (w_stack_s* )mem;
        wind_disable_interrupt();
        if (pstk->item_cnt <= pstk->item_max - pstk->item_size)
        {
            for(i = 0;i < pstk->item_size;i ++)
            {
                pstk->in[0] = *(data + i);
                pstk->in++;
                pstk->item_cnt++;
            }
            pstk->out = pstk->in - pstk->item_size;
            err = STACK_OK;
        }
        else
        {
            err = STACK_FULL;
            if (pstk->write_full != W_NULL)
            {
                err = pstk->write_full(pstk, Data);
            }
        }
        wind_enable_interrupt();
    }
    return err;
}


w_uint16_t wind_stack_datalen(void *mem)
{
    w_uint16_t temp;
    temp = 0;
    if (mem != W_NULL)
    {
        wind_disable_interrupt();
        temp = ((w_stack_s* )mem)->item_cnt / ((w_stack_s* )mem)->item_size;
        wind_enable_interrupt();
    }
    return temp;
}



w_uint16_t wind_stack_size(void *mem)
{
    w_uint16_t temp;
    temp = 0;
    if (mem != W_NULL)
    {
        wind_disable_interrupt();
        temp = ((w_stack_s* )mem)->item_max / ((w_stack_s* )mem)->item_size;
        wind_enable_interrupt();
    }
    return temp;
}




void wind_stack_flush(void *mem)
{
    w_stack_s* pstk;
    if (mem != W_NULL)
    {
        pstk = (w_stack_s*)mem;
        wind_disable_interrupt();
        pstk->item_cnt = 0;
        pstk->top = pstk->buff + pstk->item_max;
        pstk->out = pstk->buff;
        pstk->in = pstk->buff;
        wind_enable_interrupt();
    }
}



#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

