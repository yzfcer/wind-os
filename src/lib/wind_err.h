#ifndef WIND_ERR_H__
#define WIND_ERR_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"

#define ERR_OK                 0 //正常返回结果
#define ERR_COMMAN            -1 //一般错误
#define ERR_NULL_POINTER      -2 //指针为空
#define ERR_PARAM_OVERFLOW    -3 //传入的参数超出了范围
#define ERR_MEM               -4 //存储空间受到限制
#define ERR_INVALID_PARAM     -6//参数无效
#define ERR_TIMEOUT           -7//操作超时，返回失败


extern w_int8_t *wind_err_set[];
#define WIND_FUNC_NAME(name) w_int8_t funcname = x
#define WIND_ERR(i) WIND_ERROR("%s:%s",funcname,wind_err_set[i])

#endif

