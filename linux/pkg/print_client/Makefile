TARGET = print_client

include ../../common.mk
include ../../common_app.mk

print_client: print_client.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lcamkes -o $@
