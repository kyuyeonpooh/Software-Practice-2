//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #3
// October 11, 2017
//
// 2016314827 Kim Kyu Yeon
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "cmd.h"
#include "string_sw.h"
#include "macro.h"


char *pwd (int full){

	static char buffer[1024];
	char *ptr;
	
	if (getcwd(buffer, 1024) != buffer)
		return NULL;

	if (full)
		return buffer;

	if (strcmp("/", buffer) == 0)
		return buffer;

	ptr = strrchr(buffer, '/');
	return ptr+1;

}

void print_prompt(){

	int ret;
	char *ptr = pwd(0);
	char head[200];

	if (!ptr)
		exit(1);
	if (strlen(ptr) > 190)
		exit(1);

	strcpy(head, "swsh:");
	strcat(head, ptr);
	strcat(head, "> ");

	ret = write(2, head, strlen(head));
	
	if (ret <= 0)
		exit(1);

}

void child_handler(int sig){
	
	int pid = getpid();
	int stat;

	waitpid(-1, &stat, WNOHANG | WUNTRACED);

	if(WIFSTOPPED(stat) == TRUE)
		killpg(pid, SIGKILL);

}

void stop_handler(int sig){

	raise(SIGCHLD);

}

int main(void){

	int ret;
	char input[240];

	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD, (void*) child_handler);
	signal(SIGTSTP, (void*) stop_handler);

	while (1) {
		print_prompt();

		ret = read(0, input, 200);
		if (ret < 0)
			exit(1);
		else if (ret == 0) {
			write(2, "exit\n", 5);
			break;
		}
		
		cmdchk(input);
		
	}

	return 0;

}
