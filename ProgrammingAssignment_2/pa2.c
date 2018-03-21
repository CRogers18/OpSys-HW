#include <stdio.h>

#define BUFFER_SIZE 1024;
static char inputBuffer[BUFFER_SIZE];

void readAndRemove (char inputBuffer);

void readAndRemove (char inputBuffer)
{
	int i = 0;
	
	//empty input condition
	if(inputBuffer[0] == NULL)
	{
		printf("No Input Found!\n");
	}
	
	//until it hits a null terminator in the string, it prints out each character and turns it into a null terminator
	while(inputBuffer[i] != NULL)
	{
		printf("%c", inputBuffer[i]);
		
		inputBuffer[i] == NULL;
		
		i++;
	}
}

int main()
{
	inputBuffer = {NULL};
	
	//input goes into intputBuffer here
	
	//if the user wants this to happen
	readAndRemove(inputBuffer);
	
	printf("Yo\n");

	return 0;
}
