/* 	Name: Ulysses Chaparro
	ID:   1001718774 	   */

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2021 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include<ctype.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports ten arguments

//function for printing the history array, which contains the last 15
//command entries, function accepts history array and index as 
//parameters
void printHistory(char* history[], int index)
{
	int i = 0;  
	for(i = 0; i < index; i++)
	{
		printf("%d: %s", i, history[i]);
	}
}

//very similar function to printHistory where the PIDs are printed, 
//parameters passed in are the pid array and the index 
void printPids(int pid_array[], int index)
{
	int i = 0;  
	if(index == 0) //if true, this means that no process has been
				   //spawned by shell yet, so nothing to print 
	{
		printf("No processes have been spawned yet...\n"); 
	}
	else 
	{
		for(i = 0; i < index; i++)
		{
			printf("%d: %d\n", i, pid_array[i]); 
			
		}
	}
}

//function to determine if input line is full of blanks, returns 1
//if so, 0 if not, takes the cmd_str as a parameter
int blankLine(char *line)
{
	char *temp = line; //to not alter cmd_str 
	int i = 0; 
	for (i = 0; temp[i] != '\0'; i++) //iterate over each character
	{
		if (!isspace(temp[i])) //checks if character is a space 
		{
		  return 0; //if not a space, line is not full of space, return 0
		}
	}
	return 1; //line is full of spaces, return 1
}

