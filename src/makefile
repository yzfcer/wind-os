CUR_DIR=$(shell pwd)
#Read the subdirectory of the current directory
SUBDIRS=application
SUBDIRS+=core
SUBDIRS+=common
SUBDIRS+=hal
#SUBDIRS+=lib
SUBDIRS+=module
SUBDIRS+=systhread

#Export all source and target files
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
#SUBSRC=$(shell find $(SUBDIRS) -name '*.c')
#SUBOBJ=$(SUBSRC:%.c=%.o)
profile:$(SUBDIRS) $(CUR_OBJS) install
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAGS) -o $@
ECHO:
	@echo $(SUBDIRS)
.PHONY:install
	cp -rf ./*.o $(OBJS_DIR)/
.PHONY:clean
clean:
	for dir in $(SUBDIRS); do \
		echo "enter dir:"$$dir; \
		cd $(CUR_DIR)/$$dir && make clean; \
	done
	-rm -rf *.o
