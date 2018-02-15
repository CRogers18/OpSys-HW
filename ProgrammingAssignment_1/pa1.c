#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_MAX 2147483647

typedef struct process
{
	char *name;
	int burstTime, arrivalTime;
} process;

void printList(process *procList, int numProc)
{
	int i;

	for(i = 0; i < numProc; i++)
		printf("%s : %d\t", procList[i].name, procList[i].arrivalTime);

	printf("\n");
}

process *sortProcessList(process *procList, int numProc)
{
	int i, j;

//	printf("Before sorting...\n");
//	printList(procList, numProc);

	for(i = 0; i < numProc; i++)
	{
		for(j = i+1; j < numProc; j++)
		{
			if(procList[i].arrivalTime > procList[j].arrivalTime)
			{
				process temp = procList[i];
				procList[i] = procList[j];
				procList[j] = temp;
			}
		}
	}

//	printf("After sorting...\n");
//	printList(procList, numProc);

	return procList;
}

void runFCFS(process *procList, int duration, int numProcesses)
{
	int time = 0, isDone = 0, nextArrival = 0, i = 0, j = 0;
	int arrivalTimes[numProcesses];
	procList = sortProcessList(procList, numProcesses);
	process activeProc;

	for (i = 0; i < numProcesses; i++)
		arrivalTimes[i] = procList[i].arrivalTime;

	printf("\n\n%d processes\nUsing First Come First Served\n", numProcesses);

	while(!isDone)
	{
		for(i = 0; i < numProcesses; i++)
		{
			activeProc = procList[i];

			if(i == 0)
			{
				printf("Time %d : %s arrived\n", time, procList[j].name);
				j++;
			}

			printf("Time %d : %s selected (burst %d)\n", time, activeProc.name, activeProc.burstTime);

			while(activeProc.burstTime != 0)
			{
				if(time == arrivalTimes[j])
				{
					printf("Time %d : %s arrived\n", time, procList[j].name);
					
					if((j + 1) != numProcesses)
						j++;
				}

				activeProc.burstTime--;
				time++;
			}

			printf("Time %d : %s finished\n", time, activeProc.name);
		}

		isDone = 1;
		printf("Finished at time %d\n", time);
	}

}

int main(int argc, char const *argv[])
{
	char fileName[] = "set1_process.in";
	int i, runAll = 0;
	FILE *input;

	if (runAll)
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

	// Some temporary processes for testing
	process *procList = malloc(sizeof(process) * 3);
	
	procList[0].arrivalTime = 0;
	procList[0].burstTime = 24;
	procList[0].name = malloc(sizeof(char) * 2);
	strcpy(procList[0].name, "P1");
	
	procList[1].arrivalTime = 3;
	procList[1].burstTime = 3;
	procList[1].name = malloc(sizeof(char) * 2);
	strcpy(procList[1].name, "P2");

	procList[2].arrivalTime = 4;
	procList[2].burstTime = 3;
	procList[2].name = malloc(sizeof(char) * 2);
	strcpy(procList[2].name, "P3");

	runFCFS(procList, 30, 3);
	
	return 0;
}