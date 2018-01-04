//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Code for PA #2
// October 4, 2017
//
// 2016314827 Kim Kyu Yeon
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CMD_NUM 10000
#define MAX_CMD_LENGTH 1000
#define MAX_DOMAIN_LENGTH 200
#define MAX_TITLE_LENGTH 200
#define STDIN 0
#define STDOUT 1
#define STDERR 2

/* funtion declarations */
extern void handler1(int sig);
extern void handler2(int sig);
extern void collect(void);
extern void collect_l(int fd, int url_cnt);
extern void print(char* domain);
extern void stat(void);
extern void getTitle(int url_cnt);
extern char* parseDomain(int cmd_num);
extern char* parseTitle(char* title, char* domain, int fd, int cmd_num);
extern int isWhiteSpace(char c);
extern char *strchr (const char *str, int ch);
extern char *strrchr (const char *str, int ch);
extern size_t strlen(const char *str);
extern char *strcpy (char *dst, const char *src);
extern int strcmp (const char *lhs, const char *rhs);
extern int strncmp (const char *lhs, const char *rhs, size_t count);
extern char* num2str(char *dest, int num);

typedef struct domainSet{
	int maxLength;
	char domain[MAX_DOMAIN_LENGTH + 1];
	char maxTitle[MAX_TITLE_LENGTH + 1];	
} domainSet;

/* global variables */
char cmdSet[MAX_CMD_NUM + 1][MAX_CMD_LENGTH + 1]; // colletion of all command lines
domainSet dSet[MAX_CMD_NUM + 1]; // collection of unique domains with longest title
int cmd_cnt = 1; // number of saved command lines
int domain_cnt = 0; // number of unique domains
int title_cnt = 0; // number of titles

void handler1(int sig){
}

void handler2(int sig){
	stat();
	cmd_cnt++;
}

void collect(void){

	signal(SIGINT, handler1);
	signal(SIGUSR1, handler2);
	collect_l(STDIN, 0);

}

void collect_l(int fd, int url_cnt){
	
	int cmd_len = 0; // length of each command
	int status = 0;
	char* cmd; // a single command

	while(1){
	
		/* get command(or URL) from stdin */
		cmd = cmdSet[cmd_cnt];
		while((status = read(fd, &cmd[cmd_len], 1))){
			if(cmd[cmd_len] == '\n')
				break;
			cmd_len++;
		}
		if((fd != STDIN) && (status == 0)){ // exception for "load" command
			getTitle(url_cnt);
			cmd_cnt--;
			return;
		}
		cmd[cmd_len] = '\0';
		
		/* execute a single command */
		switch(cmd[0]){
			case 'h':
				if(!strncmp(cmd, "http://", 7) || !strncmp(cmd, "https://", 8)){ // URL command
					url_cnt++;
				}
				else // unexpected command (no change in cmd_cnt)
					cmd_cnt--;
				break;
			case 'l':
				if(!strncmp(cmd, "load ", 5)){ // command "load"
					collect_l(open(cmd + 5, O_RDONLY, 0755), url_cnt);
					url_cnt = 0;
				}
				else
					cmd_cnt--;
				break;
			case 'p':
				if(!strncmp(cmd, "print ", 6)){ // command "print"
					if(url_cnt)
						getTitle(url_cnt);
					print(cmd + 6);
					url_cnt = 0;
				}
				else
					cmd_cnt--;
				break;
			case 's':
				if(!strncmp(cmd, "stat", 4) && cmd_len == 4){ // command "stat"
					if(url_cnt)
						getTitle(url_cnt);
					stat();
					url_cnt = 0;
				}
				else
					cmd_cnt--;
				break;
			case 'q':
				if(!strncmp(cmd, "quit", 4) && cmd_len == 4){ // command "quit"
					if(url_cnt)
						getTitle(url_cnt);
					exit(0);
				}
				else
					cmd_cnt--;
				break;
			default: // unexpected command
				cmd_cnt--;
				break;
		}
		cmd_cnt++;
		cmd_len = 0;

		/* exception for empty stdin */
		if((fd == STDIN) && (lseek(STDIN, 1, SEEK_CUR) == -1) && (url_cnt != 0)){
			getTitle(url_cnt);
			url_cnt = 0;
		}

	}
}

void print(char* domain){
	
	char numstr[12];

	num2str(numstr, cmd_cnt);
	for(int i = 0; i < domain_cnt; i++){
		if(*domain == '\0')
			break;
		if(!strcmp(dSet[i].domain, domain)){
			write(STDOUT, numstr, strlen(numstr));
			write(STDOUT, ">", 1);
			write(STDOUT, domain, strlen(domain));
			write(STDOUT, ":", 1);
			write(STDOUT, dSet[i].maxTitle, strlen(dSet[i].maxTitle));
			write(STDOUT, "\n", 1);
			return;
		}
	}
	write(STDOUT, numstr, strlen(numstr));
	write(STDOUT, ">", 1);
	write(STDOUT, "Not Available\n", 14);

}

void stat(void){
  	
	char numstr[12];
	char cnt[12];

	num2str(numstr, cmd_cnt);
	num2str(cnt, title_cnt);
	write(STDOUT, numstr, strlen(numstr));
	write(STDOUT, ">", 1);
	write(STDOUT, cnt, strlen(cnt));
	write(STDOUT, " titles\n", 8);

}

