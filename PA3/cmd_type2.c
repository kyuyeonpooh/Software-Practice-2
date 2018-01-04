#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cmd.h"
#include "string_sw.h"
#include "macro.h"

void cmd_head(char* args[]){
	
	int fd = -1;
	int k = -1; // number of lines to print
	char buf = '\0';
	int line = 0;
	
	/* get input from stdin */
	if(args[1] == NULL || strlen(args[1]) == 0){
		fd = STDIN;
	}

	/* "-n K" option */
	else if(!strcmp(args[1], "-n")){
		if(args[2] == NULL || strlen(args[2]) == 0){
			write(STDERR, "head: Undefined option\n", 23);
			exit(1);
		}
		k = atoi2(args[2]);
	}
	else
		fd = open(args[1], O_RDONLY, 0600);
	
	if(k != -1){
		if(args[3] == NULL || strlen(args[3]) == 0)
			fd = STDIN;
		else
			fd = open(args[3], O_RDONLY, 0600);
	}
	else
		k = 10;

	if(fd < 0){
		write(STDERR, "head: No such file\n", 19);
		exit(1);
	}
	while(read(fd, &buf, 1)){
		write(STDOUT, &buf, 1);
		if(buf == '\n')
			line++;
		if(line == k)
			break;
	}
	close(fd);
	exit(0);

}

void cmd_tail(char* args[]){
	
	int fd = -1;
	int k = -1; // number of lines to print
	char buf[BUF_SIZE] = {0,};
	int line = 0;
	int rd_size = 0;
	int i = 0;

	/* get input from stdin */
	if(args[1] == NULL || strlen(args[1]) == 0){
		fd = STDIN;
	}

	/* "-n K" option */
	else if(!strcmp(args[1], "-n")){
		if(args[2] == NULL || strlen(args[2]) == 0){
			write(STDERR, "tail: Undefined option\n", 23);
			exit(1);
		}
		k = atoi2(args[2]);
	}
	else
		fd = open(args[1], O_RDONLY, 0600);
	
	if(k != -1){
		if(args[3] == NULL || strlen(args[3]) == 0)
			fd = STDIN;
		else
			fd = open(args[3], O_RDONLY, 0600);
	}
	else
		k = 10;
	
	if(fd < 0){
		write(STDERR, "tail: No such file\n", 19);
		exit(1);
	}
	
	while((rd_size = read(fd, &buf[i], BUF_SIZE)))
		i += rd_size;
	rd_size = i--;
	while(i >= 0){
		if(buf[i] == '\n')
			line++;
		if(line > k)
			break;
		i--;
	}
	i += 1;
	write(STDOUT, &buf[i], rd_size - i);
	exit(0);

}

void cmd_cat(char* args[]){

	int fd = -1;
	char buf = '\0';
	
	/* get input from stdin */
	if(args[1] == NULL || strlen(args[1]) == 0)
		fd = STDIN;
	else
		fd = open(args[1], O_RDONLY, 0600);
	if(fd < 0){
		write(STDERR, "cat: No such file\n", 18);
		exit(1);
	}
	while(read(fd, &buf, 1))
		write(STDOUT, &buf, 1);
	close(fd);
	exit(0);

}

void cmd_cp(char* args[]){

	int fdr = -1;
	int fdw = -1;
	char buf = '\0';
	
	/* need more arguments */
	if(args[1] == NULL || strlen(args[1]) == 0){
		write(STDERR, "cp: Need more arguments\n", 25);
		exit(1);
	}
	if(args[2] == NULL || strlen(args[2]) == 0){
		write(STDERR, "cp: Need more arguments\n", 25);
		exit(1);
	}
	
	/* file open */
	fdr = open(args[1], O_RDONLY, 0600);
	if(fdr < 0){
		write(STDERR, "cp: No such file\n", 17);
		exit(1);
	}
	fdw = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if(fdw < 0){
		write(STDERR, "cp: No such file\n", 17);
		close(fdr);
		exit(1);
	}
	
	while(read(fdr, &buf, 1))
		write(fdw, &buf, 1);
	close(fdr);
	close(fdw);
	exit(0);

}

void cmd_type2(int cmdtype, int eval, char* args[], char* filein, char* fileout, int* pipe2, int pos){
	
	int fin = -1;
	int fout = -1;
	int pid_p = getpid(); // process id of parent
	int pid_c; // process id of child
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
	
	/* create pipe */
	if(pipe2 != NULL){
		if(pipe2[1] != -1)
			fdsrc = pipe2[1];
		pipe(pipe2);
	}

	/* create child process */
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
		switch(cmdtype){
			case CMD_HEAD:
				cmd_head(args);
				break;
			case CMD_TAIL:
				cmd_tail(args);
				break;
			case CMD_CAT:
				cmd_cat(args);
				break;
			case CMD_CP:
				cmd_cp(args);
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
	setpgid(pid_c, pid_p); // set child's pgid to parent's pid
	waitpid(pid_c, &stat, 0);

}
