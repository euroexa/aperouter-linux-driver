#List of tunables 
DEBUG=n
PTRACE=n
REGTRACE=n

ifeq ($(src),)
	src := $$PWD
endif


ifeq ($(strip $(DEBUG)), y)
	EXTRA_CFLAGS += -DKARMA_DEBUG -DDEBUG
endif

ifeq ($(strip $(PTRACE)), y)
	EXTRA_CFLAGS += -DKARMA_PTRACE
endif

ifeq ($(strip $(REGTRACE)), y)
	EXTRA_CFLAGS += -DKARMA_REGTRACE
endif

# ccflags-y += $(DEBFLAGS) -I$(LDDINCDIR) -Wall 


ifneq ($(KERNELRELEASE),)
	# call from kernel build system

obj-m	+= karmadrv.o 
karmadrv-objs += karma.o karma_instr.o 

# EXTRA_CFLAGS += -I$(XILINX_INC)
KBUILD_EXTRA_SYMBOLS := Module.symvers

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) -Wall modules

endif



clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) -Wall clean
# 	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *ko.unsigned modules.order Module.symvers

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
