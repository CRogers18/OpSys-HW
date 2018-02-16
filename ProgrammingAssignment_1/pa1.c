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

	printf("%d processes\nUsing First Come First Served\n\n", numProcesses);

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

//sort the process list according to the burst length
process *sortlistbyburst (process *procList, int numProc)
{
    int i = 0;
    int j = 0;

    procList = sortProcessList(procList, numProcesses);

    //bubble sort the list by burst time except the first occurring process
    for(i = 1; i < numProc; i++)
	{
		for(j = i+1; j < numProc; j++)
		{
			if(procList[i].burstTime > procList[j].burstTime)
			{
				process temp = procList[i];
				procList[i] = procList[j];
				procList[j] = temp;
			}
		}
	}

	return procList;
}

//implementing the preemptive shortest job first
void preemshortestjobfirst (process *procList, int duration, int numProcesses)
{
    int time = 0;
    int isDone = 0;
    int i = 0;
    int j = 1;

    process *arrivallist = sortProcessList(procList, numProcesses);

    procList = sortlistbyburst(procList, numProcesses);

    process currproc = procList[i];

    while(!isDone)
    {
        if(time == duration)
        {
            isDone = 1;
            printf("Finished at time %d\n", time);
            continue;
        }

        if(time == arivallist[i].arrivalTime)
        {
            printf("Time %d: %s arrived\n", time, currproc.name);

            if(i == 0)
            {
                printf("Time %d: %s selected (burst %d)\n", time, currproc.name, currproc.burstTime);
            }

            i++;
        }

        if(arivallist[i].arivalTime == procList[j].arivalTime)
        {
            printf("Time %d: %s selected (burst %d)\n", time, currproc.name, currproc.burstTime);
        }

        if(currproc.burstTime == 0)
        {
            printf("Time %d: %s finished\n", time, currproc.name);

            if(j > 0 && procList[j-1].burstTime != 0)
            {
                j--;
                currproc = procList[j];
            }
            else
            {
                j++;
                currproc = procList[j];
            }
        }

        if(i > 0)
        {
            currproc.burstTime--;
        }

        time++;
    }
}

int main(int argc, char const *argv[])
{
	char inFilename[] = "set1_process.in";
	char outFilename[] = "processes.out";
	int i = 0, j = 0, h = 0, debug = 0, linenumber = 1, x = 0;
	FILE *input, *output;
	// Process variables
	int processCount, runfor, quantum; 
	char use[5]; //Holds the algorithm type
	char temp[10]; //Temp array to convert the input characters to integers 
	char chtemp[2];
	char lineBuffer[1024]; 
	int arrivalTimes[1024], burstTimes[1024];

	/*if (debug)
	{
		for (i = 1; i < 5; i++)
		{
			input = fopen(inFilename, "r");

			if (input == NULL)
			{
				printf("file %s not found!\n", inFilename);
				return -1;
			}

			char *lineBuffer = malloc(128);
			while(fgets(lineBuffer, 128, input) != NULL)
			{
				printf("%s", lineBuffer);
			}

			fclose(input);
			inFilename[3] = i + '0';
		}
	}*/

/*	else
	{*/
		input = fopen(inFilename, "r");
		// TO DO: Do output file stuff in the Algorithm functions where the print statments are
//		output = fopen(outFilename, "w");

		if (input == NULL)
		{
			printf("file %s not found!\n", inFilename);
			return -1;
		}

		
		while(fgets(lineBuffer, 128, input) != NULL)
		{	// Parse Through each line
			// printf("Line number = %d: %s", linenumber,lineBuffer);
			// Clear temporary values
			j = 0;
			x = 0;
			memset(&temp[0], 0, sizeof(temp));
			// Read until comment
			if(linenumber <= 4){ 
				while(lineBuffer[j] != '#'){
					//Process count always on first line
					if((lineBuffer[j] == ' ') && linenumber == 1){
						while(lineBuffer[j + 1] != ' '){
							// Until next space grab the number
							temp[x] = lineBuffer[j + 1];
							x++;
							j++;
						}
						// Convert char array of numbers to an integer
						sscanf(temp, "%d", &processCount);
						//printf("%d Processes\n", processCount);
						break;
					}
					// time cycles on line 2
					if((lineBuffer[j] == ' ') && linenumber == 2){
						while(lineBuffer[j + 1] != ' '){
							// Until next space grab the number
							temp[x] = lineBuffer[j + 1];
							x++;
							j++;
						}
						sscanf(temp, "%d", &runfor);
						//printf("Run for %d time cycles\n", runfor);	
						break;
					}
					if((lineBuffer[j] == ' ') && linenumber == 3){
						while(lineBuffer[j + 1] != ' '){
							// Until next space grab the number
							use[x] = lineBuffer[j + 1];
							x++;
							j++;
						}
					
						//printf("Using %s\n", use);	
						break;
					}
					// line four can either have the quantum or a process input
					if(linenumber == 4 && lineBuffer[j] == ' '){
						while(lineBuffer[j + 1] != ' '){
						// Until next space grab the number
						temp[x] = lineBuffer[j + 1];
						x++;
						j++;
					}
						sscanf(temp, "%d", &quantum);
						//printf("Quantum = %d\n", quantum);	
						break;
					}
					

					j++;
				}
		}
		// Parse the line buffer for Process information
		else{			
			while(lineBuffer[j] != '\n'){
				x = 0;
				memset(&temp[0], 0, sizeof(temp));
				//Grab the arrival number after the word arriva'l'
				if(lineBuffer[j] == 'l'){
					j = j + 1;
					while(lineBuffer[j + 1] != ' '){
						// Until next space grab the number
						temp[x] = lineBuffer[j + 1];
						x++;
						j++;
					}
					sscanf(temp, "%d", &arrivalTimes[i]);
					//printf("For process %d arrival time is %d\n", i + 1, arrivalTimes[i]);
					i++;
				}
				// Grab the burst number after the word burs't'
				if(lineBuffer[j] == 't'){
					j = j + 1;
					while(lineBuffer[j + 1] != ' '){
						// Until next space grab the number
						temp[x] = lineBuffer[j + 1];
						x++;
						j++;
					}
					sscanf(temp, "%d", &burstTimes[h]);
					//printf("For process %d burst length is %d\n", h + 1, burstTimes[h]);
					h++;
				}	
				j++;		
			}
		}

		linenumber++;
	}

		fclose(input);
	/*}*/

	process *procList = malloc(sizeof(process) * processCount);

	//Populate the Processes Array

	chtemp[0] = 'P';
	for(i = 0; i < processCount; i++){
		procList[i].arrivalTime = arrivalTimes[i];
		procList[i].burstTime = burstTimes[i];
		procList[i].name = malloc(sizeof(char) * 2);
		chtemp[1] = (i+1) + '0';
		strcpy(procList[i].name, chtemp);
		//printf("%s\n",procList[i].name);
	}

	// Send to Scheduling Algorithms 
	// If First Come First Serve
	if(use[0] == 'f'){

		runFCFS(procList, runfor, processCount);
	}
	else if(use[0] == 'r'){
		// TO DO: write round robin
		printf("use Round Robin\n");
	}
	else if(use[0] == 's'){
		// TO DO: write shortest job first
		printf("use Shortest Job First\n");
	}

/*	procList[0].arrivalTime = 0;
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

	runFCFS(procList, 30, 3);*/
	
	return 0;
}
