obj-m := sfilter.o
nfilter-y := nfilter.o filter.o container.o command.o data.o
ccflags-y := -I $(PWD)/

KDIR ?= /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f Kbuild
