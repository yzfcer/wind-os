/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  extract.h                                                       *
* Revision :  Initial developement                                            *
* Description : Headerfile for extract.c                                      *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Lennart Yseboodt *
\*****************************************************************************/

#ifndef __EXTRACT_H_
#define __EXTRACT_H_

/*****************************************************************************/
#include "disc.h"
#include "types.h"
#include "config.h"
/*****************************************************************************/

#ifdef BIG_ENDIAN

#define ltb_end16(x)  ((((uint16)(x) & 0xff00) >> 8) | \
                      (((uint16)(x) & 0x00ff) << 8))
#define ltb_end32(x)  ((((uint32)(x) & 0xff000000) >> 24) | \
                       (((uint32)(x) & 0x00ff0000) >> 8)  | \
                       (((uint32)(x) & 0x0000ff00) << 8)  | \
                       (((uint32)(x) & 0x000000ff) << 24))

#else

#define ltb_end16(x)  (x)
#define ltb_end32(x)  (x)

#endif

#define btl_end16 ltb_end16
#define btl_end32 ltb_end32


/*****************************************************************************/

euint16 ex_getb16(euint8* buf,euint32 offset);
void ex_setb16(euint8* buf,euint32 offset,euint16 data);

euint32 ex_getb32(euint8* buf,euint32 offset);
void ex_setb32(euint8* buf,euint32 offset,euint32 data);

void ex_getPartitionField(euint8* buf,PartitionField* pf, euint32 offset);
void ex_setPartitionField(euint8* buf,PartitionField* pf, euint32 offset);

#endif

