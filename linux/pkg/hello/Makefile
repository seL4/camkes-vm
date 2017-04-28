TARGET = hello

include ../../common.mk
include ../../common_app.mk

hello: hello.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
