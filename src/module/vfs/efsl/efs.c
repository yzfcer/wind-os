/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : efs.h                                                            *
* Revision : Initial developement                                             *
* Description : This should become the wrapper around efs. It will contain    *
*               functions like efs_init etc.                                  *
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

/*****************************************************************************/
#include "efs.h"
/*****************************************************************************/

/* ****************************************************************************  
 * esint8 efs_init(EmbeddedFileSystem * efs,eint8* opts)
 * Description: This function initialises all subelements of a filesystem.
 * It sets the pointerchain and verifies each step.
 * Return value: 0 on success and -1 on failure.
*/
esint8 efs_init(EmbeddedFileSystem * efs,eint8* opts)
{
	if(if_initInterface(&efs->myCard, opts))
	{
		ioman_init(&efs->myIOman,&efs->myCard,0);
		disc_initDisc(&efs->myDisc, &efs->myIOman);
		part_initPartition(&efs->myPart, &efs->myDisc);
		if(efs->myPart.activePartition==-1){
			efs->myDisc.partitions[0].type=PT_FAT32;
			efs->myDisc.partitions[0].LBA_begin=0;
			efs->myDisc.partitions[0].numSectors=efs->myCard.sectorCount;	
			/*efs->myPart.activePartition = 0;*/
			/*efs->myPart.disc = &(efs->myDisc);*/
			part_initPartition(&efs->myPart, &efs->myDisc);
		}
		/*part_initPartition(&efs->myPart, &efs->myDisc);*/
		if(fs_initFs(&efs->myFs, &efs->myPart))
			return(-2);
		return(0);
	}
	return(-1);
}
/*****************************************************************************/


