#include "tb_entry.h"
#include "wind_string.h"
#include "wind_debug.h"
//#include "db_adapter.h"
#define TB_MAGIC 0xa527e397a167e268

static w_int32_t get_tb_hash(char *tbname)
{
    w_int32_t i,len;
    w_int32_t hash = 0;
    len = wind_strlen(tbname);
    WIND_ASSERT_RETURN(len < DB_NAME_LEN,-1);
    for(i = 0;i < len;i ++)
        hash += tbname[i];
    return hash;
}

static w_err_t tb_name_split(char *combine_name,char *dname,char *tname)
{
    w_int32_t len;
    char *offset = wind_strstr(combine_name,".");
    WIND_ASSERT_RETURN(offset != W_NULL,W_ERR_INVALID);
    len  = (w_int32_t)(offset - combine_name);
    WIND_ASSERT_RETURN(len < DB_NAME_LEN,W_ERR_INVALID);
    wind_memcpy(dname,combine_name,len);
    dname[len] = 0;
    offset ++;
    len = wind_strlen(offset);
    WIND_ASSERT_RETURN(len < TB_NAME_LEN,W_ERR_INVALID);
    wind_strcpy(tname,offset);
    return W_ERR_OK;
}


static w_int32_t calc_tb_entry_size(w_int32_t item_cnt)
{
    w_int32_t size = sizeof(w_tb_s);
    size = (((size + 7) >> 3) << 3);
    size += MBR_NAME_LEN * item_cnt;//item name
    size = (((size + 7) >> 3) << 3);
    size += sizeof(w_int8_t) * item_cnt;//type
    size = (((size + 7) >> 3) << 3);
    size += sizeof(w_int8_t) * item_cnt;//count
    size = (((size + 7) >> 3) << 3);
    size += sizeof(w_int16_t) * item_cnt;//offset
    size = (((size + 7) >> 3) << 3);
    size += sizeof(w_int16_t) * item_cnt;//size
    size = (((size + 7) >> 3) << 3);
    size += sizeof(w_int16_t) * item_cnt;//attr
    size = (((size + 7) >> 3) << 3);
    return size;
}

w_tb_s *tb_entry_get_byname_from_db(char *dbname,char *tbname)
{
    w_db_s *db;
    w_tb_s *tb;
    w_dnode_s *dnode;
    w_int32_t hash = get_tb_hash(tbname);
    WIND_ASSERT_RETURN(hash > 0,W_NULL);
    db = db_get_byname(dbname);
    WIND_ASSERT_RETURN(db != W_NULL,W_NULL);
    foreach_node(dnode,&db->tblist)
    {
        tb = NODE_TO_TBENTRY(dnode);
        if(hash != tb->hash)
            continue;
        if(wind_strcmp(tb->tbname,tbname) == 0)
            return tb;
    }
    return W_NULL;

}


static w_uint16_t set_offset(w_uint16_t idx,w_uint16_t count,w_int16_t *va,w_uint16_t size)
{
    w_uint16_t si;
    *va = (((idx + 7) >> 3) << 3);
    si = (((size * count + 7) >> 3) << 3); 
    return si;
}

static w_int32_t get_mbr_index(w_tb_s *tb,char *mbrname)
{
    w_int32_t i;
    char *name;
    for(i = 0;i < tb->item_cnt;i ++)
    {
        name = (char*)db_get_addr(tb,tb->mbrname_offset + i * MBR_NAME_LEN);
        if(wind_strcmp(mbrname,name) == 0)
            return i;
    }
    return -1;
}

