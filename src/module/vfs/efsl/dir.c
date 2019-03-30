/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : dir.c                                                            *
* Revision : Initial developement                                             *
* Description : The functions of dir.c are part of fs.c, they deal with all   *
*               the directory specific stuff.                                 *
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
*                                                    (c)2004 Lennart Yseboodt *
*                                                    (c)2004 Michael De Nil   *
\*****************************************************************************/

/*****************************************************************************/
#include "dir.h"
/*****************************************************************************/

/* ****************************************************************************  
 * void dir_getFileStructure(FileSystem *fs,FileRecord *filerec,FileLocation *loc)
 * Description: This function stores the filerecord located at loc in filerec.
 * It fetches the required sector for this.
 * Return value: void
*/
void dir_getFileStructure(FileSystem *fs,FileRecord *filerec,FileLocation *loc)
{
	euint8 *buf;

	buf=part_getSect(fs->part,loc->Sector,IOM_MODE_READONLY);
	*filerec=*(((FileRecord*)buf)+loc->Offset);
	part_relSect(fs->part,buf);
}	

/*****************************************************************************/

/* ****************************************************************************  
 * void dir_createDirectoryEntry(FileSystem *fs,FileRecord *filerec,FileLocation *loc)
 * Description: This function writes the filerecord stored in filerec to disc at
 * location loc. 
 * Return value: void
*/
void dir_createDirectoryEntry(FileSystem *fs,FileRecord *filerec,FileLocation *loc)
{
	euint8 *buf;
	
	buf = part_getSect(fs->part,loc->Sector,IOM_MODE_READWRITE);
	memCpy(filerec,buf+(loc->Offset*sizeof(*filerec)),sizeof(*filerec));
	part_relSect(fs->part,buf);
}
/*****************************************************************************/

/* ****************************************************************************  
 * void dir_createDefaultEntry(FileSystem *fs,FileRecord *filerec,eint8* fatfilename)
 * Description: This function fills in a filerecord with safe default values, and
 * a given fatfilename. If your system has a means of knowing time, here is an 
 * excellent place to apply it to the filerecord.  
 * Return value: void
*/
void dir_createDefaultEntry(FileSystem *fs,FileRecord *filerec,eint8* fatfilename)
{
	memCpy(fatfilename,filerec->FileName,11);
	filerec->Attribute=0x00;
	filerec->NTReserved=0x00;
	filerec->MilliSecTimeStamp=0x00;
	filerec->CreatedTime=time_getTime();
	filerec->CreatedDate=time_getDate(); 
	filerec->AccessDate=filerec->CreatedDate;
	filerec->FirstClusterHigh=0x0000;
	filerec->WriteTime=filerec->CreatedTime;
	filerec->WriteDate=filerec->CreatedDate;
	filerec->FirstClusterLow=0x0000;
	filerec->FileSize=0x00000000;
}
/*****************************************************************************/

/* ****************************************************************************  
 * void dir_setFirstCluster(File *file,euint32 cluster_addr)
 * Description: This function requires modification to release it from
 * depending on the file object.
 * Return value:
*/
void dir_setFirstCluster(FileSystem *fs,FileLocation *loc,euint32 cluster_addr)
{
	euint8 *buf;
 	
 	buf = part_getSect(fs->part,loc->Sector,IOM_MODE_READWRITE);
	(((FileRecord*)buf)+loc->Offset)->FirstClusterHigh=cluster_addr>>16;
	(((FileRecord*)buf)+loc->Offset)->FirstClusterLow=cluster_addr&0xFFFF;	
	part_relSect(fs->part,buf);
}
/*****************************************************************************/

/* ****************************************************************************  
 * void dir_setFileSize(File *file,euint32 numbytes)
 * Description: Rewrite
 *
 * Return value:
*/
void dir_setFileSize(FileSystem *fs, FileLocation *loc,euint32 numbytes)
{
	euint8 *buf;
	
	buf = part_getSect(fs->part,loc->Sector,IOM_MODE_READWRITE);
	(((FileRecord*)buf)+loc->Offset)->FileSize=numbytes;
	part_relSect(fs->part,buf);
}
/*****************************************************************************/

esint8 dir_updateDirectoryEntry(FileSystem *fs,FileRecord *entry,FileLocation *loc)
{
	euint8 *buf;
	
	buf = part_getSect(fs->part,loc->Sector,IOM_MODE_READWRITE);
	memCpy(entry,buf+(loc->Offset*sizeof(*entry)),sizeof(*entry));
	part_relSect(fs->part,buf);
	return(0);
}

euint32 dir_findFileinBuf(euint8 *buf, eint8 *fatname, FileLocation *loc)
{
	FileRecord fileEntry;
	euint8 c;
	
	for(c=0; c<16; c++)
	{
		fileEntry = *(((FileRecord*)buf) + c);
		/* Check if the entry is for short filenames */
		if( !( (fileEntry.Attribute & 0x0F) == 0x0F ) )
		{
			if( strMatch((eint8*)fileEntry.FileName,fatname,11) == 0 )
			{
				/* The entry has been found, return the location in the dir */
				loc->Offset = c;
				loc->attrib = fileEntry.Attribute;
				return
						(
						(((euint32 )fileEntry.FirstClusterHigh)<<16)
						+ fileEntry.FirstClusterLow
						);
			}
		}
	}
	return(0);
}

