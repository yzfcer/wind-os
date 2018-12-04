/*****************************************************************************/
#include "mkfs.h"
/*****************************************************************************/

signed short mkfs_makevfat(Partition *part)
{
	unsigned long c,cc,ret;
	unsigned long ns,fs,ds,dc;
	unsigned char buf[512];
	
	ns=part->disc->partitions[part->activePartition].numSectors;
	
	if( ns < 66581 ){
		wind_debug("This is not possible due to insufficient sectors. Sorry\n");
		return(MKFS_ERR_TOOLITTLESECTORS);
	}
	
	ret=0;
	
	for(c=1<<6;c>=1;c>>=1){
		
		/* First guess */
		ds = ns - 32;
		fs = ((ds/c)+127)/128;
		/* ds was guess too large, so fs is too large now too. */
		
		for(cc=0;cc<2;cc++){
		
			/* Round 2, error round */
			ds = ns - 32 - 2*fs;
			fs = ((ds/c)+127)/128; 
			/* Since fs was too large, ds became too small. So the fs for this small ds is too small as well. */
			
			/* Round 3, correction round */
			ds = ns - 32 - 2*fs;
			fs = ((ds/c)+127)/128; 
			/* The fs was too small, so ds was too large. The calculated fs should be slightly too large. */
		
		}
		
		/* Round 4, finalise */
		ds = ns - 32 - 2*fs; 

		dc = ds / c;
		if(ret<(fs*128-dc)/128)ret=(fs*128-dc)/128;
		
		/* Check if with current setting we have a valid fat ? */
		
		if(dc >= 65525 + 16){
			break;
		}
	}

	/* Generate BPB */
	memClr(buf,512);
	
	/* Boot code */
	*(buf+0)=0xE9;	*(buf+1)=0x00;	*(buf+2)=0x00; /* RESET */
	
	/* OEM name */
	memCpy("DSCOSMSH",buf+3,8);
	
	/* Bytes/Sector */
	*((unsigned short*)(buf+11)) = 512;
	
	/* Sectors/Cluster */
	*(buf+13) = c;
	
	/* Reserved Sectors */
	*((unsigned short*)(buf+14)) = 32;
	
	/* Number of FAT Tables */
	*(buf+16) = 2;
	
	/* RootEntryCount */
	*((unsigned short*)(buf+17)) = 0;
	
	/* Total Sector Count __16 */
	*((unsigned short*)(buf+19)) = 0;
	
	/* Media (crap) */
	*(buf+21) = 0xF8;
	
	/* FAT size 16 */
	*((unsigned short*)(buf+22)) = 0;
	
	/* Total Sector Count __32 */
	*((unsigned long*)(buf+32)) = ns;
	
	/* Fat Size 32 */
	*((unsigned long*)(buf+36)) = fs;
	
	/* First Cluster fat32_root Dir */
	*((unsigned long*)(buf+44)) = 2;
	
	/* VolumeID */
	*((unsigned long*)(buf+67)) = 0x13371337;
	
	/* Volume Label */
	memCpy("DISCOSMASH!",buf+71,11);
	
	/* Filesystemtype */
	memCpy("FAT32   ",buf+82,8);
	
	/* Magic */
	*(buf+510) = 0x55; *(buf+511) = 0xAA;
	
	part_writeBuf(part,0,buf);
	
	memClr(buf,512);
	for(c=32;c<(32+2*fs);c++){
		part_writeBuf(part,c,buf);
	}
	*(((unsigned long*)buf)  )=0x0FFFFFF8;
	*(((unsigned long*)buf)+1)=0x0FFFFFFF;
	*(((unsigned long*)buf)+2)=0x0FFFFFF8;
	part_writeBuf(part,32,buf);
	part_writeBuf(part,32+fs,buf);
	
	return(0);
}
