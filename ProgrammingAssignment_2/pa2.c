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

#define BUFFER_SIZE 1024;
static char inputBuffer[BUFFER_SIZE];
static char receiveBuffer[BUFFER_SIZE]; 


int main(int argc, char *argv[])
{
	int driver, returnMessage, i;
	char characterbuffer[2000];
	//if the user wants this to happen
	//readAndRemove(inputBuffer);
	driver = open("/dev/driver", 0_RDWR); 
	if(driver < 0){
		printf("Driver not able to be opened");
		return 0;
	}

	printf("Input String: %s\n", argv[1]);

	if(strlen(argv[1]) <= BUFFER_SIZE){
		write(driver, argv[1], strlen(argv[1]));
	}
	else{
		strcpy(characterbuffer, argv[1]);
		for(i = 0; i <= BUFFER_SIZE; i++){
			inputBuffer[i] = characterbuffer[i];
		}

		write(driver, inputBuffer, BUFFER_SIZE);
	}

	printf("Press ENTER to read back from the device...\n");
   	getchar();

	printf("Reading from device...\n");
	returnMessage = read(driver, receiveBuffer, BUFFER_SIZE);
	if(returnMessage < 0){
		printf("Unable to read Message from device\n");
		return 0;
	}

	printf("The recived message is: %s\n", receiveBuffer);	

	return 0;
}
