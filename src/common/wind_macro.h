/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_macro.h
** Author      : Jason Zhou
** Last Date   : 2017.12.10
** Description : Common basic macro definitions
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2017.12.10
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2017.12.10
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef INTER_PROCRESS_H__
#define INTER_PROCRESS_H__

#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus


#define MEM_B(addr) (*((w_uint8_t *)(addr)))//Get a byte from a given address
#define MEM_W(addr) (*((w_uint16_t *)(addr)))//Get a word from a given address
#define MEM_DW(addr) (*(w_uint32_t *)(addr))//Get a doubleword from a given address

#define MAX(x,y) (((x) > (y))? (x): (y))
#define MAX3(x,y,z) MAX(MAX(x,y),z)
#define MAX4(x,y,z,w) MAX(MAX(x,y),MAX(z,w))
#define MIN(x,y) (((x) < (y))? (x): (y))
#define MIN3(x,y,z) MINMIN(x,y),z)
#define MIN4(x,y,z,w) MIN(MIN(x,y),MIN(z,w))
            

//Get the offset of a field in the structure
#define MBR_OFFSET(type,mbr) ((w_addr_t)&(((type*)0)->mbr))

//Get the length of a field in a structure
#define MBR_SIZE(type,mbr) sizeof(((type *)0)->mbr)

//Find the structure address according to the field address
#define OBJ_FROM_MBR(ptr,type,mbr) (void*)(((char*)(ptr))-((w_addr_t)&(((type*)0)->mbr)))

//Convert two bytes to one U16 bit in LSB format
#define FLIPW(arr) ( (((w_uint16_t) (arr)[0]) *256) + (arr)[1] )

//Convert a U16 bit to two bytes in LSB format
#define FLOPW( arr,val ) do{(arr)[0] = ((val) / 256);(arr)[1] = ((val) & 0xFF);}while(0)


//Get the address of a variable (uint16 width)
#define PTR_BYTE(var) ((w_uint8_t *)(void *)&(var))
#define PTR_WORD(var) ((w_uint16_t *)(void *)&(var))

//Get the high and low bytes of a word
#define WORD_LO(x) ((w_uint8_t) ((w_uint16_t)(x) & 0xff))
#define WORD_HI(x) ((w_uint8_t) ((w_uint16_t)(x) >> 8))

//Get each byte of a long integer
#define DWORD_HBYTE1(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 24) & 0xff))
#define DWORD_HBYTE2(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 16) & 0xff))
#define DWORD_HBYTE3(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 8) & 0xff))
#define DWORD_HBYTE4(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 0) & 0xff))


//Align the integer number to the right to the nth power of 2
#define RIGHT_ALIGN(x,align) ((((x) + (align-1)) / (align) ) *(align))
//Align the integer number to the left to the nth power of 2
#define LEFT_ALIGN(x,align) ((x)/(align)*(align))

//Convert a letter to uppercase
#define UPCASE(c) (((c) >= 'a' && (c) <= 'z')? ((c) - 0x20): (c))

//Convert a letter to lowercase
#define LOWERCASE(c) (((c) >= 'A' && (c) <= 'Z')? ((c) + 0x20): (c))

//Determines whether a character is a 10 digit value or not
#define IS_DECIMAL(c) ((c) >= '0' && (c) <= '9')

//Determines whether a character is a 16 in value
#define IS_HEX( c ) (((c) >= '0' && (c) <= '9') ||\
((c) >= 'A' && (c) <= 'F') ||\
((c) >= 'a' && (c) <= 'f'))

//Determine whether a character is a letter
#define IS_ALPHABET(c) (((c) >= 'A' && (c) <= 'Z')|| ((c) >= 'a' && (c) <= 'z'))




//Increment prevents overflow
#define INC_SAT(val) (val=((val)+1>(val))? (val)+1: (val))


//Get the number of array elements
#define ARR_SIZE(arr) (sizeof((arr))/sizeof((arr[0])))

//Get the modulus of an unsigned number
#define MOD_BY_POWER_OF_TWO( val,mod_by ) ((w_uint32_t)(val) & (w_uint32_t)((mod_by)-1))


#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef INTER_PROCRESS_H__