w_err_t tb_entry_create(char *tbname,tbmodel_item_s *item_info,w_uint16_t item_cnt)
{
    w_int32_t i;
    w_err_t err;
    w_tb_s *tb;
    w_uint16_t offset;
    char *name_base;
    w_uint8_t *type,*count;
    w_uint16_t *pos,*psize,*pattr;
    w_int32_t size;

    do
    {
        err = W_ERR_OK;
        size = calc_tb_entry_size(item_cnt);
        tb = db_malloc(size);
        WIND_ASSERT_BREAK(tb != W_NULL,W_ERR_MEM,"alloc tb failed");
        wind_memset(tb,0,size);
        tb->magic = TB_MAGIC;
        err = tb_name_split(tbname,tb->dbname,tb->tbname);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_INVALID,"split tb name failed");
        tb->base = (w_addr_t)tb;
        tb->db = db_get_byname(tb->dbname);
        WIND_ASSERT_BREAK(tb->db != W_NULL,W_ERR_INVALID,"db is NOT eist");
        DNODE_INIT(tb->tbnode);

        tb->entry_size = size;
        tb->item_cnt = item_cnt;
        tb->hash = get_tb_hash(tb->tbname);

        DLIST_INIT(tb->data_list);
        tb->data_cnt = 0;
        tb->data_size = item_info[item_cnt-1].offset + item_info[item_cnt-1].size;
        
        offset = sizeof(w_tb_s);
        offset = (((offset + 7) >> 3) << 3);
        offset += set_offset(offset,tb->item_cnt,&tb->mbrname_offset,MBR_NAME_LEN);
        offset += set_offset(offset,tb->item_cnt,&tb->type_offset,sizeof(item_info->type));
        offset += set_offset(offset,tb->item_cnt,&tb->count_offset,sizeof(item_info->count));
        offset += set_offset(offset,tb->item_cnt,&tb->offset_offset,sizeof(item_info->offset));
        offset += set_offset(offset,tb->item_cnt,&tb->size_offset,sizeof(item_info->size));
        offset += set_offset(offset,tb->item_cnt,&tb->attr_offset,sizeof(item_info->attr));

        name_base = (char*)db_get_addr(tb,tb->mbrname_offset);
        type = (w_uint8_t*)db_get_addr(tb,tb->type_offset);
        count = (w_uint8_t*)db_get_addr(tb,tb->count_offset);
        pos = (w_uint16_t*)db_get_addr(tb,tb->offset_offset);
        psize = (w_uint16_t*)db_get_addr(tb,tb->size_offset);
        pattr = (w_uint16_t*)db_get_addr(tb,tb->attr_offset);
        for(i = 0;i < tb->item_cnt;i ++)
        {
            wind_strncpy(name_base,item_info[i].name,MBR_NAME_LEN);
            name_base += MBR_NAME_LEN;
            type[i] = item_info[i].type;
            count[i] = item_info[i].count;
            pos[i] = item_info[i].offset;
            psize[i] = item_info[i].size;
            pattr[i] = item_info[i].attr;
        }
        err = db_entry_insert_tb(tb->db,tb);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"insert tb failed");
        wind_mutex_init(&tb->mutex,tb->tbname);
    }while(0);
    
    if(err != W_ERR_OK)
    {
        if(tb != W_NULL)
        {
            if(tb->mutex.obj.magic == WIND_MUTEX_MAGIC)
                wind_mutex_destroy(&tb->mutex);
            db_free(tb);
        }
        
    }


    
    return W_TRUE;
}

w_tb_s *tb_entry_get_byname(char *combine_name)
{
    w_err_t err;
    char dname[TB_NAME_LEN];
    char tname[TB_NAME_LEN];
    err = tb_name_split(combine_name,dname,tname);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    return tb_entry_get_byname_from_db(dname,tname);
}


w_bool_t tb_entry_exist(char *tbname)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    if(tb != W_NULL)
        return W_TRUE;
    return W_FALSE;
}

w_err_t tb_entry_destroy(w_tb_s *tb)
{
    
    w_dnode_s *dnode;
    w_err_t err;
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    err = db_entry_remove_tb(tb->db,tb);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    foreach_node(dnode,&tb->data_list)
    {
        dnode = dlist_remove(&tb->data_list,dnode);
        db_free((void *)dnode);
    }
    
    if(tb->mutex.obj.magic == WIND_MUTEX_MAGIC)
        wind_mutex_destroy(&tb->mutex);
    db_free((void *)tb);
    return W_ERR_OK;
}

w_err_t tb_entry_insert(w_tb_s *tb,void *data,w_int32_t datasize)
{
    w_int32_t size;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(datasize == tb->data_size,W_ERR_INVALID);

    size = sizeof(w_dnode_s) + tb->data_size;
    dnode = db_malloc(size);
    WIND_ASSERT_RETURN(dnode != 0,W_ERR_MEM);
    wind_memcpy((void*)db_get_addr(dnode,sizeof(w_dnode_s)),data,tb->data_size);
    dlist_insert_tail(&tb->data_list,dnode);
    tb->data_cnt ++;
    return W_ERR_OK;
}

