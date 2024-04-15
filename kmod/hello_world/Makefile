obj-m += hello.o

KERNEL_DIR := $(PWD)/../../bb-kernel/KERNEL
CROSS_COMPILE := $(PWD)/../../bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

CORES=4
PUBLIC_DRIVER_DIR=$(PWD)/../publicDriver

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) -j$(CORES) modules
	# copy result to public folder
	mkdir -p $(PUBLIC_DRIVER_DIR)
	cp *.ko $(PUBLIC_DRIVER_DIR)

clear:
	make -C $(KERNEL_DIR) M=$(PWD) clean