euint32 dir_findFreeEntryinBuf(euint8* buf, FileLocation *loc)
{
	FileRecord fileEntry;
	euint8 c;
	
	for(c=0;c<16;c++){
		fileEntry = *(((FileRecord*)buf) + c);
		if( !( (fileEntry.Attribute & 0x0F) == 0x0F ) ){
			if(fileEntry.FileName[0] == 0x00 ||
			   fileEntry.FileName[0] == 0xE5 ){
				loc->Offset=c;
				return(1);
			}
		}
	}
	return(0);
}

/* ****************************************************************************  
 * euint32  dir_findinBuf(euint8 *buf, eint8 *fatname, FileLocation *loc)
 * Description: This function searches for a given fatfilename in a buffer.
 * Return value: Returns 0 on not found, and the firstcluster when the name is found.
*/
euint32  dir_findinBuf(euint8 *buf, eint8 *fatname, FileLocation *loc, euint8 mode)
{
	switch(mode){
		case DIRFIND_FILE:
			return(dir_findFileinBuf(buf,fatname,loc));
			break;
		case DIRFIND_FREE:
			return(dir_findFreeEntryinBuf(buf,loc));
			break;
		default:
			return(0);
			break;
	}
	return(0);
}
/*****************************************************************************/

euint32 dir_findinCluster(FileSystem *fs,euint32 cluster,eint8 *fatname, FileLocation *loc, euint8 mode)
{
	euint8 c,*buf=0;
	euint32 fclus;
	
	for(c=0;c<fs->volumeId.SectorsPerCluster;c++){
		buf = part_getSect(fs->part,fs_clusterToSector(fs,cluster)+c,IOM_MODE_READONLY);
		if((fclus=dir_findinBuf(buf,fatname,loc,mode))){
			loc->Sector=fs_clusterToSector(fs,cluster)+c;
			part_relSect(fs->part,buf);
			return(fclus);
		}
	}
	part_relSect(fs->part,buf);
	return(0);
}

euint32 dir_findinDir(FileSystem *fs, eint8* fatname,euint32 firstcluster, FileLocation *loc, euint8 mode)
{
	euint32 c=0,cluster;
	ClusterChain Cache;
	
	Cache.DiscCluster = Cache.FirstCluster = firstcluster;
	Cache.LogicCluster = Cache.LastCluster = Cache.Linear = 0;
	
	if(firstcluster <= 1){
		return(dir_findinRootArea(fs,fatname,loc,mode));	
	}
	
	while(!fat_LogicToDiscCluster(fs,&Cache,c++)){
		if((cluster=dir_findinCluster(fs,Cache.DiscCluster,fatname,loc,mode))){
			return(cluster);
		}
	}
	return(0);
}

euint32 dir_findinRootArea(FileSystem *fs,eint8* fatname, FileLocation *loc, euint8 mode)
{
	euint32 c,fclus;
	euint8 *buf=0;
	
	if((fs->type != FAT12) && (fs->type != FAT16))return(0);
	//yzfcer modify£¬maybe a bug occured
    //for(c=fs->FirstSectorRootDir;c<=(fs->FirstSectorRootDir+fs->volumeId.RootEntryCount/32);c++){
	for(c=fs->FirstSectorRootDir;c<(fs->FirstSectorRootDir+fs->volumeId.RootEntryCount/32);c++){
		buf = part_getSect(fs->part,c,IOM_MODE_READONLY);
		if((fclus=dir_findinBuf(buf,fatname,loc,mode))){
			loc->Sector=c;
			part_relSect(fs->part,buf);
			return(fclus);
		}	
		part_relSect(fs->part,buf);	
	}
	part_relSect(fs->part,buf);
	return(0);
}

esint8 dir_getFatFileName(eint8* filename, eint8* fatfilename)
{
	eint8 ffnamec[11],*next,nn=0;
	
	memClr(ffnamec,11); memClr(fatfilename,11);
	next = filename;
	
	if(*filename=='/')next++;
	
	while((next=file_normalToFatName(next,ffnamec))){
		memCpy(ffnamec,fatfilename,11);	
		nn++;
	}
	if(nn)return(1);
	return(0);
}

esint8 dir_addCluster(FileSystem *fs,euint32 firstCluster)
{
	euint32 lastc,logicalc;
	ClusterChain cache;
		
	fs_initClusterChain(fs,&cache,firstCluster);
	if(fat_allocClusterChain(fs,&cache,1)){
		return(-1);
	}
	lastc = fs_getLastCluster(fs,&cache);
	if(CLUSTER_PREALLOC_DIRECTORY){
		if(fat_allocClusterChain(fs,&cache,CLUSTER_PREALLOC_DIRECTORY)){
			return(-1);
		}
		logicalc = fat_DiscToLogicCluster(fs,firstCluster,lastc);
		while(!fat_LogicToDiscCluster(fs,&cache,++logicalc)){
			fs_clearCluster(fs,cache.DiscCluster);
		}
	}else{
			fs_clearCluster(fs,lastc);
	}
	return(0);
}


