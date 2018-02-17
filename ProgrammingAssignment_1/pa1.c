#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_MAX 2147483647

typedef struct process
{
	char *name;
	int burstTime, arrivalTime, isFinished, endTime;
} process;

void printList(process *procList, int numProc)
{
	int i;
	char outFilename[] = "processes.out";
	FILE *output;
	output = fopen(outFilename, "w");

	for(i = 0; i < numProc; i++)
		fprintf(output,"%s : %d\t", procList[i].name, procList[i].arrivalTime);

	fprintf(output,"\n");
}

process *sortTime(process *procList, int numProc)
{
	int i, j;

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

	return procList;
}

int allProcessesDone(process *procList, int numProcesses)
{
	int isFinished = 1, i = 0;

	for(i = 0; i < numProcesses; i++)
	{
		if(procList[i].isFinished == 0)
		{
			return 0;
		}
	}

	return isFinished;
}

void runSJF(process *procList, int duration, int numProcesses)
{
	int time = 0, isRunning = 0, nextArrival = 0, j = 0, i, isDone = 0, bestTime = INT_MAX;
	int processTimes[numProcesses];
	procList = sortTime(procList, numProcesses);
	process activeProc;
	char outFilename[] = "processes.out";
	FILE *output;
	output = fopen(outFilename, "w");

	// Holds arrival times, sorted already
	for (i = 0; i < numProcesses; i++)
		processTimes[i] = procList[i].arrivalTime;

	fprintf(output,"%d processes\nUsing Shortest Job First (Pre)\n\n", numProcesses);

	activeProc = procList[0];	// sorted to have earliest arrival time first
	fprintf(output,"Time %d : %s arrived\n", time, procList[j].name);
	j++;

	fprintf(output,"Time %d : %s selected (burst %d)\n", time, activeProc.name, activeProc.burstTime);

	while(time < duration)
	{
		if(activeProc.burstTime == 0)
		{
			fprintf(output,"Time %d : %s finished\n", time, activeProc.name);
			activeProc.isFinished = 1;

			for (i = 0; i < numProcesses; i++)
			{
				if(activeProc.name == procList[i].name)
					procList[i] = activeProc;
			}

			bestTime = INT_MAX;
			for(i = 0; i < numProcesses; i++)
			{
				if(procList[i].burstTime < bestTime && procList[i].isFinished != 1)
				{
					bestTime = procList[i].burstTime;
					activeProc = procList[i];
				}
			}

			if(allProcessesDone(procList, numProcesses))
				break;

			fprintf(output,"Time %d : %s selected (burst %d)\n", time, activeProc.name, activeProc.burstTime);
		}

		if(time == processTimes[j])
		{
			fprintf(output,"Time %d : %s arrived\n", time, procList[j].name);

			if(procList[j].burstTime < activeProc.burstTime)
			{	
				for (i = 0; i < numProcesses; i++)
				{
					if(activeProc.name == procList[i].name)
						procList[i] = activeProc;
				}

				activeProc = procList[j];
				fprintf(output,"Time %d : %s selected (burst %d)\n", time, activeProc.name, activeProc.burstTime);
			}

			if((j + 1) != numProcesses)
				j++;
		}

		activeProc.burstTime--;
		time++;
	}

	while (time < duration)
	{
		fprintf(output,"Time %d : IDLE\n", time);
		time++;
	}

	fprintf(output,"Finished at time %d\n", time);
}


