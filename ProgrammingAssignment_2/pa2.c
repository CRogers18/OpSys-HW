/*
This will test the Character Driver 
Once the kernel module is installed this file can be used like:
> ./pa2 "Input String to be written to the Character Driver"
Will output the same string by writing it to the Driver and Reading it back.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	int driver, i = 0;
	char chbuffer[BUFFER_SIZE];
	int number_of_bytes_to_read = BUFFER_SIZE;

	driver = open("/dev/driver", 0_RDWR); 
	
	if(driver < 0){
		printf("Driver not able to be opened");
		return 0;
	}

	printf("Input String: %s\n", argv[1]);

	i = write(driver, argv[1], strlen(argv[1]));

	if(i < 0){
		printf("Driver not written to properly");
		return 0;
	}

	printf("Press ENTER to read back from the device...\n");
   	getchar();
	
	printf("Reading from device...\n");
	
	i = read(driver, chbuffer, number_of_bytes_to_read);

	if(i < 0){
		printf("Driver not read properly.");
		return 0;
	}

	printf("The recived message is: %s\n", chbuffer);	

	return 0;
}
