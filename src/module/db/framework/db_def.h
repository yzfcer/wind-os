/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : db.c
** Author      : Jason Zhou
** Last Date   : 2017.12.19
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2017.12.19
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2017.12.19
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef DB_H__
#define DB_H__

#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_mutex.h"
#define DB_BLK_SIZE 512
#define DB_NAME_LEN 20   //数据库名字长度
#define TB_NAME_LEN 20   //数据表名字长度
#define MBR_NAME_LEN 20  //数据名字长度

typedef enum 
{
    TYPE_w_int8_t = 0x01,
    TYPE_w_int16_t ,
    TYPE_w_int32_t ,
    TYPE_w_int64_t ,

    TYPE_w_uint8_t ,
    TYPE_w_uint16_t ,
    TYPE_w_uint32_t ,
    TYPE_w_uint64_t ,

    TYPE_w_bool_t ,
    TYPE_w_str16_t ,
    TYPE_w_str32_t ,
    TYPE_w_str64_t ,
    TYPE_w_str128_t ,
    TYPE_w_str256_t ,
}PARA_TYPE;


typedef struct __db_entry_s w_db_s;
typedef struct __tb_entry_s w_tb_s;


struct __db_entry_s
{
    w_uint64_t magic;
    w_dnode_s dbnode;
    w_dlist_s tblist;
    char name[DB_NAME_LEN];
    w_addr_t base;//绝对地址
    w_uint16_t attr;
    w_int32_t hash;
    w_int32_t tb_count;
};

struct __tb_entry_s
{
    w_uint64_t magic;
    w_dnode_s tbnode;
    w_dlist_s data_list;
    char dbname[DB_NAME_LEN];
    char tbname[TB_NAME_LEN];
    w_addr_t base;//绝对地址
    w_db_s *db;
    
    w_mutex_s mutex;
    w_uint32_t entry_size;
    w_uint16_t item_cnt;
    w_int32_t hash;
    
    w_uint16_t data_cnt;
    w_uint16_t data_size;
    //偏移地址
    w_int16_t mbrname_offset;
    w_int16_t type_offset;
    w_int16_t count_offset;
    w_int16_t offset_offset;
    w_int16_t size_offset;
    w_int16_t attr_offset;
};

typedef struct
{
    w_uint16_t msgid;
    char dbname[DB_NAME_LEN];
    w_int32_t dbsize;
}db_msg_s;


#define db_get_addr(base,offset) (w_addr_t)((offset)+(w_addr_t)(base))
#define db_get_offset(base,addr) (w_uint32_t)((w_addr_t)(addr)+(w_addr_t)(base))
#endif
