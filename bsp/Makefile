#export ROOTDIR=`dirname \`pwd\`/../`
export ROOTDIR=$(shell pwd)/../
#export ROOTDIR=$(shell pwd $(MAKEDIR)) 
export ARCH=linux
export CPU=linux
export BOARD=linux_simu

#source directory
WINDOS_SRC = bsp/$(ARCH)
WINDOS_SRC += bsp/$(ARCH)/$(CPU)
WINDOS_SRC += bsp/$(ARCH)/$(CPU)/$(BOARD)
WINDOS_SRC += src
export WINDOS_SRC
WINDOS_INC = $(WINDOS_SRC)

CFLAGS = -g -Wall -I${WINDOS_INC}
SUBDIRS=foo bar baz
all: wind-os 
wind-os:
	@echo "WINDOS_SRC="$(WINDOS_SRC)
	@echo "ROOTDIR="${ROOTDIR}
	for dir in $(WINDOS_SRC); do \
		echo "enter dir:"$$dir; \
		make -C $(ROOTDIR)$$dir; \
	done


clean: 
	for subdir in `echo $(WINDOS_SRC)`; \
	do \
		make -C $(subdir) ; \
	done


