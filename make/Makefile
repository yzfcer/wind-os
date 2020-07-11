#cross compile tools
export CROSS_COMPILE_BIN=/usr/bin
export SYSROOT=/opt/arm-linux-glibc-zte-cmcc/arm-buildroot-linux-gnueabi/sysroot
export CROSS_COMPILE=$(CROSS_COMPILE_BIN)/
CROSS_COMPILE_INCLUDE += -I/usr/include
CROSS_COMPILE_INCLUDE += -I/usr/include/x86_64-linux-gnu
export CROSS_COMPILE_INCLUDE


export CC=$(CROSS_COMPILE)gcc
export LD=$(CROSS_COMPILE)ld
export CXX=$(CROSS_COMPILE)g++
export CCC=$(CROSS_COMPILE)g++
export CPP=$(CROSS_COMPILE)cpp
export STRIP=$(CROSS_COMPILE)strip
export OBJCOPY=$(CROSS_COMPILE)objcopy
export OBJDUMP=$(CROSS_COMPILE)objdump
export NM=$(CROSS_COMPILE)nm
export RANLIB=$(CROSS_COMPILE)ranlib
export SIZE=$(CROSS_COMPILE)size
export AR=$(CROSS_COMPILE)ar

#local host compile tools
export HOST_GCC_BIN=/usr/bin
export HOST_COMPILE=$(HOST_GCC_BIN)/
export HOST_CC=$(HOST_COMPILE)gcc
export HOST_LD=$(HOST_COMPILE)ld
export HOST_CXX=$(HOST_COMPILE)g++
export HOST_CCC=$(HOST_COMPILE)g++
export HOST_CPP=$(HOST_COMPILE)cpp
export HOST_STRIP=$(HOST_COMPILE)strip
export HOST_OBJCOPY=$(HOST_COMPILE)objcopy
export HOST_OBJDUMP=$(HOST_COMPILE)objdump
export HOST_NM=$(HOST_COMPILE)nm
export HOST_RANLIB=$(HOST_COMPILE)ranlib
export HOST_SIZE=$(HOST_COMPILE)size
export HOST_AR=$(HOST_COMPILE)ar

#project options
export ROOTDIR=$(shell pwd)/..
export ARCH=linux
export CPU=linux
export BOARD=linux_simu


#source directory
WIND_SRC_DIR = bsp
#WIND_SRC_DIR += bsp/$(ARCH)/$(CPU)
#WIND_SRC_DIR += bsp/$(ARCH)/$(CPU)/$(BOARD)
WIND_SRC_DIR += src
export WIND_SRC_DIR
export OBJS_DIR = ${ROOTDIR}/objs


#include directory
WIND_INC_DIR += -I$(ROOTDIR)/bsp/$(ARCH)
WIND_INC_DIR += -I$(ROOTDIR)/bsp/$(ARCH)/$(CPU)
WIND_INC_DIR += -I$(ROOTDIR)/bsp/$(ARCH)/$(CPU)/$(BOARD)
WIND_INC_DIR += -I$(ROOTDIR)/bsp/$(ARCH)/$(CPU)/$(BOARD)/port
WIND_INC_DIR += -I$(ROOTDIR)/src/common
WIND_INC_DIR += -I$(ROOTDIR)/src/core
WIND_INC_DIR += -I$(ROOTDIR)/src/hal
WIND_INC_DIR += -I$(ROOTDIR)/src/lib
WIND_INC_DIR += -I$(ROOTDIR)/src/systhread
WIND_INC_DIR += -I$(ROOTDIR)/src/module
WIND_INC_DIR += -I$(ROOTDIR)/src/module/cjson
WIND_INC_DIR += -I$(ROOTDIR)/src/module/cutest/framework
WIND_INC_DIR += -I$(ROOTDIR)/src/module/db/framework
WIND_INC_DIR += -I$(ROOTDIR)/src/module/db/dbbase
WIND_INC_DIR += -I$(ROOTDIR)/src/module/dict
WIND_INC_DIR += -I$(ROOTDIR)/src/module/lua-5.3.4/src
WIND_INC_DIR += -I$(ROOTDIR)/src/module/shell/framework
WIND_INC_DIR += -I$(ROOTDIR)/src/module/vfs
WIND_INC_DIR += -I$(ROOTDIR)/src/module/vfs/hostfs
WIND_INC_DIR += -I$(ROOTDIR)/src/module/vfs/listfs
WIND_INC_DIR += -I$(ROOTDIR)/src/module/vfs/treefs
WIND_INC_DIR += -I$(ROOTDIR)/src/module/xml
WIND_INC_DIR += -I$(ROOTDIR)/src/module/log
WIND_INC_DIR += -I$(ROOTDIR)/src/module/xdemom
WIND_INC_DIR += -I$(ROOTDIR)/src/module/ydemom
WIND_INC_DIR += -I$(ROOTDIR)/src/module/net
WIND_INC_DIR += -I$(ROOTDIR)/src/module/net/common
WIND_INC_DIR += -I$(ROOTDIR)/src/module/net/stack
WIND_INC_DIR += -I$(ROOTDIR)/src/module/net/netcmd
WIND_INC_DIR += -I$(ROOTDIR)/src/module/net/layer5


CFLAGS = -g -Wall -rdynamic ${WIND_INC_DIR} ${CROSS_COMPILE_INCLUDE}
CLIBFLAGS = -g -rdynamic -L/usr/lib/86_64-linux-gnu -lm 

export WIND_INC_DIR CFLAGS CLIBFLAGS

OBJLIST=$(shell find $(ROOTDIR) -name *.o | tr ' ' '\ ' | tr '\n' ' ')

export OBJLIST




all: boot os
boot:
	@echo "make wind-boot"
	make -f mkboot

.PHONY:os
os:
	@echo "make wind-os"
	make -f mkos

.PHONY:clean
clean: 
	for dir in $(WIND_SRC_DIR); do \
		echo "enter dir:"$$dir; \
		cd $(ROOTDIR)/$$dir && make clean || exit 1; \
	done
	#-rm -f $(TARGET)
	-rm -rf $$OBJLIST



