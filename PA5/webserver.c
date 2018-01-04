//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #5
// December 6, 2017
//
// 2016314827 Kim Kyu Yeon
// Embedded Software Laboratory
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "http.h"
#include "string_sw.h"

#define BUF_SIZE 1024
#define NAME_LEN 256

pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t restart = PTHREAD_COND_INITIALIZER;
static int connections;
static long long sent_bytes;
static int html, jpg, png, gif, js, css, error;
static int thread_cnt;
static int reset_flag;

void update_type(int type){	

	switch(type){
		case TYPE_HTML:
			html += 1;
			break;
		case TYPE_JPG:
			jpg += 1;
			break;
		case TYPE_PNG:
			png += 1;
			break;
		case TYPE_GIF:
			gif += 1;
			break;
		case TYPE_JS:
			js += 1;
			break;
		case TYPE_CSS:
			css += 1;
			break;
		case TYPE_ERROR:
			error += 1;
			break;
	}

}

int content_type(char* type){

	if(!strcmp(type, "html") || !strcmp(type, "HTML"))
		return TYPE_HTML;
	if(!strcmp(type, "jpg") || !strcmp(type, "JPG"))
		return TYPE_JPG;
	if(!strcmp(type, "png") || !strcmp(type, "PNG"))
		return TYPE_PNG;
	if(!strcmp(type, "gif") || !strcmp(type, "GIF"))
		return TYPE_GIF;
	if(!strcmp(type, "js") || !strcmp(type, "JS"))
		return TYPE_JS;
	if(!strcmp(type, "css") || strcmp(type, "CSS"))
		return TYPE_CSS;
	return TYPE_ERROR;

}

void* client_proc(void *arg){

	pthread_mutex_lock(&locker);
	thread_cnt += 1; // increment thread counter
	pthread_mutex_unlock(&locker);

	int connfd = *(int*) arg; // socket descriptor
	char buf[BUF_SIZE + 1] = {0,};
	char file_name[NAME_LEN + 1] = {0,};
	char* file_type;
	int type; // integer file type value
	int file_size = 0;
	int read_cnt = 0; // counter of read bytes
	int fd = -1; // file descriptor
	char* http_header = NULL; // http header part
	char* content = NULL; // http body part

	/* get client request message */
	read_cnt = 0;
	do {
		read_cnt += read(connfd, buf + read_cnt, BUF_SIZE / 4);
		if (read_cnt >= 4 && strncmp(buf + read_cnt - 4, "\r\n\r\n", 4) == 0)
			break;
	} while (read_cnt <= BUF_SIZE);

	/* get file name */
	int i;
	for(i = 5; i < BUF_SIZE; i++){
		if(buf[i] == ' ')
			break;
	}
	if(i == 5)
		strcpy(file_name, "index.html");
	else
		strncpy(file_name, buf + 5, i - 5);

	/* get file type */
	if(!strcmp(file_name, "stat"))
		file_type = file_name;
	else if(!strcmp(file_name, "reset")){
		file_type = file_name;
	}
	else{
		file_type = strrchr(file_name, '.');
		if(file_type == NULL){
			write(2, "unknown file type\n", 18);
			file_type = file_name;
		}
		if(file_type != NULL)
			file_type += 1;
	}

	/* open requested file and make http header */
	if(!strcmp(file_type, "stat")){ // stat
		type = TYPE_HTML; // treated as HTML file
		pthread_mutex_lock(&locker);
		content = make_report(connections, sent_bytes, html, jpg, png, gif, js, css, error);
		pthread_mutex_unlock(&locker);
		file_size = strlen(content);
		http_header = make_http_header(OK, TYPE_HTML, file_size);
	}
	else if(!strcmp(file_type, "reset")){ // reset
		type = TYPE_HTML;
		pthread_mutex_lock(&locker);
		reset_flag = 1;
		while(thread_cnt > 1){ // wait for other threads to finish
			pthread_cond_wait(&cond, &locker);
		}
		sent_bytes = 0; // reset sent_bytes to 0
		content = make_report(connections, sent_bytes, html, jpg, png, gif, js, css, error);
		pthread_mutex_unlock(&locker);
		file_size = strlen(content);
		http_header = make_http_header(OK, TYPE_HTML, file_size);
	}
	else{
		fd = open(file_name, O_RDONLY, 0600);
		if(fd  < 0){
			type = TYPE_ERROR;
			switch(errno){
				case ENOENT: // not such file
					http_header = make_http_header(E_NO_FILE, TYPE_ERROR, 0);
					break;
				case EACCES: // permission denied
					http_header = make_http_header(E_NOT_ALLOWED, TYPE_ERROR, 0);
					break;
				default: // unknown error (treated as EACCES)
					http_header = make_http_header(E_NOT_ALLOWED, TYPE_ERROR, 0);
					break;
			}
		}
		else{
			type = content_type(file_type); // get file_type in interger value
			if(type == TYPE_ERROR)
				http_header = make_http_header(E_NOT_ALLOWED, type, 0);
			else{
				file_size = lseek(fd, 0, SEEK_END);
				lseek(fd, 0, SEEK_SET);
				content = (char*) malloc(file_size * sizeof(char));
				read(fd, content, file_size);
				http_header = make_http_header(OK, type, file_size);
			}
		}
	}
	if (http_header == NULL){
		write(2, "make_http_header returns NULL\n", 30);
		exit(5);
	}

	/* send http header and null line */
	long long sent = 0;
	int header_len = strlen(http_header);
	do {
		sent += write(connfd, http_header + sent, header_len - sent);
	} while (sent < header_len); // until write is done

	/* send body */
	if(!content){ // error occurred above
		
	}
	else{
		sent = 0;
		do {
			sent += write(connfd, content + sent, file_size - sent);
		} while (sent < file_size);
	}

	/* deallocate resources */
	free(arg);
	free(http_header);
	if(content)
		free(content);
	pthread_mutex_lock(&locker);
	if(fd > 0)
		close(fd);
	close(connfd);

	/* update status */
	connections += 1;
	sent_bytes += file_size;
	update_type(type);
	thread_cnt -= 1; // decrement thread counter
	pthread_cond_signal(&cond);
	if(!strcmp(file_name, "reset")){
		reset_flag = 0;
		pthread_cond_signal(&restart);
	}
	pthread_mutex_unlock(&locker);

	return NULL;

}

int main(int argc, char *argv[]){
	
	struct hostent;
	struct sockaddr_in saddr;
	int port = 1234;
	int listenfd, connfd;

	if (argc == 2 && argv[1] != NULL){
		port = atoi2(argv[1]);
	}

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(1);
	}

	bzero((char*)&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);

	if (bind(listenfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0){
		perror("bind");
		exit(2);
	}

	if (listen(listenfd, 5) < 0){
		perror("listen");
		exit(3);
	}

	pthread_t new_client;
	pthread_attr_t attr;

	while(1){

		while(reset_flag == 1){
			pthread_mutex_lock(&locker);
			while(thread_cnt > 0)
				pthread_cond_wait(&restart, &locker);
			pthread_mutex_unlock(&locker);
		}
		if ((connfd = accept(listenfd, NULL, NULL)) < 0){
			perror("accept");
			exit(4);
		}
		int* cfd = (int*) malloc(sizeof(int)); // safe place to save connfd

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		*cfd = connfd;
		pthread_create(&new_client, &attr, client_proc, (void*) cfd);
		pthread_attr_destroy(&attr);

	}

	return 0;

}