void getTitle(int url_cnt){

	pid_t pid[url_cnt];
	int child_status;
	char numstr[12]; // used for num2str function

	/* generate children process */
	for(int i = cmd_cnt - url_cnt; i < cmd_cnt; i++){
		if((pid[i - cmd_cnt + url_cnt] = fork()) == 0){
			num2str(numstr, i);
			execlp("wget", "wget", "-qO", numstr, cmdSet[i], (char*) NULL);
		}
	}
	
	for(int i = 0; i < url_cnt; i++){
		
		pid_t wpid = wait(&child_status);
		
		int j;
		for(j = 0; j < url_cnt; j++)
			if(wpid == pid[j])
				break;
		j += cmd_cnt - url_cnt;
		
		/* examine exit status of wget */
		num2str(numstr, j);
		if(!WEXITSTATUS(child_status)){ // wget successfully finished

			char* domain;
			char title[MAX_TITLE_LENGTH + 1] = {0,};
			char* refinedTitle;

			domain = parseDomain(j);
			refinedTitle = parseTitle(title, domain, open(numstr, O_RDONLY, 0755), j);

			write(STDOUT, numstr, strlen(numstr));
			write(STDOUT, ">", 1);
			write(STDOUT, domain, strlen(domain));
			write(STDOUT, ":", 1);
			write(STDOUT, refinedTitle, strlen(refinedTitle));
			write(STDOUT, "\n", 1);

		}
		else{ // error occurred in wget
			write(STDERR, numstr, strlen(numstr));
			write(STDERR, ">Error occurred!\n", 17);
		}

	}

}

char* parseDomain(int cmd_num){
	
	char* domain = strchr(cmdSet[cmd_num], '/') + 2;
	char* domain_end;
	int dot_cnt = 0;

	if((domain_end = strchr(domain, '/'))) // has '/' after http:// or https://
		*domain_end = '\0';

	domain_end = strrchr(domain, '.');
	if(!strncmp(domain_end + 1, "kr", 2)){ // end with .kr
		while((domain != domain_end) && (dot_cnt < 2)){
			domain_end--;
			if(*domain_end == '.')
				dot_cnt++;
		}
	}
	else{ // end with something else (.com ...)
		while((domain != domain_end) && (dot_cnt < 1)){
			domain_end--;
			if(*domain_end == '.')
				dot_cnt++;
		}
	}
	if(domain != domain_end)
		domain_end++;

	/* save in domain set */
	int i;
	for(i = 0; i < domain_cnt; i++)
		if(!strcmp(dSet[i].domain, domain_end))
			break;
	if(i == domain_cnt){
		strcpy(dSet[i].domain, domain_end);
		domain_cnt++; // increment number of unique domain count
	}
	return domain_end;

}

char* parseTitle(char* roughTitle, char* domain, int fd, int cmd_num){
	
	char buffer[10];
	char* refinedTitle = roughTitle;
	int length = 0;

	while(read(fd, buffer, 1)){
		if(buffer[0] == '<'){
			read(fd, buffer, 6);
			if(!strncmp(buffer, "title>", 6)){ // looking for <title>
				while(read(fd, buffer, 1)){
					if(buffer[0] == '<'){
						read(fd, buffer, 7);
						if(!strncmp(buffer, "/title>", 7)){ // looking for </title>
							
							/* refine the title */
							while(isWhiteSpace(refinedTitle[0])) // refine from front
								refinedTitle++;
							while(isWhiteSpace(roughTitle[length - 1]))
								length--;
							roughTitle[length] = '\0';
							length -= roughTitle - refinedTitle;
							title_cnt++; // increment title count

							/* save in domain set */
							int i;
							for(i = 0; i < domain_cnt; i++)
								if(!strcmp(dSet[i].domain, domain))
									break;
							if(length > dSet[i].maxLength){
								dSet[i].maxLength = length;
								strcpy(dSet[i].maxTitle, refinedTitle);
							}
							return refinedTitle;

						}
						else{
							roughTitle[length++] = '<';
							lseek(fd, -7, SEEK_CUR);
						}
					}
					else
						roughTitle[length++] = buffer[0]; // save title
				}
			}
			else
				lseek(fd, -6, SEEK_CUR);
		}
	}
	write(STDOUT, "Failed to parse the title!\n", 27);
	return roughTitle;

}

int isWhiteSpace(char c){
	return (c == ' ') || (c == '\t') || (c == '\n');
}

size_t strlen (const char *str){

	size_t len = 0;	

	while(*str)
		len++, str++;
	return len;

}

char *strcpy (char *dst, const char *src){
	
	size_t index = 0;

	while(*src){
		dst[index] = *src;
		index++, src++;
	}
	dst[index] = '\0';
	return dst;

}

int strcmp (const char *lhs, const char *rhs){
	
	int dif;
	
	if(!lhs || !rhs)
		return -1;

	while(*lhs || *rhs){
		dif = *lhs - *rhs;
		if(dif)
			return dif;
		lhs++, rhs++;
	}
	return 0;

}

int strncmp (const char *lhs, const char *rhs, size_t count){

	size_t index = 0;
	int dif;

	if(!lhs || !rhs)
		return -1;

	while((lhs[index] || rhs[index]) && index < count){
		dif = lhs[index] - rhs[index];
		if(dif)
			return dif;
		index++;
	}
	return 0;

}

char *strchr (const char *str, int ch){
	
	while(*str){
		if(ch == *str)
			return (char*) str;
		str++;
	}
	return NULL;

}

char *strrchr (const char *str, int ch){

	char* p = NULL;

	while(*str){
		if(ch == *str)
			p = (char*) str;
		str++;
	}
	return p;

}

char* num2str(char *dest, int num){
	
	int index = 0;
	char temp[12];
	int i = 0;

	switch(num){
		case 0:
			dest[0] = '0';
			dest[1] = '\0';
			return dest;
		default:
			while(num){
				temp[index++] = (num % 10) + '0';
				num /= 10;
			}
	}

	while(index >= 0)
		dest[i++] = temp[--index];
	return dest;

}

int main(int argc, char* argv[]){

	collect();
	return 0;

}
