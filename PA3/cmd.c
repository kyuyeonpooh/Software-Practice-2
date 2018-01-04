#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "cmd.h"
#include "cmd_type1.h"
#include "cmd_type2.h"
#include "cmd_type3.h"
#include "cmd_type4.h"
#include "string_sw.h"
#include "macro.h"

void cmdchk(char* input){

	int chkCount = 1;
	int cmdCount = 0;
	char cmd[MAX_CMD_NUM + 1][MAX_CMD_LENGTH + 1];
	int cmdtype;
	char* args[MAX_ARG_NUM + 1];
	char filein[MAX_ARG_LENGTH + 1];
	char fileout[MAX_ARG_LENGTH + 1];
	int eval = 0;
	char* cmdToken;

	*(strchr(input, '\n') + 1) = '\0'; // null-terminate input
	if(!strcmp(input, "\n")){
		writeInvalid();
		return;
	}

	/* check the number of commands in input */
	chkCount += strstr_n(input, " | ");

	/* tokenize input by '|' */
	cmdToken = strtok_i(input, "|");
	while(cmdToken){
		cmdCount++;
		strcpy(cmd[cmdCount], cmdToken);
		cmdToken = strtok_i(NULL, "|");
	}
	
	if(chkCount != cmdCount){
		writeInvalid();
		return;
	}
	
	/* no pipe in command */
	if(cmdCount == 1){
	
		*strrchr(cmd[1], '\n') = '\0'; // delete new-line character
		
		eval = evalRedir(cmd[1]);
		if(eval == UNDEFINED){
			writeInvalid();
			return;
		}
		
		init_args(args);
		cmdtype = parse(args, cmd[1], filein, fileout);

		switch(cmdtype / 10){
			case INVALID:
				writeInvalid();
				break;
			case 1: // cmd_type 1
				cmd_type1(cmdtype, eval, args, filein, fileout, NULL, NO_PIPE);
				break;
			case 2: // cmd_type 2
				cmd_type2(cmdtype, eval, args, filein, fileout, NULL, NO_PIPE);
				break;
			case 3: // cmd_type 3
				cmd_type3(cmdtype, eval, args, filein, fileout, NULL, NO_PIPE);
				break;
			case 4: // cmd_type 4
				cmd_type4(cmdtype, eval, args, filein, fileout, NULL, NO_PIPE);
				break;
			default:
				writeInvalid();
				break;
		}

		free_args(args);

	}

	/* pipe exists in command */
	else{
		
		char* cmdcur;
		int pipe[2] = {-1, -1};

		for(int i = 1; i <= cmdCount; i++){

			cmdcur = cmd[i];
			
			/* if first command */
			if(i == 1){
				
				*strrchr(cmdcur, ' ') = '\0'; // delete white space
				
				eval = evalRedir(cmdcur);
				if(eval != NO_RDIR && eval != LEFT_RDIR){ // has redirection but not "<"
					writeInvalid();
					return;
				}
				
				init_args(args);
				cmdtype = parse(args, cmdcur, filein, fileout);

				switch(cmdtype / 10){
					case INVALID:
						writeInvalid();
						break;
					case 1: // cmd_type 1
						cmd_type1(cmdtype, eval, args, filein, fileout, pipe, PIPE_FRNT);
						break;
					case 2: // cmd_type 2
						cmd_type2(cmdtype, eval, args, filein, fileout, pipe, PIPE_FRNT);
						break;
					case 3: // cmd_type 3
						cmd_type3(cmdtype, eval, args, filein, fileout, pipe, PIPE_FRNT);
						break;
					case 4: // cmd_type 4
						cmd_type4(cmdtype, eval, args, filein, fileout, pipe, PIPE_FRNT);
						break;
					default:
						writeInvalid();
						break;
				}

			}

			/* if last command */
			else if(i == cmdCount){
				
				cmdcur += 1; // delete white space at the front
				*strrchr(cmdcur, '\n') = '\0'; // delete new-line character
				
				eval = evalRedir(cmdcur);
				if(eval != NO_RDIR && eval != RIGHT_RDIR){ // has redirection but not ">"
					writeInvalid();
					return;
				}
				
				init_args(args);
				cmdtype = parse(args, cmdcur, filein, fileout);
				
				switch(cmdtype / 10){
					case INVALID:
						writeInvalid();
						break;
					case 1: // cmd_type 1
						cmd_type1(cmdtype, eval, args, filein, fileout, pipe, PIPE_BACK);
						break;
					case 2: // cmd_type 2
						cmd_type2(cmdtype, eval, args, filein, fileout, pipe, PIPE_BACK);
						break;
					case 3: // cmd_type 3
						cmd_type3(cmdtype, eval, args, filein, fileout, pipe, PIPE_BACK);
						break;
					case 4: // cmd_type 4
						cmd_type4(cmdtype, eval, args, filein, fileout, pipe, PIPE_BACK);
						break;
					default:
						writeInvalid();
						break;
				}

			}

			/* commands between pipes */
			else{
				
				cmdcur += 1;
				*strrchr(cmdcur, ' ') = '\0'; // delete white space at front and back
				
				eval = evalRedir(cmdcur);
				if(eval != NO_RDIR){ // has redirection
					writeInvalid();
					return;
				}

				init_args(args);
				cmdtype = parse(args, cmdcur, filein, fileout);
				
				switch(cmdtype / 10){
					case INVALID:
						writeInvalid();
						break;
					case 1: // cmd_type 1
						cmd_type1(cmdtype, eval, args, filein, fileout, pipe, PIPE_MID);
						break;
					case 2: // cmd_type 2
						cmd_type2(cmdtype, eval, args, filein, fileout, pipe, PIPE_MID);
						break;
					case 3: // cmd_type 3
						cmd_type3(cmdtype, eval, args, filein, fileout, pipe, PIPE_MID);
						break;
					case 4: // cmd_type 4
						cmd_type4(cmdtype, eval, args, filein, fileout, pipe, PIPE_MID);
						break;
					default:
						writeInvalid();
						break;
				}

			}

			free_args(args);
		
		}

	}

}

