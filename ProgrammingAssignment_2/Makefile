obj-m = driver.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

pa2:pa2.c
		cc -o pa2 pa2.c

