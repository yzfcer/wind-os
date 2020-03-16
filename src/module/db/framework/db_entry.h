/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: db_entry.h
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
#ifndef DB_ENTRY_H_
#define DB_ENTRY_H_
#include "wind_type.h"
#include "db_def.h"
#include "wind_dlist.h"
#include "tb_entry.h"
#define DB_MAGIC 0xa5253867a16725b6

#define DB_NEXT(dbinfo) (w_db_s*)(dbinfo->base+dbinfo->next_offset)

//数据库函数
w_err_t db_entry_init(void);
w_db_s *db_get_byname(char *dbname);
w_bool_t db_entry_exist(char *dbname);
w_err_t db_entry_create(char *dbname,w_uint16_t attr);
w_err_t db_entry_destroy(w_db_s *db);
w_err_t db_entry_setattr(w_db_s *db,w_uint16_t attr);
w_err_t db_entry_getattr(w_db_s *db,w_uint16_t *attr);
w_err_t db_entry_insert_tb(w_db_s *db,w_tb_s *tb);
w_err_t db_entry_remove_tb(w_db_s *db,w_tb_s *tb);
w_err_t db_entry_print_info(w_db_s *db);
w_err_t db_entry_print_data(w_db_s *db);
w_err_t db_entry_print_all(void);


#endif


