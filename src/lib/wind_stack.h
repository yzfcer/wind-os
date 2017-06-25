/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_stack.h
**创   建   人: 周江村
**最后修改日期: 2013.10.05
**描        述: wind os的消息相关的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.10.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.10.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef    WIND_STACK_H_
#define WIND_STACK_H_

#include "wind_config.h"
#include "wind_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_STACK_SUPPORT > 0


#ifndef NOT_OK
#define NOT_OK              -3                        /* 参数错误                                     */
#endif

#define STACK_OK            0                           /* 操作成功                                     */
#define STACK_ERR            -1
#define STACK_FULL          -2                           /* 队列满                                       */
#define STACK_EMPTY         -3                           /* 无数据                                       */

//#define Q_WRITE_MODE        1                           /* 操作成功                                     */
//#define Q_WRITE_FRONT_MODE  2                           /* 操作成功                                     */

#ifndef STACK_DATA_TYPE
#define STACK_DATA_TYPE     u8_t
#endif

//__packed
//#pragma pack(1)
typedef struct __stack_s{
    STACK_DATA_TYPE     *Out;                   /* 指向数据输出位置         */
    STACK_DATA_TYPE     *In;                    /* 指向数据输入位置         */      
    STACK_DATA_TYPE     *Top;                    /* 指向Buf的结束位置        */
    u16_t              NData;                  /* 队列中数据个数           */
    u16_t              MaxData;                /* 队列中允许存储的数据个数 */
    u8_t                DataWid;                /* 元素的数据宽度 */  
    u32_t              emptycnt;
    u32_t              fullcnt;
    err_t               (* ReadEmpty)(struct __stack_s *pstk,void *data);     /* 读空处理函数             */
    err_t               (* WriteFull)(struct __stack_s *pstk,void *data);        /* 写满处理函数             */
    STACK_DATA_TYPE     Buf[1];                 /* 存储数据的空间           */
} stack_s,*pstack_s;


//#ifndef IN_STACK
err_t wind_stack_create(void *Buf,
                          u32_t SizeOfBuf,
                          u16_t DataWid,
                          err_t (* ReadEmpty)(),
                          err_t (* WriteFull)()
                          );


err_t wind_stack_read(void *Buf,void *Ret);

err_t wind_stack_write(void *Buf, void *Data);

u16_t wind_stack_datalen(void *Buf);

u16_t wind_stack_size(void *Buf);

void wind_stack_flush(void *Buf);
//#endif //IN_STACK

#define EN_STACK_WRITE            1     /* 禁止(0)或允许(1)FIFO发送数据       */
#define EN_STACK_WRITE_FRONT      1     /* 禁止(0)或允许(1)LIFO发送数据       */
#define EN_STACK_NDATA            1     /* 禁止(0)或允许(1)取得队列数据数目   */
#define EN_STACK_SIZE             1     /* 禁止(0)或允许(1)取得队列数据总容量 */
#define EN_STACK_FLUSH            1     /* 禁止(0)或允许(1)清空队列           */


#endif

#ifdef __cplusplus
}
#endif

#endif  //WIND_STACK_H_

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

