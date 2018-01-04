#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cmd.h"
#include "string_sw.h"
#include "macro.h"

void cmd_pwd(char* args[]){

	char dir[100];

	getcwd(dir, 100);
	write(STDOUT, dir, strlen(dir));
	write(STDOUT, "\n", 1);
	exit(0);

}

void cmd_exit(char* args[]){

	int exitnum;

	if(args[1] == NULL || strlen(args[1]) == 0){
		write(STDERR, "exit\n", 5);
		free_args(args);
		exit(0);
	}
	write(STDERR, "exit\n", 5);
	exitnum = atoi(args[1]);
	free_args(args);
	exit(exitnum);

}

void cmd_type4(int cmdtype, int eval, char* args[], char* filein, char* fileout, int* pipe2, int pos){

	int pid_c;
	int pid_p = getpid();
	int fin = -1;
	int fout = -1;
	int stat;
	int fdsrc = -1;

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
	
	switch(cmdtype){
		case CMD_PWD:
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
			setpgid(pid_c, pid_p);
			waitpid(pid_c, &stat, 0);
			break;
		case CMD_EXIT:
			cmd_exit(args);
			break;
	}

}

