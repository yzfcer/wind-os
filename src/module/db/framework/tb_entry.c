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


static w_int32_t get_tb_entry_size(w_int32_t item_cnt)
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
    w_db_s *dbentry;
    w_tb_s *tbentry;
    w_dnode_s *dnode;
    w_int32_t hash = get_tb_hash(tbname);
    WIND_ASSERT_RETURN(hash > 0,W_NULL);
    dbentry = db_get_byname(dbname);
    WIND_ASSERT_RETURN(dbentry != W_NULL,W_NULL);
    foreach_node(dnode,&dbentry->tblist)
    {
        tbentry = NODE_TO_TBENTRY(dnode);
        if(hash != tbentry->hash)
            continue;
        if(wind_strcmp(tbentry->tbname,tbname) == 0)
            return tbentry;
    }
    return W_NULL;

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

static w_uint16_t set_offset(w_uint16_t idx,w_uint16_t count,w_uint16_t *va,w_uint16_t size)
{
    w_uint16_t si;
    *va = (((idx + 7) >> 3) << 3);
    si = (((size * count + 7) >> 3) << 3); 
    return si;
}

static w_int32_t get_mbr_index(w_tb_s *entry,char *mbrname)
{
    w_int32_t i;
    char *name;
    for(i = 0;i < entry->item_cnt;i ++)
    {
        name = (char*)db_get_addr(entry,entry->mbrname_offset + i * MBR_NAME_LEN);
        if(wind_strcmp(mbrname,name) == 0)
            return i;
    }
    return -1;
}

w_err_t tb_entry_create(char *tbname,tbmodel_item_s *item_info,w_uint16_t item_cnt)
{
    w_int32_t i;
    w_err_t err;
    w_uint16_t offset;
    char *name_base;
    w_uint8_t *type,*count;
    w_uint16_t *pos,*psize,*pattr;
    w_int32_t size;
    w_tb_s *entry;
    
    size = get_tb_entry_size(item_cnt);
    entry = db_malloc(size);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_MEM);
    wind_memset(entry,0,size);
    entry->magic = TB_MAGIC;
    err = tb_name_split(tbname,entry->dbname,entry->tbname);
    WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,db_free(entry),W_ERR_INVALID);
    entry->base = (w_uint32_t)entry;
    entry->db = db_get_byname(entry->dbname);
    WIND_ASSERT_TODO_RETURN(entry->db != W_NULL,db_free(entry),W_ERR_INVALID);
    DNODE_INIT(entry->tbnode);


    entry->entry_size = size;
    entry->item_cnt = item_cnt;
    entry->hash = get_tb_hash(entry->tbname);

    DLIST_INIT(entry->data_list);
    entry->data_cnt = 0;
    entry->data_size = item_info[item_cnt-1].offset + item_info[item_cnt-1].size;
    
    offset = sizeof(w_tb_s);
    offset = (((offset + 7) >> 3) << 3);
    offset += set_offset(offset,entry->item_cnt,&entry->mbrname_offset,MBR_NAME_LEN);
    offset += set_offset(offset,entry->item_cnt,&entry->type_offset,sizeof(item_info->type));
    offset += set_offset(offset,entry->item_cnt,&entry->count_offset,sizeof(item_info->count));
    offset += set_offset(offset,entry->item_cnt,&entry->offset_offset,sizeof(item_info->offset));
    offset += set_offset(offset,entry->item_cnt,&entry->size_offset,sizeof(item_info->size));
    offset += set_offset(offset,entry->item_cnt,&entry->attr_offset,sizeof(item_info->attr));
    
    name_base = (char*)db_get_addr(entry,entry->mbrname_offset);
    type = (w_uint8_t*)db_get_addr(entry,entry->type_offset);
    count = (w_uint8_t*)db_get_addr(entry,entry->count_offset);
    pos = (w_uint16_t*)db_get_addr(entry,entry->offset_offset);
    psize = (w_uint16_t*)db_get_addr(entry,entry->size_offset);
    pattr = (w_uint16_t*)db_get_addr(entry,entry->attr_offset);
    for(i = 0;i < entry->item_cnt;i ++)
    {
        wind_strncpy(name_base,item_info[i].name,MBR_NAME_LEN);
        name_base += MBR_NAME_LEN;
        type[i] = item_info[i].type;
        count[i] = item_info[i].count;
        pos[i] = item_info[i].offset;
        psize[i] = item_info[i].size;
        pattr[i] = item_info[i].attr;
    }
    err = db_entry_insert_tb(entry->db,entry);
    WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,db_free(entry),W_ERR_FAIL);
    return W_TRUE;
}

w_bool_t tb_entry_exist(char *tbname)
{
    w_tb_s *entry;
    entry = tb_entry_get_byname(tbname);
    if(entry != W_NULL)
        return W_TRUE;
    return W_FALSE;
}

w_err_t tb_entry_destroy(char *tbname)
{
    
    w_dnode_s *dnode;
    w_err_t err;
    w_tb_s *entry;
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(entry->db != W_NULL,W_ERR_PTR_NULL);
    err = db_entry_remove_tb(entry->db,entry);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    foreach_node(dnode,&entry->data_list)
    {
        dnode = dlist_remove(&entry->data_list,dnode);
        db_free((void *)dnode);
    }
    db_free((void *)entry);
    return W_ERR_OK;
}

