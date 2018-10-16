/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_debug.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ������Ϣ�������ʽͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_DEBUG_H__
#define WIND_DEBUG_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
//ϵͳ������붨��
#define W_ERR_OK                 0 //�������ؽ��
#define W_ERR_FAIL              -1 //һ�����
#define W_ERR_PTR_NULL          -2 //ָ��Ϊ��
#define W_ERR_OVERFLOW          -3 //����Ĳ��������˷�Χ
#define W_ERR_MEM               -4 //�洢�ռ��ܵ�����
#define W_ERR_INVALID           -6//������Ч
#define W_ERR_TIMEOUT           -7//������ʱ������ʧ��
#define W_ERR_STATUS            -8//��ǰ״̬����
#define W_ERR_REPEAT            -9//�ض������¶����ظ�
#define W_ERR_NOFILE           -10//�ļ�������
#define W_ERR_FILE_OPENED      -11//�ļ��Ѿ���
#define W_ERR_NOT_SUPPORT      -12//�ļ�ϵͳ��֧��


//---------------------------------------------------------------------
//��ӡ�����Ķ���
#define WIND_DEBUG_SUPPORT 1
#if WIND_DEBUG_SUPPORT
typedef char *  wind_va_list;
extern w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
w_int32_t wind_vsprintf(char *buf, const char *fmt, wind_va_list args);
w_int32_t wind_printf(const char *fmt, ...);
w_int32_t wind_sprintf(char *buff, const char *fmt, ...);
void wind_print_space(w_int32_t space8_cnt);
#else
#define wind_printf(fmt, ...) 0
#define wind_vsprintf(buf, fmt, args) 0
#define wind_printf(fmt, ...) 0
#define wind_sprintf(buff,fmt, ...) 0
#define wind_print_space(a)
#endif

//---------------------------------------------------------------------
//ϵͳ������Ϣ��ӡ����
#define PRINT_LV_DEBUG   1
#define PRINT_LV_NOTICE  2
#define PRINT_LV_WARN    3
#define PRINT_LV_ERROR   4
#define PRINT_LV_CRIT    5

#define PRINT_LEVEL PRINT_LV_NOTICE

//---------------------------------------------------------------------
#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_DEBUG))
#define wind_debug(fmt,...) do{wind_printf("%s"fmt"\r\n","[debug] ",##__VA_ARGS__);}while(0)
#else 
#define wind_debug(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_NOTICE))
#define wind_notice(fmt,...) do{wind_printf("%s"fmt"\r\n","[notice] ",##__VA_ARGS__);}while(0)
#else 
#define wind_notice(fmt,...) 
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_WARN))
#define wind_warn(fmt,...) do{wind_printf("%s[%s,%d] "fmt"\r\n","[warn] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_warn(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_ERROR))
#define wind_error(fmt,...) do{wind_printf("%s[%s,%d] "fmt"\r\n","[error] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_error(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_CRIT))
#define wind_critical(fmt,...) do{wind_printf("%s[%s,%d] "fmt"\r\n","[critical] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_critical(fmt,...)
#endif



//---------------------------------------------------------------------
//�ж���������
#define WIND_ASSERT_RETURN(cond,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return res;}}while(0)
#define WIND_ASSERT_TODO(cond,todo,res) do{if(!(cond)) {todo;return res;}}while(0)
    
        

#ifdef __cplusplus
}
#endif

#endif  //WIND_DEBUG_H__

