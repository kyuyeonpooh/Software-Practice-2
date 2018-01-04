#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cmd.h"
#include "string_sw.h"
#include "macro.h"

void cmd_type1(int cmdtype, int eval, char* args[], char* filein, char* fileout, int* pipe2, int pos){
	
	int fin = -1;
	int fout = -1;
	int pid_p = getpid(); // process id of parent
	int pid_c; // process id of child
	int stat;
	int fdsrc = -1; // file descriptor of previous pipe
	
	/* check validation */
	if(eval == NO_RDIR && (strlen(filein) != 0 || strlen(fileout) != 0)){
		writeInvalid();
		return;
	}
	else if(eval == LEFT_RDIR && (strlen(filein) == 0 || strlen(fileout) != 0)){
		writeInvalid();
		return;
	}
	else if(eval == RIGHT_RDIR && (strlen(fileout) == 0 || strlen(filein) != 0)){
		writeInvalid();
		return;
	}
	else if(eval == LEFT_RIGHT_RDIR && (strlen(filein) == 0 || strlen(fileout) == 0)){
		writeInvalid();
		return;
	}
	else if(eval == RIGHT_APND_RDIR && (strlen(fileout) == 0 || strlen(filein) != 0)){
		writeInvalid();
		return;
	}
	
	/* check if input file exists */
	if(eval == LEFT_RDIR || eval == LEFT_RIGHT_RDIR){
		fin = open(filein, O_RDONLY, 0600);
		if(fin < 0){
			write(STDOUT, "swsh: No such file\n", 19);
			return;
		}
	}

	/* open output file */
	if(eval == RIGHT_RDIR || eval == LEFT_RIGHT_RDIR){
		fout = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, 0600);
		if(fout < 0){
			write(STDOUT, "swsh: Failed to open output file\n", 33);
			return;
		}
	}
	if(eval == RIGHT_APND_RDIR){
		fout = open(fileout, O_WRONLY | O_CREAT | O_APPEND, 0600);
		if(fout < 0){
			if(fin != -1)
				close(fin);
			write(STDOUT, "swsh: Failed to open output file\n", 33);
			return;
		}
	}
	
	/* create pipe */
	if(pipe2 != NULL){
		if(pipe2[1] != -1)
			fdsrc = pipe2[1];
		pipe(pipe2);
	}

	if((pid_c = fork()) == 0){
		switch(pos){
			case NO_PIPE:
				if(fin != -1){
					dup2(fin, STDIN);
					close(fin);
				}
				if(fout != -1){
					dup2(fout, STDOUT);
					close(fout);
				}
				break;
			case PIPE_FRNT:
				if(fin != -1){
					dup2(fin, STDIN);
					close(fin);
				}
				close(STDOUT);
				close(pipe2[READ]);
				dup2(pipe2[WRITE], STDOUT);
				close(pipe2[WRITE]);
				break;
			case PIPE_MID:
				dup2(fdsrc, STDIN);
				close(fdsrc);
				dup2(pipe2[WRITE], STDOUT);
				close(pipe2[WRITE]);
				break;
			case PIPE_BACK:
				if(fout != -1){
					dup2(fout, STDOUT);
					close(fout);
				}
				close(STDIN);
				close(pipe2[WRITE]);
				dup2(fdsrc, STDIN);
				close(fdsrc);
				break;
		}
		if(cmdtype != CMD_PATH){
			if(execvp(args[0], args) < 0){
				writeInvalid();
				exit(1);
			}
		}
		else{
			if(execv(args[0], args) < 0){
				writeInvalid();
				exit(1);
			}
		}
	}
	switch(pos){
		case NO_PIPE:
			break;
		case PIPE_FRNT:
		case PIPE_MID:
			close(pipe2[WRITE]);
			pipe2[1] = pipe2[READ];
			break;
		case PIPE_BACK:
			close(pipe2[WRITE]);
			close(pipe2[READ]);
			break;
	}
	setpgid(pid_c, pid_p); // set child's pgid to parent's pid
	waitpid(pid_c, &stat, 0);

}
