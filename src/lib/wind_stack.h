/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_stack.h
**��   ��   ��: Jason Zhou
**����޸�����: 2013.10.05
**��        ��: wind os��FILOջ�ṹ��ع���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.10.05
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2013.10.05
** �衡��: 
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
#define NOT_OK              -3                        /* ��������                                     */
#endif

#define STACK_OK            0                           /* �����ɹ�                                     */
#define STACK_ERR           -1
#define STACK_FULL          -2                           /* ������                                       */
#define STACK_EMPTY         -3                           /* ������                                       */

//#define Q_WRITE_MODE        1                           /* �����ɹ�                                     */
//#define Q_WRITE_FRONT_MODE  2                           /* �����ɹ�                                     */

#ifndef STACK_DATA_TYPE
#define STACK_DATA_TYPE     w_uint8_t
#endif

typedef struct __w_stack_s w_stack_s;
struct __w_stack_s
{
    STACK_DATA_TYPE     *out;                   /* ָ���������λ��         */
    STACK_DATA_TYPE     *in;                    /* ָ����������λ��         */      
    STACK_DATA_TYPE     *top;                    /* ָ��Buf�Ľ���λ��        */
    w_uint16_t              item_cnt;                  /* ���������ݸ���           */
    w_uint16_t              item_max;                /* ����������洢�����ݸ��� */
    w_uint8_t                item_size;                /* Ԫ�ص����ݿ�� */  
    w_uint32_t              emptycnt;
    w_uint32_t              fullcnt;
    w_err_t               (* read_empty)(w_stack_s *pstk,void *data);     /* ���մ�����             */
    w_err_t               (* write_full)(w_stack_s *pstk,void *data);        /* д��������             */
    STACK_DATA_TYPE     buff[1];                 /* �洢���ݵĿռ�           */
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

