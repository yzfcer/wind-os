/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: 
**创   建   人: 
**最后修改日期: 
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef INTER_PROCRESS_H__
#define INTER_PROCRESS_H__

#include "wind_types.h"
#ifdef c_plusplus__
extern {
#endif
#define MEM_B(addr) (*((u8_t *)(addr)))//从给定的地址上获取一个字节
#define MEM_W(addr) (*((u16_t *)(addr)))//从给定的地址上获取一个字
#define MEM_DW(addr) (*(u32_t *)(addr))//从给定的地址上获取一个双字

#define MAX(x, y) (((x) > (y))?(x):(y))
#define MAX3(x, y, z) MAX(MAX(x,y),z)
#define MAX4(x, y, z, w) MAX(MAX(x,y),MAX(z,w))
#define MIN(x, y) (((x) < (y))?(x):(y))
#define MIN3(x, y, z) MINMIN(x,y),z)
#define MIN4(x, y, z, w) MIN(MIN(x,y),MIN(z,w))

//将大端格式转换成小端格式或者将小端格式转换成大端格式
#define BE2LE2(bit16) ((bit16 >> 8) + (bit16 << 8))
#define BE2LE4(bit32) (((bit32 >> 24) & 0xff) \
            + ((bit32 >> 16) & 0xff00)\
            + ((bit32 << 16) & 0xff0000)\
            + ((bit32 << 24) & 0xff000000))
            

//获得结构体中一个字段的偏移量
#define FPOS( type, field ) ((u32_t) &((type *)0)->field)
//得到一个结构体中的某个字段的长度
#define FSIZ(type,field) sizeof(((type *)0)->field)

//按照LSB格式把两个字节转化为一个u16_t
#define FLIPW( ray ) ( (((u16_t) (ray)[0]) * 256) + (ray)[1] )

//按照LSB格式把一个u16_t转化为两个字节
#define FLOPW( ray, val ) \
(ray)[0] = ((val) / 256); \
(ray)[1] = ((val) & 0xFF)

//得到一个变量的地址（u16_t宽度）
#define B_PTR( var ) ( (u8_t *) (void *) &(var) )
#define W_PTR( var ) ( (u16_t *) (void *) &(var) )

//得到一个字的高位和低位字节
#define WORD_LO(x) ((u8_t) ((u16_t)(x) & 0xff))
#define WORD_HI(x) ((u8_t) ((u16_t)(x) >> 8))

//返回一个比X大的最接近的8的倍数
#define RND8( x ) ((((x) + 7) / 8 ) * 8 )

//将一个字母转换为大写
#define UPCASE(c) (((c) >= 'a' && (c) <= 'z')?((c) - 0x20):(c))

////将一个字母转换为大写
#define DOWNCASE(c) (((c) >= 'A' && (c) <= 'Z')?((c) + 0x20):(c))

//判断字符是不是10进值的数字
#define DECCHK( c ) ((c) >= '0' && (c) <= '9')

//判断字符是不是16进值的数字
#define HEXCHK( c ) (((c) >= '0' && (c) <= '9') ||\
((c) >= 'A' && (c) <= 'F') ||\
((c) >= 'a' && (c) <= 'f'))

//防止溢出的一个方法
#define INC_SAT(val) (val=((val)+1>(val))?(val)+1:(val))


//返回数组元素的个数
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))

//返回一个无符号数n尾的值MOD_BY_POWER_OF_TWO(X,n)=X%(2^n)
#define MOD_BY_POWER_OF_TWO( val, mod_by ) \
((u32_t)(val) & (u32_t)((mod_by)-1))


#ifdef _DEBUG
#define DEBUGMSG(msg,date) printf(msg);printf("?d%d%d"?date,_LINE_,_FILE_)
#else
#define DEBUGMSG(msg,date)
#endif


















#ifdef c_plusplus__
}
#endif


#endif
