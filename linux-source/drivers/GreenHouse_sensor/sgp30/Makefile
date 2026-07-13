KERNELDIR := $(shell pwd)/../../..
CURRENT_PATH := $(shell pwd)
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

obj-m := sgp30.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

.PHONY: build kernel_modules clean