w_err_t tb_entry_insert(char *tbname,void *data,w_int32_t datasize)
{
    w_int32_t size;
    w_tb_s *entry;
    w_dnode_s *dnode;
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(entry->data_cnt < 65535,W_ERR_FAIL);
    size = sizeof(w_dnode_s) + entry->data_size;
    dnode = db_malloc(size);
    WIND_ASSERT_RETURN(dnode != 0,W_ERR_MEM);
    wind_memcpy((void*)db_get_addr(dnode,sizeof(w_dnode_s)),data,entry->data_size);
    dlist_insert_tail(&entry->data_list,dnode);
    entry->data_cnt ++;
    return W_ERR_OK;
}

w_err_t tb_entry_delete(char *tbname,w_int32_t row_idx)
{
    //w_int32_t size;
    w_tb_s *entry;
    w_dnode_s *dnode;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < entry->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(entry->data_cnt > 0,W_ERR_FAIL);
    foreach_node(dnode,&entry->data_list)
    {
        if(idx == row_idx)
        {
            dnode = dlist_remove(&entry->data_list,dnode);
            db_free((void *)dnode);
            entry->data_cnt --;
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_get_data(char *tbname,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    //w_int32_t size;
    w_tb_s *entry;
    w_dnode_s *dnode;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < entry->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data_size >= entry->data_size,W_ERR_INVALID);
    foreach_node(dnode,&entry->data_list)
    {
        if(idx == row_idx)
        {
            dnode = dlist_remove(&entry->data_list,dnode);
            wind_memcpy(data,(void*)db_get_addr(dnode,sizeof(w_dnode_s)),entry->data_size);
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_modify(char *tbname,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    //w_int32_t size;
    w_tb_s *entry;
    w_dnode_s *dnode;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < entry->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data_size >= entry->data_size,W_ERR_INVALID);
    foreach_node(dnode,&entry->data_list)
    {
        if(idx == row_idx)
        {
            //dnode = dlist_remove(&entry->data_list,dnode);
            wind_memcpy((void*)db_get_addr(dnode,sizeof(w_dnode_s)),data,entry->data_size);
            return W_ERR_OK;
        }
        idx ++;
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_modify_value(char *tbname,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    //w_int32_t i;
    w_tb_s *entry;
    w_dnode_s *dnode;
    w_int32_t idx = 0,item_idx;
    w_addr_t addr;
    //char *name;
    w_uint16_t *offset;
    WIND_ASSERT_RETURN(row_idx >= 0,W_ERR_INVALID);
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    item_idx = get_mbr_index(entry,mbrname);
    WIND_ASSERT_RETURN(item_idx >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(row_idx < entry->data_cnt,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data_size > 0,W_ERR_INVALID);
    foreach_node(dnode,&entry->data_list)
    {
        if(idx == row_idx)
        {
            offset = (w_uint16_t*)db_get_addr(entry,entry->offset_offset);
            addr = db_get_addr(dnode,sizeof(w_dnode_s)+offset[item_idx]);
            wind_memcpy((void*)addr,data,data_size);
            return W_ERR_OK;
        }
        idx ++;
    }
    return W_ERR_FAIL;
}

w_err_t tb_entry_query_count(char *tbname,w_int32_t *count)
{
    //w_int32_t size;
    w_tb_s *entry;
    //w_dnode_s *dnode;
    w_int32_t idx = 0;
    entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    *count = entry->data_cnt;
    return W_ERR_OK;
}



w_err_t tb_entry_query_cond_count(char *tbname,char *cond,w_int32_t *idxlist,w_int32_t cnt)
{
    wind_printf("tb_entry_query_cond_count is NOT supported.\r\n");
    return W_ERR_FAIL;
}

w_err_t tb_entry_print_info(char *tbname)
{
    w_tb_s *entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    wind_printf("\r\ntable info:\r\n");
    wind_printf("DB    name :%s\r\n",entry->dbname);
    wind_printf("table name :%s\r\n",entry->tbname);
    wind_printf("entry size :%d\r\n",entry->entry_size);
    wind_printf("entry hash :%d\r\n",entry->hash);
    wind_printf("item count :%d\r\n",entry->item_cnt);
    wind_printf("data count :%d\r\n",entry->data_cnt);
    wind_printf("data  size :%d\r\n",entry->data_size);

    wind_printf("name   offset:%d\r\n",entry->mbrname_offset);
    wind_printf("type   offset:%d\r\n",entry->type_offset);
    wind_printf("count  offset:%d\r\n",entry->count_offset);
    wind_printf("offset offset:%d\r\n",entry->offset_offset);
    wind_printf("size   offset:%d\r\n",entry->size_offset);
    wind_printf("attr   offset:%d\r\n",entry->attr_offset);
    return W_ERR_OK;
}

w_err_t tb_entry_print_data(w_tb_s *entry)
{
    w_dnode_s *dnode;
    w_uint8_t *data;
    w_int32_t idx = 0;
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    wind_printf("|   |---<table name=%s>\r\n",entry->tbname);
    foreach_node(dnode,&entry->data_list)
    {
        data = (w_uint8_t *)&dnode[1];
        tb_print_data(entry,data,idx);
        idx ++;
    }
    wind_printf("|   |---</table name=%s>\r\n",entry->tbname);
    return W_ERR_OK;
}

w_err_t tb_entry_print_table(char *tbname)
{
    w_tb_s *entry = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(entry != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(entry->magic == TB_MAGIC,W_ERR_INVALID);
    wind_printf("|---<DB name=%s>\r\n",entry->dbname);
    tb_entry_print_data(entry);
    wind_printf("|---</DB name=%s>\r\n",entry->dbname);
    return W_ERR_OK;
}

