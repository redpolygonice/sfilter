obj-m := sfilter.o
nfilter-y := nfilter.o filter.o slist.o nlist.o command.o
ccflags-y := -I $(PWD)/

KDIR ?= /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f Kbuild
