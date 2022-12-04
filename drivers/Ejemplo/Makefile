obj-m = ejemplocd...o
KVERSION = 5.15.76-v7+
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean