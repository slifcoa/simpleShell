/***********************************************************
 * * Lab 2 Programming Assignment
 * * January 24th 2018
 * * Done By Michael Ames & Adam Slifco
 * ********************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/resource.h>
#include<stdint.h>
#include<time.h>

#define MAX 50

int main(int argc, char* argv[]){
	
	//Declare process identifier variables
	pid_t pid, child;
	int status;
	
	//Variables for tracking CPU time used
	clock_t start, end;
	double cpu_time_used;

	//Placeholder variable for Involuntary Context Switches
	long lastIVC;

	//Tracks use of context switches
	struct rusage ru;
	
	//Variables to handle user input
	char* input = malloc(MAX * sizeof(char));
	char* tokens[MAX];

	while(1){

		int i = 0;

		//Prompt user to enter command
		printf("Enter A Command (type quit to end): ");

		//reads entire line
		fgets(input, MAX, stdin);
	
		//This line prevents segmentation faults on blank inputs	
		input[strlen(input) -1] = '\0';
	
		//Initializes the tokenizer
		tokens[0] = strtok(input, " \n");

		//handles blank lines...Prompt's user to enter command again
		if(input[0] != '\0'){
	
			//parses through the user input 
			while(tokens[i] != NULL){
			i++;
			tokens[i] = strtok(NULL, " \n"); 
						}
		
			//Ends program if quit command is inputted
			if(strcmp(tokens[0] , "quit") == 0){
			break;
			}
	
			printf("Processing...\n");

			//Begin clocking CPU time
			start = clock();
		
			//Call before forking to get Previous total IVC of Child's
			getrusage(RUSAGE_CHILDREN, &ru);
			lastIVC = ru.ru_nivcsw;
	
			//Spawn a child process
			pid = fork();
	
			//If child process failed to spawn
			if(pid < 0)
			{
			perror("Fork Failed");
			exit(1);		
			}

			//Child executes command
			else if(pid == 0)
			{
			execvp(tokens[0],tokens);
			exit(0);
			}

			//Parent waits for Child process to execute command
			else
			{
			child = wait(&status);
			end = clock();
			}
		
			//Find total time cpu was used by child and convert to nanoseconds
			cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
			cpu_time_used = cpu_time_used * 1000000000;
		
			//Get usage info of Child process
			getrusage(RUSAGE_CHILDREN, &ru);
		
			//Print out the amount of ICS's and CPU time used
			printf("Child %ld: INVOLUNTARY CONTEXT SWITCHES: %ld \n", (long) child, ru.ru_nivcsw - lastIVC);
			printf("Child %ld CPU time used: %lf nanoseconds\n",(long) child, cpu_time_used);	
		}
	}	 
		
	printf("Ending Program... \n");
	return 0;
}
