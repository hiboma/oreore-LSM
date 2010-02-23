CFILES = oreore.c common.c control.c

obj-m     += mod_oreore.o
mod_oreore-objs := $(CFILES:.c=.o)
KDIR	  :=	/lib/modules/$(shell uname -r)/build
PWD	  :=	$(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.o
	rm -f *.mod.c
	rm -f *.ko
	rm -f *~
	rm -f Module.symvers
