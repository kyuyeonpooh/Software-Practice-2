#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "cmd.h"
#include "cmd_type3.h"
#include "string_sw.h"
#include "macro.h"

void cmd_mv(char* args[]){
	
	/* need more arguments */
	if(args[1] == NULL || strlen(args[1]) == 0){
		write(STDERR, "mv: Need more arguments\n", 24);
		exit(1);
	}
	if(args[2] == NULL || strlen(args[2]) == 0){
		write(STDERR, "mv: Need more arguments\n", 24);
		exit(1);
	}

	/* error occurred */
	if(rename(args[1], args[2]) < 0){
		write(STDERR, "mv: ", 4);
		writeError(errno);
	}

}

void cmd_rm(char* args[]){

	/* need more arguments */
	if(args[1] == NULL || strlen(args[1]) == 0){
		write(STDERR, "mv: Need more arguments\n", 24);
		exit(1);
	}
	
	for(int i = 1; args[i] != NULL; i++){
		if(unlink(args[i]) < 0){
			write(STDERR, "rm: ", 4);
			writeError(errno);
		}
	}

}

void cmd_cd(char* args[]){

	char* path;

	/* need more arguments */
	if(args[1] == NULL || strlen(args[1]) == 0){
		if((path = (char*) getenv("HOME")))
			path = ".";
	}
	else
		path = args[1];

	if(chdir(path) < 0){
		write(STDERR, "cd: ", 4);
		writeError(errno);
	}

}

void cmd_type3(int cmdtype, int eval, char* args[], char* filein, char* fileout, int* pipe, int pos){
	
	int fin = -1;
	int fout = -1;
	
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
		case CMD_MV:
			cmd_mv(args);
			break;
		case CMD_RM:
			cmd_rm(args);
			break;
		case CMD_CD:
			cmd_cd(args);
			break;
	}

}

void writeError(int e){
	
	char errstr[12];

	switch(e){
		case EACCES:
			write(STDERR, "Permission denied\n", 18);
			break;
		case EISDIR:
			write(STDERR, "Is a directory\n", 15);
			break;
		case ENOENT:
			write(STDERR, "No such file or directory\n", 26);
			break;
		case ENOTDIR:
			write(STDERR, "Not a directory\n", 16);
			break;
		case EPERM:
			write(STDERR, "Permission denied\n", 18);
			break;
		default:
			write(STDERR, "Error occurred: ", 16);
			write(STDERR, int2str(errstr, e), strlen(errstr));
			write(STDERR, "\n", 1);
			break;
	}

}
