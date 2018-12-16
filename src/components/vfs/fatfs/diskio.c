/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "wind_string.h"		
#include "wind_debug.h"		
#include "file_port.h"		
#define SEC_COUNT 65535
#define SEC_SIZE 512

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
static BYTE blkbuf[SEC_COUNT*SEC_SIZE];


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	int result;
	return STA_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)

{
	DSTATUS stat;
	int result;
    wind_debug("disk_initialize pdrv:%d",pdrv);
	return STA_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;
    DWORD offset = sector * SEC_SIZE * count;
    wind_debug("disk_read sector:%d,count:%d",sector,count);
    wind_memcpy(buff,&blkbuf[offset],SEC_SIZE);
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;
    DWORD offset = sector * SEC_SIZE * count;
    wind_debug("disk_write sector:%d,count:%d",sector,count);
    wind_memcpy(&blkbuf[offset],buff,SEC_SIZE);
	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;
	wind_debug("ioctl cmd:%d",cmd);
    switch(cmd)
    {
    case CTRL_SYNC :
        return RES_OK;
    case GET_SECTOR_COUNT:
        *((DWORD*)buff) = SEC_COUNT;
        return RES_OK;
    case GET_BLOCK_SIZE:
        *((DWORD*)buff) = SEC_SIZE;
        return RES_OK;
    case GET_SECTOR_SIZE:
        *((DWORD*)buff) = SEC_SIZE;
        return RES_OK;
    default:
        break;
    }

	return RES_PARERR;
}

DRESULT disk_load(BYTE pdrv,char *filename)
{
    int len;
    wind_debug("disk_load drvid:%d",pdrv);
    wind_memset(blkbuf,0,sizeof(blkbuf));
    len = read_file(filename,0,blkbuf,sizeof(blkbuf));
    if(len <= 0)
    {
        wind_memset(blkbuf,0,sizeof(blkbuf));
        write_file(filename,0,blkbuf,sizeof(blkbuf));
    }
    return RES_OK;
}

DRESULT disk_flush(BYTE pdrv,char *filename)
{
    wind_debug("disk_flush drvid:%d",pdrv);
    write_file(filename,0,blkbuf,sizeof(blkbuf));
    return RES_OK;
}


