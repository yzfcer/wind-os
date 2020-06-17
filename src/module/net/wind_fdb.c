/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_fdb.c
** Author      : Jason Zhou
** Last Date   : 2020-06-14
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-14
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-14
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_fdb.h"
#include "wind_timer.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

static w_fdb_s fdb_list[WIND_FDB_MAX_NUM];


static w_err_t fdb_timer(w_timer_s *timer,void &arg)
{
    wind_fdb_flush();
}

w_err_t wind_fdb_init(void)
{
    w_err_t err;
    w_timer_s *tmr;
    err = wind_fdb_clear()
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    tmr = wind_timer_create("fdb_flush",2000,fdb_timer,W_NULL,F_TIMER_RUN | F_TIMER_REPEAT);
    WIND_ASSERT_RETURN(tmr != W_NULL,err);
    return err;
}

w_err_t wind_fdb_deinit(void)
{
    w_timer_s *tmr;
    tmr = wind_timer_get("fdb_flush");
    if(tmr != W_NULL)
        wind_timer_destroy(tmr);
    return W_ERR_OK;
}

w_err_t wind_fdb_update(w_uint8_t *mac,w_uint16_t vlanid,w_uint8_t portid)
{
    w_int32_t i;
    w_err_t err;
    w_fdb_s *fdb;
    w_uint32_t tick;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        wind_disable_switch();
        fdb = wind_fdb_get(mac);
        if(fdb == W_NULL)
        {
            for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
            {
                if(fdb_list[i].enable == 0)
                {
                    fdb_list[i].enable = 1;
                    fdb = &fdb_list[i]
                    break;
                }
            }
        }
        WIND_ASSERT_BREAK(fdb != W_NULL,W_ERR_MEM,"fdb table full");
        tick = wind_get_tick();
        tick /= WIND_TICK_PER_SEC;
        wind_memcpy(fdb->mac,mac,6);
        fdb->vlanid = vlanid;
        fdb->port_id = portid;
        fdb->ttl = tick;
    }while(0);
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_fdb_clear(void)
{
    w_int32_t i;
    wind_disable_switch();
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        fdb_list[i].enable = 0;
    }
    wind_disable_switch();
    return W_ERR_OK;

}

w_err_t wind_fdb_flush(void)
{
    w_int32_t i;
    w_uint32_t tick;
    w_fdb_s *fdb = W_NULL;
    tick = wind_get_tick();
    tick /= WIND_TICK_PER_SEC;
    wind_disable_switch();
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        if((fdb_list[i].enable) && (tick - fdb_list[i].ttl >= WIND_FDB_TTL))
        {
            fdb_list[i].enable = 0;
            break;
        }
    }
    wind_disable_switch();
    return W_ERR_OK;
}

w_fdb_s *wind_fdb_get(w_uint8_t *mac)
{
    w_int32_t i;
    w_fdb_s *fdb = W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        if((fdb_list[i].enable) && (wind_memcmp(mac,fdb_list[i].mac,6) == 0))
        {
            fdb = &fdb_list[i];
            break;
        }
    }
    wind_disable_switch();
    return fdb;
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

