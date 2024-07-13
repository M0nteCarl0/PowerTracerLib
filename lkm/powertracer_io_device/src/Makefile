obj-m += riscurechardev.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

all: default

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
