#cross compile tools
export CROSS_COMPILE_BIN=/usr/bin
export SYSROOT=/opt/arm-linux-glibc-zte-cmcc/arm-buildroot-linux-gnueabi/sysroot
export CROSS_COMPILE=$(CROSS_COMPILE_BIN)/
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
WINDOS_SRC = bsp/$(ARCH)
WINDOS_SRC += bsp/$(ARCH)/$(CPU)
WINDOS_SRC += bsp/$(ARCH)/$(CPU)/$(BOARD)
WINDOS_SRC += src
export WINDOS_SRC


#include directory
WINDOS_INC += -I$(ROOTDIR)/bsp/$(ARCH)
WINDOS_INC += -I$(ROOTDIR)/bsp/$(ARCH)/$(CPU)
WINDOS_INC += -I$(ROOTDIR)/bsp/$(ARCH)/$(CPU)/$(BOARD)
WINDOS_INC += -I$(ROOTDIR)/bsp/$(ARCH)/$(CPU)/$(BOARD)/port
WINDOS_INC += -I$(ROOTDIR)/src/application
WINDOS_INC += -I$(ROOTDIR)/src/common
WINDOS_INC += -I$(ROOTDIR)/src/core
WINDOS_INC += -I$(ROOTDIR)/src/hal
WINDOS_INC += -I$(ROOTDIR)/src/lib
WINDOS_INC += -I$(ROOTDIR)/src/systhread
WINDOS_INC += -I$(ROOTDIR)/src/module
WINDOS_INC += -I$(ROOTDIR)/src/module/cjson
WINDOS_INC += -I$(ROOTDIR)/src/module/cutest/framework
WINDOS_INC += -I$(ROOTDIR)/src/module/db/framework
WINDOS_INC += -I$(ROOTDIR)/src/module/db/dbbase
WINDOS_INC += -I$(ROOTDIR)/src/module/dict
WINDOS_INC += -I$(ROOTDIR)/src/module/lua-5.3.4/src
WINDOS_INC += -I$(ROOTDIR)/src/module/shell/framework
WINDOS_INC += -I$(ROOTDIR)/src/module/vfs
WINDOS_INC += -I$(ROOTDIR)/src/module/vfs/hostfs
WINDOS_INC += -I$(ROOTDIR)/src/module/vfs/listfs
WINDOS_INC += -I$(ROOTDIR)/src/module/vfs/treefs

CFLAGS = -g -Wall ${WINDOS_INC}
export WINDOS_INC CFLAGS

all: wind-os 
wind-os:
	@echo "WINDOS_SRC="$(WINDOS_SRC)
	@echo "ROOTDIR="${ROOTDIR}
	for dir in $(WINDOS_SRC); do \
		make -C $(ROOTDIR)/$$dir profile; \
	done


clean: 
	for dir in $(WINDOS_SRC); do \
		echo "enter dir:"$$dir; \
		make -C $(ROOTDIR)/$$dir clean; \
	done


