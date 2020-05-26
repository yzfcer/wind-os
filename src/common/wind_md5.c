/****************************************Copyright(c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_crc32.h
** Author      : Jason Zhou
** Last Date   : 2019.2.18
** Description : MD5算法模块
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2019.2.18
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2019.2.18
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_md5.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#define S11 7 
#define S12 12 
#define S13 17 
#define S14 22 
#define S21 5 
#define S22 9 
#define S23 14 
#define S24 20 
#define S31 4 
#define S32 11 
#define S33 16 
#define S34 23 
#define S41 6 
#define S42 10 
#define S43 15 
#define S44 21 
   
static void md5_transform(w_uint32_t state[4],w_uint8_t block[64]); 
static void encode(w_uint8_t *output,w_uint32_t *input,w_uint32_t len); 
static void decode(w_uint32_t *output,w_uint8_t *input,w_uint32_t len); 
   
static w_uint8_t PADDING[64] = { 
 0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 
}; 
   

#define F(x,y,z)(((x) &(y)) | ((~x) &(z))) 
#define G(x,y,z)(((x) &(z)) | ((y) &(~z))) 
#define H(x,y,z)((x) ^(y) ^(z)) 
#define I(x,y,z)((y) ^((x) | (~z))) 
   

#define ROTATE_LEFT(x,n)(((x) <<(n)) | ((x) >>(32-(n)))) 
   

#define FF(a,b,c,d,x,s,ac) { \
(a) += F((b),(c),(d)) +(x) +(w_uint32_t)(ac); \
(a) = ROTATE_LEFT((a),(s)); \
(a) +=(b); \
 }
#define GG(a,b,c,d,x,s,ac) { \
(a) += G((b),(c),(d)) +(x) +(w_uint32_t)(ac); \
(a) = ROTATE_LEFT((a),(s)); \
(a) +=(b); \
 }
#define HH(a,b,c,d,x,s,ac) { \
(a) += H((b),(c),(d)) +(x) +(w_uint32_t)(ac); \
(a) = ROTATE_LEFT((a),(s)); \
(a) +=(b); \
 }
#define II(a,b,c,d,x,s,ac) { \
(a) += I((b),(c),(d)) +(x) +(w_uint32_t)(ac); \
(a) = ROTATE_LEFT((a),(s)); \
(a) +=(b); \
 }

static void encode(w_uint8_t *output,w_uint32_t *input,w_uint32_t len) 
{ 
    w_uint32_t i,j; 
    for(i = 0,j = 0; j < len; i++,j += 4)
    { 
        output[j+0] =(w_uint8_t)((input[i] >> 0) & 0xff); 
        output[j+1] =(w_uint8_t)((input[i] >> 8) & 0xff); 
        output[j+2] =(w_uint8_t)((input[i] >> 16) & 0xff); 
        output[j+3] =(w_uint8_t)((input[i] >> 24) & 0xff); 
    }
}
   

static void decode(w_uint32_t *output,w_uint8_t *input,w_uint32_t len) 
{ 
    w_uint32_t i,j; 

    for(i = 0,j = 0; j < len; i++,j += 4) 
    output[i] =((w_uint32_t)input[j]) | (((w_uint32_t)input[j+1]) << 8) | 
   (((w_uint32_t)input[j+2]) << 16) | (((w_uint32_t)input[j+3]) << 24); 
}

static void md5_transform(w_uint32_t state[4],w_uint8_t block[64]) 
{ 
   w_uint32_t a = state[0],b = state[1],c = state[2],d = state[3],x[16]; 
   decode(x,block,64); 

   /* Round 1 */ 
   FF(a,b,c,d,x[ 0],S11,0xd76aa478);/* 1 */ 
   FF(d,a,b,c,x[ 1],S12,0xe8c7b756);/* 2 */ 
   FF(c,d,a,b,x[ 2],S13,0x242070db);/* 3 */ 
   FF(b,c,d,a,x[ 3],S14,0xc1bdceee);/* 4 */ 
   FF(a,b,c,d,x[ 4],S11,0xf57c0faf);/* 5 */ 
   FF(d,a,b,c,x[ 5],S12,0x4787c62a);/* 6 */ 
   FF(c,d,a,b,x[ 6],S13,0xa8304613);/* 7 */ 
   FF(b,c,d,a,x[ 7],S14,0xfd469501);/* 8 */ 
   FF(a,b,c,d,x[ 8],S11,0x698098d8);/* 9 */ 
   FF(d,a,b,c,x[ 9],S12,0x8b44f7af);/* 10 */ 
   FF(c,d,a,b,x[10],S13,0xffff5bb1);/* 11 */ 
   FF(b,c,d,a,x[11],S14,0x895cd7be);/* 12 */ 
   FF(a,b,c,d,x[12],S11,0x6b901122);/* 13 */ 
   FF(d,a,b,c,x[13],S12,0xfd987193);/* 14 */ 
   FF(c,d,a,b,x[14],S13,0xa679438e);/* 15 */ 
   FF(b,c,d,a,x[15],S14,0x49b40821);/* 16 */ 

   /* Round 2 */ 
   GG(a,b,c,d,x[ 1],S21,0xf61e2562);/* 17 */ 
   GG(d,a,b,c,x[ 6],S22,0xc040b340);/* 18 */ 
   GG(c,d,a,b,x[11],S23,0x265e5a51);/* 19 */ 
   GG(b,c,d,a,x[ 0],S24,0xe9b6c7aa);/* 20 */ 
   GG(a,b,c,d,x[ 5],S21,0xd62f105d);/* 21 */ 
   GG(d,a,b,c,x[10],S22,0x02441453);/* 22 */ 
   GG(c,d,a,b,x[15],S23,0xd8a1e681);/* 23 */ 
   GG(b,c,d,a,x[ 4],S24,0xe7d3fbc8);/* 24 */ 
   GG(a,b,c,d,x[ 9],S21,0x21e1cde6);/* 25 */ 
   GG(d,a,b,c,x[14],S22,0xc33707d6);/* 26 */ 
   GG(c,d,a,b,x[ 3],S23,0xf4d50d87);/* 27 */ 
   GG(b,c,d,a,x[ 8],S24,0x455a14ed);/* 28 */ 
   GG(a,b,c,d,x[13],S21,0xa9e3e905);/* 29 */ 
   GG(d,a,b,c,x[ 2],S22,0xfcefa3f8);/* 30 */ 
   GG(c,d,a,b,x[ 7],S23,0x676f02d9);/* 31 */ 
   GG(b,c,d,a,x[12],S24,0x8d2a4c8a);/* 32 */ 

   /* Round 3 */ 
   HH(a,b,c,d,x[ 5],S31,0xfffa3942);/* 33 */ 
   HH(d,a,b,c,x[ 8],S32,0x8771f681);/* 34 */ 
   HH(c,d,a,b,x[11],S33,0x6d9d6122);/* 35 */ 
   HH(b,c,d,a,x[14],S34,0xfde5380c);/* 36 */ 
   HH(a,b,c,d,x[ 1],S31,0xa4beea44);/* 37 */ 
   HH(d,a,b,c,x[ 4],S32,0x4bdecfa9);/* 38 */ 
   HH(c,d,a,b,x[ 7],S33,0xf6bb4b60);/* 39 */ 
   HH(b,c,d,a,x[10],S34,0xbebfbc70);/* 40 */ 
   HH(a,b,c,d,x[13],S31,0x289b7ec6);/* 41 */ 
   HH(d,a,b,c,x[ 0],S32,0xeaa127fa);/* 42 */ 
   HH(c,d,a,b,x[ 3],S33,0xd4ef3085);/* 43 */ 
   HH(b,c,d,a,x[ 6],S34,0x04881d05);/* 44 */ 
   HH(a,b,c,d,x[ 9],S31,0xd9d4d039);/* 45 */ 
   HH(d,a,b,c,x[12],S32,0xe6db99e5);/* 46 */ 
   HH(c,d,a,b,x[15],S33,0x1fa27cf8);/* 47 */ 
   HH(b,c,d,a,x[ 2],S34,0xc4ac5665);/* 48 */ 

   /* Round 4 */ 
   II(a,b,c,d,x[ 0],S41,0xf4292244);/* 49 */ 
   II(d,a,b,c,x[ 7],S42,0x432aff97);/* 50 */ 
   II(c,d,a,b,x[14],S43,0xab9423a7);/* 51 */ 
   II(b,c,d,a,x[ 5],S44,0xfc93a039);/* 52 */ 
   II(a,b,c,d,x[12],S41,0x655b59c3);/* 53 */ 
   II(d,a,b,c,x[ 3],S42,0x8f0ccc92);/* 54 */ 
   II(c,d,a,b,x[10],S43,0xffeff47d);/* 55 */ 
   II(b,c,d,a,x[ 1],S44,0x85845dd1);/* 56 */ 
   II(a,b,c,d,x[ 8],S41,0x6fa87e4f);/* 57 */ 
   II(d,a,b,c,x[15],S42,0xfe2ce6e0);/* 58 */ 
   II(c,d,a,b,x[ 6],S43,0xa3014314);/* 59 */ 
   II(b,c,d,a,x[13],S44,0x4e0811a1);/* 60 */ 
   II(a,b,c,d,x[ 4],S41,0xf7537e82);/* 61 */ 
   II(d,a,b,c,x[11],S42,0xbd3af235);/* 62 */ 
   II(c,d,a,b,x[ 2],S43,0x2ad7d2bb);/* 63 */ 
   II(b,c,d,a,x[ 9],S44,0xeb86d391);/* 64 */ 

   state[0] += a; 
   state[1] += b; 
   state[2] += c; 
   state[3] += d; 
   wind_memset((char*)x,0,sizeof(x)); 
}

