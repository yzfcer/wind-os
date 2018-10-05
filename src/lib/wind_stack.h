/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_stack.h
**创   建   人: Jason Zhou
**最后修改日期: 2013.10.05
**描        述: wind os的FILO栈结构相关功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2013.10.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2013.10.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef    WIND_STACK_H_
#define WIND_STACK_H_

#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_STACK_SUPPORT

#ifndef NOT_OK
#define NOT_OK              -3                        /* 参数错误                                     */
#endif

#define STACK_OK            0                           /* 操作成功                                     */
#define STACK_ERR           -1
#define STACK_FULL          -2                           /* 队列满                                       */
#define STACK_EMPTY         -3                           /* 无数据                                       */

//#define Q_WRITE_MODE        1                           /* 操作成功                                     */
//#define Q_WRITE_FRONT_MODE  2                           /* 操作成功                                     */

#ifndef STACK_DATA_TYPE
#define STACK_DATA_TYPE     w_uint8_t
#endif

typedef struct __w_stack_s w_stack_s;
struct __w_stack_s
{
    STACK_DATA_TYPE     *out;                   /* 指向数据输出位置         */
    STACK_DATA_TYPE     *in;                    /* 指向数据输入位置         */      
    STACK_DATA_TYPE     *top;                    /* 指向Buf的结束位置        */
    w_uint16_t              item_cnt;                  /* 队列中数据个数           */
    w_uint16_t              item_max;                /* 队列中允许存储的数据个数 */
    w_uint8_t                item_size;                /* 元素的数据宽度 */  
    w_uint32_t              emptycnt;
    w_uint32_t              fullcnt;
    w_err_t               (* read_empty)(w_stack_s *pstk,void *data);     /* 读空处理函数             */
    w_err_t               (* write_full)(w_stack_s *pstk,void *data);        /* 写满处理函数             */
    STACK_DATA_TYPE     buff[1];                 /* 存储数据的空间           */
};


w_err_t wind_stack_create(void *mem,
                          w_uint32_t size,
                          w_uint16_t item_size,
                          w_err_t (* ReadEmpty)(),
                          w_err_t (* WriteFull)());
                          


w_err_t wind_stack_read(void *mem,void *Ret);

w_err_t wind_stack_write(void *mem, void *Data);

w_uint16_t wind_stack_datalen(void *mem);

w_uint16_t wind_stack_size(void *mem);

void wind_stack_flush(void *mem);

#endif

#ifdef __cplusplus
}
#endif

#endif  //WIND_STACK_H_

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