void runFCFS(process *procList, int duration, int numProcesses)
{
	int time = 0, isRunning = 0, nextArrival = 0, i = 0, j = 0;
	int processTimes[numProcesses];
	procList = sortTime(procList, numProcesses);
	process activeProc;
	char outFilename[] = "processes.out";
	FILE *output;
	output = fopen(outFilename, "w");

	for (i = 0; i < numProcesses; i++)
		processTimes[i] = procList[i].arrivalTime;

	fprintf(output,"%d processes\nUsing First Come First Served\n\n", numProcesses);

	while(!isRunning)
	{
		for(i = 0; i < numProcesses; i++)
		{
			activeProc = procList[i];

			if(i == 0)
			{
				fprintf(output,"Time %d : %s arrived\n", time, procList[j].name);
				j++;
			}

			fprintf(output,"Time %d : %s selected (burst %d)\n", time, activeProc.name, activeProc.burstTime);

			while(activeProc.burstTime != 0)
			{
				if(time == processTimes[j])
				{
					fprintf(output,"Time %d : %s arrived\n", time, procList[j].name);
					
					if((j + 1) != numProcesses)
						j++;
				}

				activeProc.burstTime--;
				time++;
			}

			fprintf(output,"Time %d : %s finished\n", time, activeProc.name);
		}

		isRunning = 1;
		fprintf(output,"Finished at time %d\n", time);
		activeProc.endTime = time;

	}

}

int processesArrived(process *procList, int numProcesses, int time)
{
	int i = 0, arrivalCount = 0;

	for(i = 0; i < numProcesses; i++)
	{
		if(procList[i].arrivalTime <= time)
			arrivalCount++;
	}

	return arrivalCount;
}

void runRR(process *procList, int duration, int numProcesses, int timeQ)
{
	int time = 0, isRunning = 0, nextArrival = 0, i = 0, j = 0, k = 0, isDone = 0, lastRun, procArrived, isFinished = 0;
	int processTimes[numProcesses];
	procList = sortTime(procList, numProcesses);
	process activeProc;
	char outFilename[] = "processes.out";
	FILE *output;
	output = fopen(outFilename, "w");
	for (i = 0; i < numProcesses; i++)
		processTimes[i] = procList[i].arrivalTime;

	fprintf(output,"%d processes\nUsing Round-Robin\nQuantum %d\n", numProcesses, timeQ);
	activeProc = procList[0];

	fprintf(output,"Time %d: %s arrived\n", time, procList[j].name);
	j++;

	while(time < duration && !isFinished)
	{	
		// Go for 2 cycles
		for(i = 0; i < timeQ; i++)
		{
			if(activeProc.burstTime == 0 && activeProc.isFinished == 1)
			{
				isFinished = 1;
				break;	
			}

			if (i == 0)
				fprintf(output,"Time %d: %s selected (burst %d)\n", time, activeProc.name, activeProc.burstTime);

			if(time == processTimes[j])
			{
				fprintf(output,"Time %d: %s arrived\n", time, procList[j].name);
				
				if((j + 1) != numProcesses)
					j++;
			}

		//	fprintf(output,"%d\n", time);

			// If not finished, decrement
			if(activeProc.burstTime > 0)
			{
				activeProc.burstTime--;
			}

			// If finished, swap out
			else if (activeProc.burstTime == 0)
			{
				activeProc.isFinished = 1;
				fprintf(output,"Time %d: %s finished\n", time, activeProc.name);

				// Update procList
				for (k = 0; k < numProcesses; k++)
				{
					if(activeProc.name == procList[k].name)
					{
						procList[k] = activeProc;
						lastRun = k;
					}
				}

				// Get the next one
				for(k = 0; k < numProcesses; k++)
				{
					procArrived = processesArrived(procList, numProcesses, time);

					if(procList[k].burstTime != 0 && (procArrived == 1))
					{
						activeProc = procList[k];
						break;
					}

					else if(procList[k].burstTime != 0 && k != lastRun)
					{
						activeProc = procList[k];
						break;
					}

					else if (k == numProcesses - 1)
					{
						k = 0;
						isDone++;

						if(isDone > 1)
						{
							isDone = 0;
							break;
						}
					}
				}

				break;
			}

			time++;
		}

		if(time == processTimes[j])
		{
			fprintf(output,"Time %d: %s arrived\n", time, procList[j].name);
			
			if((j + 1) != numProcesses)
				j++;
		}

		// Update procList
		for (k = 0; k < numProcesses; k++)
		{
			if(activeProc.name == procList[k].name)
			{
				procList[k] = activeProc;
				lastRun = k;
			}
		}

		// Get the next one
		for(k = 0; k < numProcesses; k++)
		{
			procArrived = processesArrived(procList, numProcesses, time);

			if(procList[k].burstTime != 0 && (procArrived == 1))
			{
				activeProc = procList[k];
				break;
			}

			else if(procList[k].burstTime != 0 && k != lastRun)
			{
				activeProc = procList[k];
				break;
			}

			else if (k == numProcesses - 1)
			{
				k = 0;
				isDone++;

				if(isDone > 1)
				{
					isDone = 0;
					break;
				}
			}
		}
	}

	while (time < duration)
	{
		fprintf(output,"Time %d: IDLE\n", time);
		time++;
	}

	fprintf(output,"Finished at time %d\n", time);
}

