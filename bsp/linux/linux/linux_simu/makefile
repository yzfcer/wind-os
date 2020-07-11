CUR_DIR=$(shell pwd)
#Read the subdirectory of the current directory
#SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
SUBDIRS=device
SUBDIRS+=port
#Export all source and target files
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
profile:$(SUBDIRS) $(CUR_OBJS) install
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	@$(CC) -c $^ $(CFLAGS) -o $@
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

