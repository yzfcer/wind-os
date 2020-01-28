/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_debug.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 调试信息的输出方式头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_DEBUG_H__
#define WIND_DEBUG_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef USE_SYS_VSPRINTF
#include <stdarg.h>
#include <stdio.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
//系统错误代码定义
#define W_ERR_OK                 0 //正常返回结果
#define W_ERR_FAIL              -1 //一般错误
#define W_ERR_PTR_NULL          -2 //指针为空
#define W_ERR_NO_OBJ            -3 //对象不存在
#define W_ERR_OVERFLOW          -4 //传入的参数超出了范围
#define W_ERR_MEM               -5 //存储空间受到限制
#define W_ERR_INVALID           -6 //参数无效
#define W_ERR_TIMEOUT           -7 //操作超时，返回失败
#define W_ERR_STATUS            -8 //当前状态错误
#define W_ERR_REPEAT            -9 //特定条件下对象重复
#define W_ERR_NOFILE           -10 //文件不存在
#define W_ERR_FILE_OPENED      -11 //文件已经打开
#define W_ERR_NOT_SUPPORT      -12 //文件系统不支持
#define W_ERR_CRC              -13 //CRC检验错误
#define W_ERR_HARDFAULT        -14 //硬件故障错误
#define W_ERR_VERSION          -15 //版本信息错误


//---------------------------------------------------------------------
//打印函数的定义
#ifndef WIND_DEBUG_SUPPORT
#define WIND_DEBUG_SUPPORT 1
#endif

#if WIND_DEBUG_SUPPORT
#ifdef  __cplusplus
#define _WIND_ADDRESSOF(v)   (&reinterpret_cast<const char &>(v) )
#else
#define _WIND_ADDRESSOF(v)   (&(v))
#endif

#define _WIND_INTSIZEOF(n)   ((sizeof(n) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

#define _wind_crt_va_start(ap,v)  ( ap = (wind_va_list)_WIND_ADDRESSOF(v) + _WIND_INTSIZEOF(v) )
#define _wind_crt_va_arg(ap,t)    ( *(t *)((ap += _WIND_INTSIZEOF(t)) - _WIND_INTSIZEOF(t)) )
#define _wind_crt_va_end(ap)      ( ap = (wind_va_list)0 )

#define wind_va_start _wind_crt_va_start /* windows stdarg.h */
#define wind_va_arg _wind_crt_va_arg
#define wind_va_end _wind_crt_va_end
#define do_div(n,base) _div(&n,base)




typedef char *  wind_va_list;
extern w_int32_t wind_std_output(w_uint8_t *buff,w_int32_t len);
extern w_int32_t wind_std_input(w_uint8_t *buff,w_int32_t len);
w_int32_t wind_vsprintf(char *buf, const char *fmt, wind_va_list args);
w_int32_t wind_printf(const char *fmt, ...);
w_int32_t wind_sprintf(char *buff, const char *fmt, ...);

w_int32_t wind_vsscanf(const char *buf, const char *fmt, wind_va_list args);
w_int32_t wind_scanf(const char *fmt,...);
w_int32_t wind_sscanf(const char *buff, const char *fmt,...);

void wind_print_space(w_int32_t space8_cnt);
#else
#define wind_printf(fmt, ...) 0
#define wind_vsprintf(buf, fmt, args) 0
#define wind_printf(fmt, ...) 0
#define wind_sprintf(buff,fmt, ...) 0

#define wind_vsscanf(buf, fmt, args) 0
#define wind_scanf(fmt, ...) 0
#define wind_sscanf(buff,fmt, ...) 0

#define wind_print_space(a)
#endif

//---------------------------------------------------------------------
//系统调试信息打印级别
#define PRINT_LV_DEBUG   1
#define PRINT_LV_TRACE   2
#define PRINT_LV_NOTICE  3
#define PRINT_LV_WARN    4
#define PRINT_LV_ERROR   5
#define PRINT_LV_CRIT    6

#define PRINT_LEVEL PRINT_LV_NOTICE//PRINT_LV_DEBUG//

//---------------------------------------------------------------------
#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_DEBUG))
#define wind_debug(fmt,...) do{wind_printf("%s[%s,%d] "fmt"\r\n","[debug] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_debug(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_TRACE))
#define wind_trace(fmt,...) do{wind_printf("%s[%s,%d] "fmt"\r\n","[trace] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#define wind_trace_enter() do{wind_printf("%s %s enter(%d)\r\n","[trace] ",__FUNCTION__,__LINE__ );}while(0)
#define wind_trace_exit()  do{wind_printf("%s %s exit (%d)\r\n","[trace] ",__FUNCTION__,__LINE__ );}while(0)
#else 
#define wind_trace(fmt,...)
#define wind_trace_enter()
#define wind_trace_exit()
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
//判断条件断言
#define WIND_ASSERT_RETURN(cond,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return res;}}while(0)
#define WIND_ASSERT_TODO_RETURN(cond,todo,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);todo;return res;}}while(0)
#define WIND_ASSERT_TODO(cond,todo) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);todo;}}while(0)
#define WIND_ASSERT_RETURN_VOID(cond) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return;}}while(0)
#define WIND_ASSERT_BREAK(cond,res,msg,...) {if(!(cond)) {wind_error("ASSERT(%s)"#msg,#cond,##__VA_ARGS__);err = res;break;}}

#define WIND_CHECK_BREAK(cond,res) {if(!(cond)) {wind_debug("EXPECT(%s)",#cond);err = res;break;}}
#define WIND_CHECK_RETURN(cond,res) {if(!(cond)) {wind_debug("EXPECT(%s)",#cond);return res;}}
#define WIND_CHECK_RETURN_VOID(cond) {if(!(cond)) {wind_debug("EXPECT(%s)",#cond);return ;}}
#define WIND_CHECK_TODO_RETURN(cond,todo,res) {if(!(cond)) do{if(!(cond)) {wind_debug("EXPECT(%s)",#cond);todo;return res;}}while(0)

#define WIND_ASSERT_MSG_RETURN(cond,res,msg,...) do{if(!(cond)){wind_error(msg,##__VA_ARGS__);return res;}}while(0)

#ifdef __cplusplus
}
#endif

#endif  //WIND_DEBUG_H__

