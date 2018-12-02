/*****************************************************************************/
#include "interface.h"
#include "file_port.h"
/*****************************************************************************/
#define SEC_COUNT 10240
#define SEC_SIZE 512
/* ****************************************************************************  
 * short if_initInterface(hwInterface* hwif, char* fileName)
 * Description: This function should bring the hardware described in hwif in a
 * ready state to receive and retrieve data.
 * Return value: Return 0 on succes and -1 on failure.
*/
static w_uint8_t blkbuf[SEC_COUNT*SEC_SIZE];
eint16 if_initInterface(hwInterface* hwif, eint8* hwifName)
{
    hwif->sectorCount = SEC_COUNT;
    hwif->offset = 0;
	return(1);
}
/*****************************************************************************/ 

esint8 if_erase(hwInterface* hwif,euint32 address,euint32 count)
{
    euint32 offset = address * SEC_SIZE;
    euint32 size = count * SEC_SIZE;
    wind_memset(&blkbuf[offset],0,size);
    return (1);
}
/* ****************************************************************************  
 * short if_readBuf(hwInterface* hwif,unsigned long address,unsigned char* buf)
 * Description: This function should fill the characterpointer buf with 512 
 * bytes, offset by address*512 bytes. Adress is thus a LBA address.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 if_readBuf(hwInterface* hwif,euint32 address,euint8* buf)
{
    euint32 offset = address * SEC_SIZE;
    wind_memcpy(buf,&blkbuf[offset],SEC_SIZE);
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_writeBuf(hwInterface* hwif,unsigned long address,unsigned char* buf)
 * Description: This function writes 512 bytes from uchar* buf to the hardware
 * disc described in hwif. The write offset should be address sectors of 512 bytes.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 if_writeBuf(hwInterface* hwif,euint32 address,euint8* buf)
{
    euint32 offset = address * SEC_SIZE;
    wind_memcpy(&blkbuf[offset],buf,SEC_SIZE);
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_setPos(hwInterface* hwif,unsigned long address)
 * Description: This function may or may not be required. It would set the write
 * or read buffer offset by 512*address bytes from the beginning of the disc.
 * Return value: Return 0 on success and -1 on failure.
*/
eint16 if_setPos(hwInterface* hwif,euint32 address)
{
    euint32 offset = address * SEC_SIZE;
    hwif->offset = offset;
	return(0);
}


eint16 if_load(hwInterface* hwif)
{
    esint32 len;
    wind_memset(blkbuf,0,sizeof(blkbuf));
    len = read_file("fat32.img",0,blkbuf,sizeof(blkbuf));
    if(len <= 0)
    {
        wind_memset(blkbuf,0,sizeof(blkbuf));
        write_file("fat32.img",0,blkbuf,sizeof(blkbuf));
    }
    return (0);
}

eint16 if_flush(hwInterface* hwif)
{
    write_file("fat32.img",0,blkbuf,sizeof(blkbuf));
    return (0);
}

/*****************************************************************************/ 

