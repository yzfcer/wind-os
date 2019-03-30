/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: tb_print.c
**创   建   人: Jason Zhou
**最后修改日期: 2017.12.19
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2017.12.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2017.12.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_debug.h"
#include "db_def.h"

static void print_int8_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_int8_t *va = (w_int8_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%d ",va[i]);
}

static void print_uint8_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_uint8_t *va = (w_uint8_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%u ",va[i]);
}

static void print_int16_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_int16_t *va = (w_int16_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%d ",va[i]);
}

static void print_uint16_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_uint16_t *va =(w_uint16_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%u ",va[i]);
}

static void print_int32_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_int32_t *va = (w_int32_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%ld ",va[i]);
}



static void print_uint32_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_uint32_t *va = (w_uint32_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%lu ",va[i]);
}

static void print_int64_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_int64_t *va = (w_int64_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%lld ",va[i]);
}

static void print_uint64_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_uint64_t *va = (w_uint64_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%llu ",va[i]);
}

static void print_bool_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_bool_t *va = (w_bool_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("%s ",va[i]?"true":"false");
}

static void print_string_arr(void *data,w_int32_t count)
{
    w_int32_t i;
    w_str256_t *va = (w_str256_t *)data;
    for(i = 0;i < count;i ++)
        wind_printf("\"%s\" ",va[i]);
}

void tb_print_data(w_tb_s *entry,w_uint8_t *data,w_int32_t idx)
{
    w_int32_t i;
    char *name_base;
    w_uint8_t *type,*count;
    w_uint16_t *pos,*psize,*pattr;
    name_base = (char*)db_get_addr(entry,entry->mbrname_offset);
    type = (w_uint8_t*)db_get_addr(entry,entry->type_offset);
    count = (w_uint8_t*)db_get_addr(entry,entry->count_offset);
    pos = (w_uint16_t*)db_get_addr(entry,entry->offset_offset);
    psize = (w_uint16_t*)db_get_addr(entry,entry->size_offset);
    pattr = (w_uint16_t*)db_get_addr(entry,entry->attr_offset);
    wind_printf("|   |   |---<row index=%d>\r\n",idx);
    for(i = 0;i < entry->item_cnt;i ++)
    {
        wind_printf("|   |   |   |---<member %s=",&name_base[i*MBR_NAME_LEN]);
        switch(type[i])
        {
            case TYPE_w_int8_t:
                print_int8_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_int16_t:
                print_int16_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_int32_t:
                print_int32_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_int64_t:
                print_int64_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_uint8_t:
                print_uint8_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_uint16_t:
                print_uint16_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_uint32_t:
                print_uint32_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_uint64_t:
                print_uint64_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_bool_t:
                print_bool_arr(&data[pos[i]],count[i]);
                break;
            case TYPE_w_str16_t:
            case TYPE_w_str32_t:
            case TYPE_w_str64_t:
            case TYPE_w_str128_t:
            case TYPE_w_str256_t:
                print_string_arr(&data[pos[i]],count[i]);
                break;
            default:
                wind_printf("undefined param type:%d\r\n",type[i]);
                break;
        }
        wind_printf("/>\r\n");
    }
    wind_printf("|   |   |---</row index=%d>\r\n",idx);
}

