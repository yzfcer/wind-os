#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "types.h"

#ifndef __LINUXFILE_H_
#define __LINUXFILE_H_


/*************************************************************\
              hwInterface
               ----------
* FILE* 	imagefile		File emulation of hw interface.
* long		sectorCount		Number of sectors on the file.
\*************************************************************/
struct __hwInterface{
	void 	*imageFile; 
	eint32  sectorCount;
    eint32  offset;
};
typedef struct __hwInterface hwInterface;

eint16 if_initInterface(hwInterface* hwif,eint8* hwifName);
esint8 if_erase(hwInterface* hwif,euint32 address,euint32 count);
esint8 if_readBuf(hwInterface* hwif,euint32 address,euint8* buf);
esint8 if_writeBuf(hwInterface* hwif,euint32 address,euint8* buf);
eint16 if_setPos(hwInterface* hwif,euint32 address);
eint16 if_load(hwInterface* hwif,char *filename);
eint16 if_flush(hwInterface* hwif,char *filename);

#endif
