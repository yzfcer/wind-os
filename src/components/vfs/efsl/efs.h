/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : efs.h                                                            *
* Revision : Initial developement                                             *
* Description : Headerfile for efs.c                                          *
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
\*****************************************************************************/

#ifndef __EFS_H__
#define __EFS_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
#include "interface.h"
#include "disc.h"
#include "partition.h"
#include "fs.h"
#include "file.h"
#include "time.h"
/*****************************************************************************/

typedef File EmbeddedFile;

struct EmbeddedFileSystem{
	hwInterface myCard;
	IOManager myIOman;
	Disc myDisc;
	Partition myPart;
	FileSystem myFs;
};
typedef struct  EmbeddedFileSystem EmbeddedFileSystem;

esint8 efs_init(EmbeddedFileSystem * efs,eint8 * opts);


#endif

