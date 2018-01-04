//======================================
//
// Software Experiment 2 (PA 4)
//
// 2016314827 Kim Kyu Yeon
//
//======================================

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "string_sw.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define STDIN 0
#define STDOUT 1
#define MAX_SIZE 300
#define BUF_SIZE 1024

int main(int argc, char *argv[]){

	char input[MAX_SIZE + 1];
	int rd_size;
	char* input_ptr; // move through string to parse the input
	char *host, *port_str, *path;
	int port_num;

	while(1){
	
		memset(input, 0, MAX_SIZE + 1);
		host = port_str = path = (char*) NULL;
		port_num = -1;

		/* get command from user */
		write(STDOUT, "kyuyeon:PA4> ", 13);
		rd_size = read(STDIN, input, MAX_SIZE);
		input[rd_size] = '\0';
		
		/* parse input */
		if(rd_size <= 0){
			write(STDOUT, "\n", 1);
			exit(1);
		}
		if(!strcmp(input, "exit"))
			exit(0);
		
		host = input_ptr = input;
		if((input_ptr = strchr(input, ':'))){ // port number exist
			*input_ptr = '\0';
			port_str = ++input_ptr;
		}
		else // no port number given
			port_num = 10001;
	
		if(!input_ptr)
			input_ptr = input;
		if((input_ptr = strchr(input_ptr, '/'))){ // path exist
			*input_ptr = '\0';
			path = ++input_ptr;
		}
		else // no path given (invalid input)
			continue;
		
		if(port_str != NULL)
			port_num = atoi_port(port_str);
		if(port_num == -1)
			continue;
		if(strlen(host) == 0 || strlen(path) == 0)
			continue;

		/* connection starts from here */
		int soc_fd;
		struct hostent *h;
		struct sockaddr_in saddr;

		if((soc_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // socket() failed
			write(STDOUT, "E_CONN, ", 8);
			write(STDOUT, path, strlen(path));
			continue;
		}
		if(!(h = gethostbyname(host))){ // invalid host name
			write(STDOUT, "E_CONN, ", 8);
			write(STDOUT, path, strlen(path));
			continue;
		}
		
		bzero((char*) &saddr, sizeof(saddr));
		saddr.sin_family = AF_INET;
		bcopy((char*) h -> h_addr, (char*) &saddr.sin_addr.s_addr, h -> h_length);
		saddr.sin_port = htons(port_num);
		
		if(connect(soc_fd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0){ // connect() failed
			write(STDOUT, "E_CONN, ", 8);
			write(STDOUT, path, strlen(path));
			continue;
		}
		
		/* communicate with server */
		int result_code = -1;

		write(soc_fd, path, strlen(path));
		read(soc_fd, &result_code, sizeof(int));
		result_code = ntohl(result_code);
		switch(result_code){
			case 0:
				write(STDOUT, "OK, ", 4);
				write(STDOUT, path, strlen(path));
				break;
			case 1:
				write(STDOUT, "E_FILE, ", 8);
				write(STDOUT, path, strlen(path));
				break;
			case 2:
				write(STDOUT, "E_PERM, ", 8);
				write(STDOUT, path, strlen(path));
				break;
			default:
				write(STDOUT, "Unknown error occurred!\n", 24);
				break;
		}

		/* get file from server */
		if(result_code == 0){
			
			int file_size;
			char path_str[MAX_SIZE + 1] = "/";
			char* file_name;
			int fd;
			char buf[BUF_SIZE];

			read(soc_fd, &file_size, sizeof(int));
			file_size = ntohl(file_size);
			strcat(path_str, path);
			*strchr(path_str, '\n') = '\0';
			file_name = strrchr(path_str, '/') + 1;
			fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0600);

			while(file_size > 0){
				rd_size = read(soc_fd, buf, BUF_SIZE);
				write(fd, buf, rd_size);
				file_size -= rd_size;
			}
			close(fd);

		}		
		close(soc_fd);

	}
	
	return 0;

}