/* return type of command, return false if command is invalid */
int parse(char* args[], char* cmd, char* filein, char* fileout){
	
	int argc = 0;
	int cmdtype;
	char* arg;

	*filein = '\0';
	*fileout = '\0';

	arg = strtok(cmd, " ");
	if(arg == NULL)
		return INVALID;
	strcpy(args[argc++], arg);
	
	if(!strcmp(arg, "ls"))
		cmdtype = CMD_LS;
	else if(!strcmp(arg, "man"))
		cmdtype = CMD_MAN;
	else if(!strcmp(arg, "grep"))
		cmdtype = CMD_GREP;
	else if(!strcmp(arg, "sort"))
		cmdtype = CMD_SORT;
	else if(!strcmp(arg, "awk"))
		cmdtype = CMD_AWK;
	else if(!strcmp(arg, "bc"))
		cmdtype = CMD_BC;
	else if(!strncmp(arg, "./", 2))
		cmdtype = CMD_PATH;
	else if(!strcmp(arg, "head"))
		cmdtype = CMD_HEAD;
	else if(!strcmp(arg, "tail"))
		cmdtype = CMD_TAIL;
	else if(!strcmp(arg, "cat"))
		cmdtype = CMD_CAT;
	else if(!strcmp(arg, "cp"))
		cmdtype = CMD_CP;
	else if(!strcmp(arg, "mv"))
		cmdtype = CMD_MV;
	else if(!strcmp(arg, "rm"))
		cmdtype = CMD_RM;
	else if(!strcmp(arg, "cd"))
		cmdtype = CMD_CD;
	else if(!strcmp(arg, "pwd"))
		cmdtype = CMD_PWD;
	else if(!strcmp(arg, "exit"))
		cmdtype = CMD_EXIT;
	else
		return INVALID;
		
	arg = strtok(NULL, " ");
	while(arg){
		if(!strcmp(arg, "<")){
			arg = strtok(NULL, " ");
			if(arg == NULL)
				break;
			strcpy(filein, arg);
			arg = strtok(NULL, " ");
		}
		else if(!strcmp(arg, ">") || !strcmp(arg, ">>")){
			arg = strtok(NULL, " ");
			if(arg == NULL)
				break;
			strcpy(fileout, arg);
			arg = strtok(NULL, " ");
		}
		else{
			strcpy(args[argc++], arg);
			arg = strtok(NULL, " ");
		}
	}
	free(args[argc]);
	args[argc] = NULL;

	return cmdtype;

}

int evalRedir(const char* cmd){

	int lcnt = strcnt(cmd, '<');
	int rcnt = strcnt(cmd, '>');
	int rdirLeft = strstr_n(cmd, " < ");
	int rdirRight = strstr_n(cmd, " > ");
	int rdirRight_a = strstr_n(cmd, " >> ");
	char* p;

	if(rdirLeft == 0 && rdirRight == 0 && rdirRight_a == 0 && lcnt == 0 && rcnt == 0)
		return NO_RDIR;	
	else if(rdirLeft == 1 && rdirRight == 0 && rdirRight_a == 0 && lcnt == 1 && rcnt == 0)
		return LEFT_RDIR;
	else if(rdirLeft == 0 && rdirRight == 1 && rdirRight_a == 0 && lcnt == 0 && rcnt == 1)
		return RIGHT_RDIR;
	else if(rdirLeft == 0 && rdirRight == 0 && rdirRight_a == 1 && lcnt == 0 && rcnt == 2)
		return RIGHT_APND_RDIR;
	else if(rdirLeft == 1 && rdirRight == 1 && rdirRight_a == 0 && lcnt == 1 && rcnt == 1){
		p = strstr(cmd, " < ") + 3;
		p = strstr(p, " > ");
		if(p != NULL)
			return LEFT_RIGHT_RDIR;
	}
	return UNDEFINED;

}

void writeInvalid(void){

	write(STDOUT, "swsh: Command not found\n", 24);

}

void init_args(char* args[]){
	
	/* initialize argument array */
	for(int i = 0; i < MAX_ARG_NUM + 1; i++)
		args[i] = (char*) calloc(MAX_ARG_LENGTH + 1, sizeof(char));

}

void free_args(char* args[]){

	/* free argument array */
	for(int i = 0; i < MAX_ARG_NUM + 1; i++)
		if(args[i])
			free(args[i]);

}
