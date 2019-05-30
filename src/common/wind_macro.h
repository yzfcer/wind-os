/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_macro.h
**��   ��   ��: Jason Zhou
**����޸�����: 2017.12.10
**��        ��: һЩ�����ĺ궨��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 2017.12.10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 2017.12.10
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef INTER_PROCRESS_H__
#define INTER_PROCRESS_H__

#include "wind_type.h"
#ifdef c_plusplus__
extern {
#endif

#define MEM_B(addr) (*((w_uint8_t *)(addr)))//�Ӹ����ĵ�ַ�ϻ�ȡһ���ֽ�
#define MEM_W(addr) (*((w_uint16_t *)(addr)))//�Ӹ����ĵ�ַ�ϻ�ȡһ����
#define MEM_DW(addr) (*(w_uint32_t *)(addr))//�Ӹ����ĵ�ַ�ϻ�ȡһ��˫��

#define MAX(x, y) (((x) > (y))?(x):(y))
#define MAX3(x, y, z) MAX(MAX(x,y),z)
#define MAX4(x, y, z, w) MAX(MAX(x,y),MAX(z,w))
#define MIN(x, y) (((x) < (y))?(x):(y))
#define MIN3(x, y, z) MINMIN(x,y),z)
#define MIN4(x, y, z, w) MIN(MIN(x,y),MIN(z,w))
            

//��ýṹ����һ���ֶε�ƫ����
#define MBR_OFFSET(type, mbr) ((w_uint32_t)&(((type*)0)->mbr))

//�õ�һ���ṹ���е�ĳ���ֶεĳ���
#define MBR_SIZE(type,mbr) sizeof(((type *)0)->mbr)

//�����ֶε�ַ�ҵ��ṹ��ַ
#define OBJ_FROM_MBR(ptr,type,mbr) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbr)))

//����LSB��ʽ�������ֽ�ת��Ϊһ��u16_t
#define FLIPW(arr) ( (((w_uint16_t) (arr)[0]) *256) + (arr)[1] )

//����LSB��ʽ��һ��u16_tת��Ϊ�����ֽ�
#define FLOPW( arr, val ) do{(arr)[0] = ((val) / 256);(arr)[1] = ((val) & 0xFF);}while(0)


//�õ�һ�������ĵ�ַ��w_uint16_t��ȣ�
#define PTR_BYTE(var) ((w_uint8_t *)(void *)&(var))
#define PTR_WORD(var) ((w_uint16_t *)(void *)&(var))

//�õ�һ���ֵĸ�λ�͵�λ�ֽ�
#define WORD_LO(x) ((w_uint8_t) ((w_uint16_t)(x) & 0xff))
#define WORD_HI(x) ((w_uint8_t) ((w_uint16_t)(x) >> 8))

//��ȡһ�������͵ĸ����ֽ�
#define DWORD_HBYTE1(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 24) & 0xff))
#define DWORD_HBYTE2(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 16) & 0xff))
#define DWORD_HBYTE3(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 8) & 0xff))
#define DWORD_HBYTE4(x) ((w_uint8_t)((((w_uint32_t)(x)) >> 0) & 0xff))


//�������������Ҷ��룬alignΪ2��N�η�
#define RIGHT_ALIGN(x,align) ((((x) + (align-1)) / (align) ) *(align))
//����������������룬alignΪ2��N�η�
#define LEFT_ALIGN(x,align) ((x)/(align)*(align))

//��һ����ĸת��Ϊ��д
#define UPCASE(c) (((c) >= 'a' && (c) <= 'z')?((c) - 0x20):(c))

////��һ����ĸת��Ϊ��д
#define LOWERCASE(c) (((c) >= 'A' && (c) <= 'Z')?((c) + 0x20):(c))

//�ж��ַ��ǲ���10��ֵ������
#define IS_DECIMAL(c) ((c) >= '0' && (c) <= '9')

//�ж��ַ��ǲ���16��ֵ������
#define IS_HEX( c ) (((c) >= '0' && (c) <= '9') ||\
((c) >= 'A' && (c) <= 'F') ||\
((c) >= 'a' && (c) <= 'f'))

//�ж��ַ��ǲ�����ĸ
#define IS_ALPHABET(c) (((c) >= 'A' && (c) <= 'Z')||((c) >= 'a' && (c) <= 'z'))




//��ֹ�����һ������
#define INC_SAT(val) (val=((val)+1>(val))?(val)+1:(val))


//��������Ԫ�صĸ���
#define ARR_SIZE(arr) (sizeof((arr))/sizeof((arr[0])))

//����һ���޷�����nβ��ֵMOD_BY_POWER_OF_TWO(X,n)=X%(2^n)
#define MOD_BY_POWER_OF_TWO( val, mod_by ) \
((w_uint32_t)(val) & (w_uint32_t)((mod_by)-1))

#ifdef c_plusplus__
}
#endif


#endif
