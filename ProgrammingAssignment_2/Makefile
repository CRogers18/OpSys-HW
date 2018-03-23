obj-m = driver.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		gcc -o pa2 pa2.c

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
		rm pa2


