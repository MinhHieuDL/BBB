# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its variables.
# $(info This message will be printed once)
ifneq (${KERNELRELEASE},)
	obj-m += src/main.o

# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
	KERNEL_DIR := $(PWD)/../../bb-kernel/KERNEL
	CC := $(PWD)/../../bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

	CORES=4
	PUBLIC_DRIVER_DIR=$(PWD)/../publicDriver
	LDDINC := $(PWD)/inc

default:
	${MAKE} ARCH=arm CROSS_COMPILE=${CC} -C ${KERNEL_DIR} M=${PWD} -j${CORE} EXTRA_CFLAGS="-std=gnu99 -I${LDDINC}" modules
	# copy result to public folder
	mkdir -p ${PUBLIC_DRIVER_DIR}
	cp src/*.ko ${PUBLIC_DRIVER_DIR}/scull.ko

all: default

clear:
	${MAKE} -C ${KERNEL_DIR} M=${PWD} clean
endif