int main(int argc, char const *argv[])
{
	char inFilename[] = "set2_process.in";
	char outFilename[] = "processes.out";
	int i = 0, j = 0, h = 0, debug = 0, linenumber = 1, x = 0;
	FILE *input, *output;

	// Process variables
	int processCount, runfor, quantum;
	char use[5]; //Holds the algorithm type
	char temp[10]; //Temp array to convert the input characters to integers 
	char *chtemp = malloc(sizeof(char) * 2);
	char lineBuffer[1024]; 
	int processTimes[1024], burstTimes[1024];

	input = fopen(inFilename, "r");

	// TO DO: Do output file stuff in the Algorithm functions where the print statments are

	if (input == NULL)
	{
		fprintf(output,"file %s not found!\n", inFilename);
		return -1;
	}

	while(fgets(lineBuffer, 128, input) != NULL)
	{	// Parse Through each line
		// fprintf(output,"Line number = %d: %s", linenumber,lineBuffer);
		// Clear temporary values
		j = 0;
		x = 0;
		memset(&temp[0], 0, sizeof(temp));
		// Read until comment
		if(linenumber <= 4){ 
			while(lineBuffer[j] != '#') {
				//Process count always on first line
				if((lineBuffer[j] == ' ') && linenumber == 1) {
					while(lineBuffer[j + 1] != ' ') {
						// Until next space grab the number
						temp[x] = lineBuffer[j + 1];
						x++;
						j++;
					}
					// Convert char array of numbers to an integer
					sscanf(temp, "%d", &processCount);
					//fprintf(output,"%d Processes\n", processCount);
					break;
				}
				// time cycles on line 2
				if((lineBuffer[j] == ' ') && linenumber == 2) {
					while(lineBuffer[j + 1] != ' ') {
						// Until next space grab the number
						temp[x] = lineBuffer[j + 1];
						x++;
						j++;
					}
					sscanf(temp, "%d", &runfor);
					break;
				}
				if((lineBuffer[j] == ' ') && linenumber == 3) {
					while(lineBuffer[j + 1] != ' ') {
						// Until next space grab the number
						use[x] = lineBuffer[j + 1];
						x++;
						j++;
					}					
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
					//fprintf(output,"Quantum = %d\n", quantum);	
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
					sscanf(temp, "%d", &processTimes[i]);
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
					h++;
				}	
				j++;		
			}
		}

		linenumber++;
	}

	fclose(input);

	process *procList = malloc(sizeof(process) * processCount);

	//Populate the Processes Array
	chtemp[0] = 'P';
	for(i = 0; i < processCount; i++){
		procList[i].arrivalTime = processTimes[i];
		procList[i].burstTime = burstTimes[i];
		procList[i].name = malloc(sizeof(char) * 2);
		procList[i].isFinished = 0;
		chtemp[1] = (i+1) + '0';
		strcpy(procList[i].name, chtemp);
	}

	// Send to Scheduling Algorithms 
	// If First Come First Serve
	if(use[0] == 'f') {
		runFCFS(procList, runfor, processCount);
	}
	else if(use[0] == 'r'){
		runRR(procList, runfor, processCount, quantum);
	}
	else if(use[0] == 's'){
		runSJF(procList, runfor, processCount);
	}
	
	return 0;
}