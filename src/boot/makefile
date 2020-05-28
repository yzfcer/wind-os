#Read the subdirectory of the current directory
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
#Export all source and target files
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
profile:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ -o $(CFLAGS) $(ROOT_DIR)/$(OBJS_DIR)/$@
ECHO:
	@echo $(SUBDIRS)