void wind_md5_init(w_md5_ctx_s *ctx)
{ 
    ctx->bits[0] = 0;
    ctx->bits[1] = 0; 
    ctx->state[0] = 0x67452301; 
    ctx->state[1] = 0xefcdab89; 
    ctx->state[2] = 0x98badcfe; 
    ctx->state[3] = 0x10325476; 
}

void wind_md5_update(w_md5_ctx_s *ctx,w_uint8_t*data,w_uint32_t len) 
{ 
    w_uint32_t i,index,part_len; 
    index =(w_uint32_t)((ctx->bits[0] >> 3) & 0x3F); 

    if((ctx->bits[0] +=((w_uint32_t)len << 3)) <((w_uint32_t)len << 3)) 
        ctx->bits[1]++; 
    ctx->bits[1] +=((w_uint32_t)len >> 29); 

    part_len = 64 - index; 

    if(len >= part_len) 
    { 
        wind_memcpy((char*)&ctx->buffer[index],(char*)data,part_len); 
        md5_transform(ctx->state,ctx->buffer); 
        for(i = part_len; i + 63 < len; i += 64) 
            md5_transform(ctx->state,&data[i]); 
        index = 0; 
    }
    else 
        i = 0; 

    wind_memcpy((char*)&ctx->buffer[index],(char*)&data[i],len-i); 
}
   
void wind_md5_final(w_md5_ctx_s *ctx,w_uint8_t digest[16])         
{ 
    w_uint8_t bits[8]; 
    w_uint32_t index,pad_len; 

    encode(bits,ctx->bits,8); 

    index =(w_uint32_t)((ctx->bits[0] >> 3) & 0x3f); 
    pad_len =(index < 56) ? (56 - index) : (120 - index); 
    wind_md5_update(ctx,PADDING,pad_len); 

    wind_md5_update(ctx,bits,8); 

    encode(digest,ctx->state,16); 

    wind_memset((char*)ctx,0,sizeof(*ctx)); 
}
   

   

   

#if 0
void md5_test(void)
{ 
    w_md5_ctx_s md5c; 
    wind_md5_init(&md5c);
    wind_md5_update(ctx,(w_uint8_t *)string1,len);
    wind_md5_update(ctx,(w_uint8_t *)string2,len);
    wind_md5_update(ctx,(w_uint8_t *)string3,len);
    wind_md5_final(&md5c,digest);
}
#endif

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus