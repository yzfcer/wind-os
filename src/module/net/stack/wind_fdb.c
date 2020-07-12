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
#include "wind_string.h"
#include "wind_core.h"
#include "wind_netdev.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_NET_SUPPORT

static w_fdb_s fdb_list[WIND_FDB_MAX_NUM];


static void fdb_timer(w_timer_s *timer,void *arg)
{
    wind_fdb_flush();
}

w_err_t wind_fdb_init(void)
{
    w_err_t err;
    w_timer_s *tmr;
    wind_notice("net:init fdb table");
    err = wind_fdb_clear();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    tmr = wind_timer_create("fdb",2000,fdb_timer,W_NULL,F_TIMER_RUN | F_TIMER_REPEAT);
    WIND_ASSERT_RETURN(tmr != W_NULL,err);
    return err;
}

w_err_t wind_fdb_deinit(void)
{
    w_timer_s *tmr;
    wind_notice("net:deinit fdb table");
    tmr = wind_timer_get("fdb_flush");
    if(tmr != W_NULL)
        wind_timer_destroy(tmr);
    return W_ERR_OK;
}

w_err_t wind_fdb_insert(w_fdb_s *fdb)
{
    w_int32_t i;
    w_err_t err;
    w_fdb_s *tmpfdb;
    w_uint32_t tick;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        tmpfdb = wind_fdb_get(fdb->mac);
        if(tmpfdb == W_NULL)
        {
            for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
            {
                if(fdb_list[i].enable)
                    continue;
                fdb_list[i].enable = 1;
                tmpfdb = &fdb_list[i];
                break;
            }
        }
        WIND_ASSERT_BREAK(tmpfdb != W_NULL,W_ERR_MEM,"fdb table full");
        tick = wind_get_tick();
        tick /= WIND_TICK_PER_SEC;
        wind_memcpy(tmpfdb,fdb,sizeof(w_fdb_s));
		fdb->ttl = (w_uint16_t)tick;
    }while(0);
    wind_enable_switch();
    return err;
}

w_err_t wind_fdb_remove(w_fdb_s *fdb)
{
    w_err_t err;
    w_fdb_s *tmp_fdb;
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        tmp_fdb = wind_fdb_get(fdb->mac);
        WIND_ASSERT_BREAK(tmp_fdb != W_NULL,W_ERR_FAIL,"find route table rule fail");
        if(tmp_fdb->dev_id != fdb->dev_id)
            break;
        tmp_fdb->enable = 0;
    }while(0);
    wind_enable_switch();
    return err;
}


w_err_t wind_fdb_clear(void)
{
    w_int32_t i;
    wind_disable_switch();
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        fdb_list[i].enable = 0;
    }
    wind_enable_switch();
    return W_ERR_OK;

}

w_err_t wind_fdb_flush(void)
{
    w_int32_t i;
    w_uint32_t tick;
    tick = wind_get_tick();
    tick /= WIND_TICK_PER_SEC;
    wind_disable_switch();
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        if(!fdb_list[i].enable)
            continue;
        if(tick - fdb_list[i].ttl >= WIND_FDB_TTL)
        {
            fdb_list[i].enable = 0;
            break;
        }
    }
    wind_enable_switch();
    return W_ERR_OK;
}

w_fdb_s *wind_fdb_get(w_uint8_t *mac)
{
    w_int32_t i;
    w_fdb_s *fdb = (w_fdb_s*)W_NULL;
    wind_disable_switch();
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        if(!fdb_list[i].enable)
            continue;
        if(wind_memcmp(mac,fdb_list[i].mac,6) == 0)
        {
            fdb = &fdb_list[i];
            break;
        }
    }
    wind_enable_switch();
    return fdb;
}

w_err_t wind_fdb_print(void)
{
    w_int32_t i;
    w_fdb_s *fdb;
    w_netdev_s *netdev;
    char macstr[20];
    //char ipstr[16];
    wind_print_space(7);
    wind_printf("%-20s %-8s %-8s %-12s\r\n","mac_addr","dev_id","vlanid","dev");
    wind_print_space(7);
    for(i = 0;i < WIND_FDB_MAX_NUM;i ++)
    {
        if(!fdb_list[i].enable)
            continue;
        fdb = &fdb_list[i];
        wind_mac_to_str(fdb->mac,macstr,':');
        netdev = wind_netdev_get_byid(fdb->dev_id);
        WIND_ASSERT_RETURN(netdev != W_NULL,W_ERR_FAIL);
        //wind_ip_to_str(fdb->ipaddr,ipstr);
        wind_printf("%-20s %-8s %-8d %-12s\r\n",macstr,fdb->dev_id,fdb->vlanid,netdev->netnode.obj.name);
    }
    wind_print_space(7);
    return W_ERR_OK;
}



#endif //#if WIND_MODULE_NET_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