w_err_t tb_entry_delete(w_tb_s *tb,w_int32_t row_idx)
{
    w_dnode_s *dnode;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb->data_cnt > 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN(row_idx < tb->data_cnt,W_ERR_INVALID);
    foreach_node(dnode,&tb->data_list)
    {
        if(idx == row_idx)
        {
            dnode = dlist_remove(&tb->data_list,dnode);
            db_free((void *)dnode);
            tb->data_cnt --;
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_get_data(w_tb_s *tb,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    w_dnode_s *dnode;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < tb->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data_size >= tb->data_size,W_ERR_INVALID);
    foreach_node(dnode,&tb->data_list)
    {
        if(idx == row_idx)
        {
            dnode = dlist_remove(&tb->data_list,dnode);
            wind_memcpy(data,(void*)db_get_addr(dnode,sizeof(w_dnode_s)),tb->data_size);
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_modify(w_tb_s *tb,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    w_dnode_s *dnode;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < tb->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data_size >= tb->data_size,W_ERR_INVALID);
    foreach_node(dnode,&tb->data_list)
    {
        if(idx == row_idx)
        {
            wind_memcpy((void*)db_get_addr(dnode,sizeof(w_dnode_s)),data,tb->data_size);
            return W_ERR_OK;
        }
        idx ++;
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_modify_value(w_tb_s *tb,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    w_dnode_s *dnode;
    w_int32_t idx = 0,item_idx;
    w_addr_t addr;
    w_uint16_t *offset;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    item_idx = get_mbr_index(tb,mbrname);
    WIND_ASSERT_RETURN(item_idx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < tb->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data_size > 0,W_ERR_INVALID);
    foreach_node(dnode,&tb->data_list)
    {
        if(idx == row_idx)
        {
            offset = (w_uint16_t*)db_get_addr(tb,tb->offset_offset);
            addr = db_get_addr(dnode,sizeof(w_dnode_s)+offset[item_idx]);
            wind_memcpy((void*)addr,data,data_size);
            return W_ERR_OK;
        }
        idx ++;
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_query_count(w_tb_s *tb,w_int32_t *count)
{
    //w_int32_t idx = 0;
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    *count = tb->data_cnt;
    return W_ERR_OK;
}



w_err_t tb_entry_query_cond_count(w_tb_s *tb,char *cond,w_int32_t *idxlist,w_int32_t cnt)
{
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    wind_printf("tb_entry_query_cond_count is NOT supported.\r\n");
    return W_ERR_FAIL;
}

w_err_t tb_entry_print_info(w_tb_s *tb)
{
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    wind_printf("\r\ntable info:\r\n");
    wind_printf("DB    name :%s\r\n",tb->dbname);
    wind_printf("table name :%s\r\n",tb->tbname);
    wind_printf("tb size :%d\r\n",tb->entry_size);
    wind_printf("tb hash :%d\r\n",tb->hash);
    wind_printf("item count :%d\r\n",tb->item_cnt);
    wind_printf("data count :%d\r\n",tb->data_cnt);
    wind_printf("data  size :%d\r\n",tb->data_size);

    wind_printf("name   offset:%d\r\n",tb->mbrname_offset);
    wind_printf("type   offset:%d\r\n",tb->type_offset);
    wind_printf("count  offset:%d\r\n",tb->count_offset);
    wind_printf("offset offset:%d\r\n",tb->offset_offset);
    wind_printf("size   offset:%d\r\n",tb->size_offset);
    wind_printf("attr   offset:%d\r\n",tb->attr_offset);
    return W_ERR_OK;
}

w_err_t tb_entry_print_data(w_tb_s *tb)
{
    w_dnode_s *dnode;
    w_uint8_t *data;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    wind_printf("|   |---<table name=%s>\r\n",tb->tbname);
    foreach_node(dnode,&tb->data_list)
    {
        data = (w_uint8_t *)&dnode[1];
        tb_print_data(tb,data,idx);
        idx ++;
    }
    wind_printf("|   |---</table name=%s>\r\n",tb->tbname);
    return W_ERR_OK;
}

w_err_t tb_entry_print_table(w_tb_s *tb)
{
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb->magic == TB_MAGIC,W_ERR_INVALID);
    wind_printf("|---<DB name=%s>\r\n",tb->dbname);
    tb_entry_print_data(tb);
    wind_printf("|---</DB name=%s>\r\n",tb->dbname);
    return W_ERR_OK;
}

