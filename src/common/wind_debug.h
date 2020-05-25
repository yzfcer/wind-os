/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_debug.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : Debug information printing module
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
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
#endif //#ifdef __cplusplus

//---------------------------------------------------------------------
//system error code
#define W_ERR_OK                 0 //Successful implementation
#define W_ERR_FAIL              -1 //Common error
#define W_ERR_PTR_NULL          -2 //Error caused by null pointer 
#define W_ERR_NO_OBJ            -3 //Error raised by object NOT exsisting
#define W_ERR_OVERFLOW          -4 //Error caused by parameter out of bounds
#define W_ERR_MEM               -5 //Error caused by memory allocation failure
#define W_ERR_INVALID           -6 //Error caused by invalid parameter
#define W_ERR_TIMEOUT           -7 //Error caused by operation timeout
#define W_ERR_STATUS            -8 //Error caused by error status
#define W_ERR_REPEAT            -9 //Error caused by duplicate object
#define W_ERR_NOFILE           -10 //Error caused by file not existing
#define W_ERR_FILE_OPENED      -11 //Error caused by file already open
#define W_ERR_NOT_SUPPORT      -12 //Error caused by feature not supported
#define W_ERR_CRC              -13 //Error caused by CRC checking error
#define W_ERR_HARDFAULT        -14 //Error caused by hardware fault
#define W_ERR_VERSION          -15 //Error caused by version information


//---------------------------------------------------------------------
//print functions
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
w_int32_t wind_vsprintf(char *buf,const char *fmt,wind_va_list args);
w_int32_t wind_printf(const char *fmt,...);
w_int32_t wind_sprintf(char *buff,const char *fmt,...);

w_int32_t wind_vsscanf(const char *buf,const char *fmt,wind_va_list args);
w_int32_t wind_scanf(const char *fmt,...);
w_int32_t wind_sscanf(const char *buff,const char *fmt,...);

void wind_print_space(w_int32_t space8_cnt);
#else
#define wind_printf(fmt,...) 0
#define wind_vsprintf(buf,fmt,args) 0
#define wind_printf(fmt,...) 0
#define wind_sprintf(buff,fmt,...) 0

#define wind_vsscanf(buf,fmt,args) 0
#define wind_scanf(fmt,...) 0
#define wind_sscanf(buff,fmt,...) 0

#define wind_print_space(a)
#endif

//---------------------------------------------------------------------
//System debugging printing level
#define PRINT_LV_DEBUG   1
#define PRINT_LV_TRACE   2
#define PRINT_LV_NOTICE  3
#define PRINT_LV_WARN    4
#define PRINT_LV_ERROR   5
#define PRINT_LV_CRIT    6

#define PRINT_LEVEL PRINT_LV_NOTICE//PRINT_LV_DEBUG//

//---------------------------------------------------------------------
//Various print level functions
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
//Judgment condition assertion
#define WIND_ASSERT_RETURN(cond,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return res;}}while(0)
#define WIND_ASSERT_TODO_RETURN(cond,todo,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);todo;return res;}}while(0)
#define WIND_ASSERT_TODO(cond,todo) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);todo;}}while(0)
#define WIND_ASSERT_RETURN_VOID(cond) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return;}}while(0)
#define WIND_ASSERT_BREAK(cond,res,msg,...) {if(!(cond)) {wind_error("ASSERT(%s):"#msg,#cond,##__VA_ARGS__);err = res;break;}}
#define WIND_TRAP() {wind_error("proc trapped here now!");while(1);}
	
#define WIND_CHECK_BREAK(cond,res) {if(!(cond)) {wind_debug("EXPECT(%s)",#cond);err = res;break;}}
#define WIND_CHECK_RETURN(cond,res) {if(!(cond)) {wind_debug("EXPECT(%s)",#cond);return res;}}
#define WIND_CHECK_RETURN_VOID(cond) {if(!(cond)) {wind_debug("EXPECT(%s)",#cond);return ;}}
#define WIND_CHECK_TODO_RETURN(cond,todo,res) {if(!(cond)) do{if(!(cond)) {wind_debug("EXPECT(%s)",#cond);todo;return res;}}while(0)
#define WIND_CHECK_TODO_BREAK(cond,todo,res) {if(!(cond)) do{if(!(cond)) {wind_debug("EXPECT(%s)",#cond);todo;break;}}while(0)

#define WIND_ASSERT_MSG_RETURN(cond,res,msg,...) do{if(!(cond)){wind_error(msg,##__VA_ARGS__);return res;}}while(0)

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif  //WIND_DEBUG_H__

