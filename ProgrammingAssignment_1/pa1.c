#include <stdio.h>
#include <stdlib.h>

typedef struct process
{
	char name[2];
	int burstTime, arrivalTime;
} process;

int main(int argc, char const *argv[])
{
	char fileName[] = "set1_process.in";
	int i, debug = 0;
	FILE *input;

	if (debug)
	{
		for (i = 1; i < 5; i++)
		{
			input = fopen(fileName, "r");

			if (input == NULL)
			{
				printf("file %s not found!\n", fileName);
				return -1;
			}

			char *lineBuffer = malloc(128);
			while(fgets(lineBuffer, 128, input) != NULL)
			{
				printf("%s", lineBuffer);
			}

			fclose(input);
			fileName[3] = i + '0';
		}
	}

	else
	{
		input = fopen(fileName, "r");

		if (input == NULL)
		{
			printf("file %s not found!\n", fileName);
			return -1;
		}

		char *lineBuffer = malloc(128);
		while(fgets(lineBuffer, 128, input) != NULL)
		{
			printf("%s", lineBuffer);
		}

		fclose(input);
	}

//	process *processArray = malloc(sizeof(process) * 2);
	
	return 0;
}