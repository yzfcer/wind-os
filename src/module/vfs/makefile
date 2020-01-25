CUR_DIR=$(shell pwd)
#子目录的Makefile直接读取其子目录就行
#SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
SUBDIRS=fscmd hostfs listfs treefs
#以下同根目录下的makefile的相同代码的解释
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
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
