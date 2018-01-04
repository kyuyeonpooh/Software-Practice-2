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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define STDIN 0
#define STDOUT 1
#define MAX_SIZE 200
#define BUF_SIZE 1024

void child_handler(int sig){

	while(waitpid(-1, NULL, WNOHANG) > 0);

}

int main (int argc, char *argv[]) {

	int port_num = -1;
	int lis_fd, conn_fd;
	struct sockaddr_in saddr, caddr;
	socklen_t caddr_len;

	if(argc == 1)
		port_num = 10001;
	else if(argc > 1)
		port_num = atoi_port(argv[1]);

	/* set server socket */
	if((lis_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // socket()
		write(STDOUT, "socket() failed\n", 16);
		return -1;
	}	
	bzero((char*) &saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port_num);
	if(bind(lis_fd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0){ // bind()
		write(STDOUT, "bind() failed\n", 14);
		return -1;
	}
	if(listen(lis_fd, 5) < 0){ // listen()
		write(STDOUT, "listen() failed\n", 16);
		return -1;
	}

	/* accept connection request */
	signal(SIGCHLD, (void*) child_handler);
	while(1){

		caddr_len = sizeof(caddr);
		if((conn_fd = accept(lis_fd, (struct sockaddr*) &caddr, &caddr_len)) < 0){
			write(STDOUT, "accept() failed\n", 16);
			continue;
		}
		write(STDOUT, "Server is connected with client\n", 32);

		pid_t pid;
		
		if((pid = fork()) == 0){ // child process
		
			int fd;
			char path[MAX_SIZE + 1];
			int rd_size;
			int result_code;
			int file_size;
			char buf[BUF_SIZE];

			rd_size = read(conn_fd, path, MAX_SIZE);
			path[rd_size - 1] = '\0';
			
			fd = open(path, O_RDONLY, 0600);
			if(fd < 0){
				switch(errno){
					case ENOENT:
						result_code = htonl(1);
						break;
					case EPERM:
						result_code = htonl(2);
						break;
					default:
						result_code = htonl(errno);
				}
				write(conn_fd, &result_code, sizeof(int));
				close(conn_fd);
				exit(1);
			}
			result_code = htonl(0);
			write(conn_fd, &result_code, sizeof(int));
			
			/* send file size and file to client */
			file_size = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);
			file_size = htonl(file_size);
			write(conn_fd, &file_size, sizeof(int));
			while((rd_size = read(fd, buf, BUF_SIZE)))
				write(conn_fd, buf, rd_size);

			close(fd);
			close(conn_fd);
			exit(0);
		
		}
		close(conn_fd);
		write(STDOUT, "Connection terminated\n", 22); 

	}

	return 0;

}
