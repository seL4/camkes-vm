obj-m += poke.o
CFLAGS_poke.o = -I../../include -I../../../common/shared_include

all:
	make -C $(KHEAD) M=$(PWD) modules

clean:
	make -C $(KHEAD) M=$(PWD) clean
