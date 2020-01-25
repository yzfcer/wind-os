CUR_DIR=$(shell pwd)
#子目录的Makefile直接读取其子目录就行
SUBDIRS=application
SUBDIRS+=core
SUBDIRS+=common
SUBDIRS+=hal
#SUBDIRS+=lib
SUBDIRS+=module
SUBDIRS+=systhread

#以下同根目录下的makefile的相同代码的解释
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
#SUBSRC=$(shell find $(SUBDIRS) -name '*.c')
#SUBOBJ=$(SUBSRC:%.c=%.o)
profile:$(SUBDIRS) $(CUR_OBJS) combine
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAGS) -o $(OBJS_DIR)/$@
ECHO:
	@echo $(SUBDIRS)
.PHONY:combine
combine:
	#$(CC) $(SUBOBJ) -o src.o
.PHONY:clean
clean:
	for dir in $(SUBDIRS); do \
		echo "enter dir:"$$dir; \
		cd $(CUR_DIR)/$$dir && make clean; \
	done
	-rm -rf $(OBJS_DIR)/*.o
