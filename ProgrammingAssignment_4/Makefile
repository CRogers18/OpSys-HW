obj-m += CharBufferInput.o 
obj-m += CharBufferOutput.o
#obj-m = NSACharBuffer.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
		


