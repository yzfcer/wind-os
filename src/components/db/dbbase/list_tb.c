#include "wind_type.h"
#include "db_if.h"
#include "wind_debug.h"

TABLE_DECLARE(tb_demo);
TABLE_DECLARE(tb_demo1);

static w_dlist_s tblist;
static w_tb_s *wind_tb_get(const char *name)
{
    return (w_tb_s*)wind_obj_get(name,&tblist);
}


w_err_t wind_tb_register(tb_param_s *tb,w_int32_t count)
{
    w_int32_t i;
    w_tb_s *tbl;    
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(tb[i].obj.magic == (~TB_PARA_MAGIC),W_ERR_INVALID);
        WIND_ASSERT_RETURN(tb[i].obj.name != W_NULL,W_ERR_PTR_NULL);
        wind_notice("register table:%s",tb[i].obj.name);
        tbl = wind_tb_get(tb[i].obj.name);
        if(tbl != W_NULL)
        {
            wind_notice("table has been registered.\r\n");
            continue;
        }
        //tb[i].mutex = wind_mutex_create(tb[i].obj.name);
        wind_obj_init(&tb[i].obj,TB_PARA_MAGIC,tb[i].obj.name,&tblist);
    }
    return W_ERR_OK;
}

w_err_t wind_tb_unregister(tb_param_s *tb)
{
    w_err_t err;
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->obj.magic == TB_PARA_MAGIC,W_ERR_INVALID);
    wind_notice("unregister table:%s",tb->obj.name);
    err = wind_obj_deinit(&tb->obj,TB_PARA_MAGIC,&tblist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    //wind_mutex_destroy(tb->mutex);
    //tb->mutex = W_NULL;
    return W_ERR_OK;
}

w_err_t wind_tbs_register(void)
{
    wind_tb_register(TABLE(tb_demo),1);
    wind_tb_register(TABLE(tb_demo1),1);
}