int main()
{

  char *cmd_str = (char*)malloc(MAX_COMMAND_SIZE);
  
  //creating a history array which will contain the last 15 command entries 
  char *history[15];
  int history_index = 0;
  
  //creating a pid array to store the last 15 process pids
  int pid_array[15]; 
  int pid_index = 0;
  
  //initializing a size of the history array which means how many strings there
  //are in the array, aka how many commands, this will be useful for the !n command
  int size_history = 0; 
  
  //this is useful for !n command where the input is checked for validation
  int current_digit = 0;
  char digit[2]; 
  
  //memory allocating each char* entry of history to 255 since that's the
  //maximum command-line size allowed anyway
  int i = 0; 
  for(i = 0; i < 15; i++)
  {
	history[i] = (char*) malloc(sizeof(char)*MAX_COMMAND_SIZE);
  } 
  
  while(1)
  {
    	// Print out the msh prompt
    	printf("msh> ");

	//clear out the input string for minimal errors
	memset(cmd_str, 0, strlen(cmd_str));
	
   	// Read the command from the commandline.  The
    	// maximum command that will be read is MAX_COMMAND_SIZE
    	// This while command will wait here until the user
    	// inputs something since fgets returns NULL when there
    	// is no input
    	while(!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));
	
	//save cmd_str into history array
	strncpy(history[history_index], cmd_str, MAX_COMMAND_SIZE);
	if(history_index >= 14)
	{
		history_index = 0; //restarts the index of the history array since
						   //array can only hold last 15 entries
	}
	else
	{
		history_index++;
	}
	
	//reading the first character will
	//tell us if input is '!', if so, command is !n 
	if(cmd_str[0] == '!')
	{
		//size_history is the number of strings, or commands, in history 
		size_history = sizeof(history)/sizeof(history[0]); 
		
		//check if character following ! is a digit, if not then print out 
		//the input and ": Command not found." and return to top of while loop 
		if(!isdigit(cmd_str[1]))
		{
			printf("%s: Command not found.\n", cmd_str);
			continue;
		}
		
		current_digit = cmd_str[1] - '0'; //converts char to int 
		
		//check if digit entered is greater than or equal to number of strings,
		//if so, then print "command not in history" because digit is not within
		//the options to choose from history 
		if(current_digit >= size_history)
		{
			printf("Command not in history.\n"); 
			continue;
		}
		
		if(isdigit(cmd_str[2])) //check if the third character is a digit
		{
			//if there is a second digit but the first digit is not 1, then
			//automatically know that is an invalid index 
			if(current_digit != 1)
			{
				printf("Command not in history.\n");
				continue;
			}
			current_digit = cmd_str[2] - '0'; //changing current_digit to second digit
			
			//check if the second digit is below 5, so that it is sure that the 
			//index is from 0 to 14 
			if(current_digit >= 5)
			{
				printf("Command not in history.\n");
				continue;
			}
			
			//concatenate the characters representing the digits
			digit[0] = cmd_str[1];
			digit[1] = cmd_str[2];
			current_digit = atoi(digit); //current_digit is now n in !n
		}
		  
		//n refers to the index of history[] to access and 
		//you copy the contents to cmd_str, overriding in a way, so that the input 
		//can be parsed correctly now and the program can continue 
		strncpy(cmd_str, history[current_digit], MAX_COMMAND_SIZE); 
	}
	
	//if input is either exit or quit, quit the shell
	if(strcmp(cmd_str, "exit")==0 
	|| strcmp(cmd_str, "exit\n")==0)
	{ 
		return 0; 
	}
	
	if(strcmp(cmd_str, "quit")==0 
	|| strcmp(cmd_str, "quit\n")==0)
	{ 
		return 0; 
	}
	
	//check if input is history, show history before even parsing input
	if(strcmp(cmd_str, "history") == 0 
	|| strcmp(cmd_str, "history\n") == 0)
	{
		printHistory(history, history_index);
		continue;
	}
	
	//check if input is showpids, show pids before even parsing input
	if(strcmp(cmd_str, "showpids") == 0
	|| strcmp(cmd_str, "showpids\n") == 0)
	{
		printPids(pid_array, pid_index); 
		continue;
	}
	
	//check if input is a blank line, if so, return to top of while loop
	//by continue
	if(blankLine(cmd_str))
	{
		continue; 
	}	
		
    	/* Parse input */
    	char *token[MAX_NUM_ARGUMENTS+1]; //+1 to take command into account

    	int token_count = 0;                                 
                                                           
    	// Pointer to point to the token
    	// parsed by strsep
    	char *argument_ptr;                                         
                                                           
    	char *working_str  = strdup(cmd_str);                

    	// we are going to move the working_str pointer so
    	// keep track of its original value so we can deallocate
    	// the correct amount at the end
    	char *working_root = working_str;

    	// Tokenize the input strings with whitespace used as the delimiter
    	while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS+1))
    	{
      		token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      		if(strlen(token[token_count]) == 0)
      		{
        		token[token_count] = NULL;
      		}
        	token_count++;
    	}

	//basically, token[0] is going to have the command and everything after that
	//is arguments
	
	//if command is cd, run built in chdir() to change directories to
	//token[1] aka the directory to change to 
	if(strcmp(token[0], "cd") == 0)
	{
		int ret = chdir(token[1]); 
		if(ret == -1)
		{
			printf("Directory not found.\n");
			continue; 
		}
	}		
	
	//else the command is in bin or somewhere and the child process has to
	//take over and call exec to execute the command 
	else 
	{
		pid_t pid = fork(); //creates a new process and returns either a 0 or a 
							 //pid that is non-zero 
							 
		pid_array[pid_index++] = pid; //storing the pid of the process created by fork 
		if(pid_index > 14)
		{
			pid_index = 0; //since only last 15 pids can be stored, reset pid_index 
						   //after 14 
		}

		if( pid == 0 ) //if true, we are in the child process
		{
			//execvp is used to execute the command and essentially replace the 
			//current process with a new process, ret is used to evaluate if
			//execvp returned successfully or not
			int ret = execvp(token[0], &token[0]); 
		  
			if(ret == -1)
			{
			  printf("%s command not found.\n", token[0]);
			}
		}
		  else //we are in the parent process and must wait for child to end 
		{
			int status;
			wait(&status);
		}
	}
	free(working_root);
  }
  return 0;
}

