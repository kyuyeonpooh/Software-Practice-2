//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Code for PA #1
// September 12, 2017
//
// 2016314827 Kim Kyu Yeon
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "convert.h"
#include "string_sw.h" // used string manipulation functions in string_sw.h

int convert(const char *input){
	
	int fd_r, fd_w; // file descriptor of read and write

	/* open the input file */
	fd_r = open(input, O_RDONLY, 0755);
	if(fd_r < 0){ // error management
		write(2, "Error occurred!\n", 16);
		switch(errno){
			case 2:
				return E_INPUT_NOT_EXIST;
			case 13:
				return E_INPUT_NOT_ALLOWED;
			default:
				return -1;					
		}
	}

	/* set name of output file */
	int input_len = strlen(input);
	char output[input_len + 5];
	strcpy(output, input);
	strcat(output, ".out");
	//printf("Output file name: %s\n", output);

	/* open the output file */
	fd_w = open(output, O_CREAT | O_WRONLY | O_EXCL, 0755);
	if(fd_w < 0){
		write(2, "Error occurred!\n", 16);
		switch(errno){
			case 13:
				return E_OUTPUT_NOT_ALLOWED;
			case 17:
				return E_OUTPUT_EXIST;
			default:
				return -1;
		}
	}

	/* determine the file type and start converting */
	char fileType = getFileType(fd_r);
	int size; // size of file
	lseek(fd_r, 0, SEEK_SET); // set back to the start point of input file
	//printf("Convert file type: %c\n", fileType);
	switch(fileType){
		case 'U': // Unix -> Dos
			size = unix2dos(fd_r, fd_w);
			break;
		case 'D': // Dos -> Unix
			size = dos2unix(fd_r, fd_w, input);
			break;
		case 'N': // no new line charatcer (just copy)
			size = copy(fd_r, fd_w);
			break;
	}

	/* write result message */
	write(1, "Input:", 6);
	write(1, input, input_len);
	write(1, " Output:", 8);
	write(1, output, input_len + 4);
	char *c = NULL;
	c = int2str(c, size);
	write(1, " Size:", 6);
	write(1, c, strlen(c));
	write(1, "\n", 1);

	/* close file descriptor and return 0 */
	free(c);
	close(fd_r);
	close(fd_w);
	return 0;

}

/* determine the type to convert */
char getFileType(int fd){

	/* count new lines according to each type */
	int dosCount = 0;
	int unixCount = 0;
	char buffer = '\0';
	while(read(fd, &buffer, 1)){
		if(buffer == '\n'){
			lseek(fd, -2, SEEK_CUR);
			read(fd, &buffer, 1);
			if(buffer == '\r')
				dosCount++;
			else
				unixCount++;
			lseek(fd, 1, SEEK_CUR);
		}
	}
	
	/* compare counts and return the file type */
	if(unixCount >= dosCount && unixCount){
		if(dosCount == 0) // if input file is completely Unix format (Unix -> Dos)
			return 'U';
		else // Unix, Dos mixed format, but general trend is Unix (Dos -> Unix)
			return 'D';
	}
	else if(dosCount > unixCount){
		if(unixCount == 0) // if input file is completely Dos format (Dos -> Unix)
			return 'D';
		else // Unix, Dos mixed format, but general trend is Dos (Unix -> Dos)
			return 'U';
	}
	return 'N'; // there is no new line character (just copy the input to output file)

}

/* check if given input file is source code file */
int isSourceCode(const char* input){

	char *p = strrchr(input, '.') + 1;

	//printf("Type check: %s\n", p);
	if(!strcmp(p, "c") || !strcmp(p, "h") || !strcmp(p, "java"))
		return 1;
	return 0;

}

/* Unix format -> Dos format */
int unix2dos(int fd_r, int fd_w){
	
	int size = 0;
	char buffer = '\0';
	int spaceCount = 0; // counter of continuous white space

	while(read(fd_r, &buffer, 1)){
		switch(buffer){
			case ' ':
				spaceCount += 1;
				break;
			case '\t':
				spaceCount += 4; // 1 tab = 4 space
				break;
			case '\r':
				read(fd_r, &buffer, 1);
				if(buffer == '\n'){ // "\r\n"
					spaceCount = 0;
					write(fd_w, "\r\n", 2);
					size += 2;
					break;
				}
				lseek(fd_r, -2, SEEK_CUR); // not "\r\n": same with default case
				read(fd_r, &buffer, 1);
			default:
				size += spaceCount;
				while(spaceCount-- > 0)
					write(fd_w, " ", 1);
				spaceCount++; // set to 0
				write(fd_w, &buffer, 1);
				size += 1;
				break;
			case '\n':
				spaceCount = 0; // trim white space
				write(fd_w, "\r\n", 2); // "\n" -> "\r\n"
				size += 2;
				break;
		}
	}
	return size;

}

/* Dos format -> Unix format */
int dos2unix(int fd_r, int fd_w, const char* input){
	
	int size = 0;
	char buffer = '\0';
	char wBuffer[512]; // buffer for white spaces (has ' ' or '\t')
	int wBufSize = 0; // current size of wBuffer
	int isCode = isSourceCode(input);
	int spaceCount = 0; // counter of consecutive space

	while(read(fd_r, &buffer, 1)){
		switch(buffer){
			case ' ':
				wBuffer[wBufSize++] = ' ';
				break;
			case '\t':
				wBuffer[wBufSize++] = '\t';
				break;
			case '\r':
				read(fd_r, &buffer, 1);
				if(buffer == '\n'){ // "\r\n" -> "n"
					wBufSize = 0; // trim white space
					write(fd_w, "\n", 1);
					size += 1;
					break;
				}
				lseek(fd_r, -2, SEEK_CUR);  // not "\r\n": same with default case
				read(fd_r, &buffer, 1);
			default:
				if(isCode){ // if input is source code
					for(int i = 0; i < wBufSize; i++){
						switch(wBuffer[i]){
							case ' ':
								spaceCount += 1;
								if(spaceCount == 8){ // 8 space = 1 tab
									write(fd_w, "\t", 1);
									size += 1;
									spaceCount = 0;
								}
								break;
							case '\t':
								if(spaceCount < 8){
									size += spaceCount;
									while(spaceCount-- > 0)
										write(fd_w, " ", 1);
									spaceCount++; // set to 0
								}
								write(fd_w, "\t", 1);
								size += 1;
								break;
						}
					}
					size += spaceCount; // consider remaining space counts
					while(spaceCount-- > 0)
						write(fd_w, " ", 1);
					spaceCount++; // set to 0;
					wBufSize = 0;
				}
				else{ // if input is not a source code
					write(fd_w, wBuffer, wBufSize);
					size += wBufSize;
					wBufSize = 0;
				}
				write(fd_w, &buffer, 1);
				size += 1;
				break;
			case '\n':
				wBufSize = 0; // trim white space
				write(fd_w, "\n", 1);
				size += 1;
				break;
		}
	}
	return size;

}

/* if there is no new line in input file, just copy on output */
int copy(int fd_r, int fd_w){

	int size = 0;
	char buffer = '\0';

	while(read(fd_r, &buffer, 1)){
		write(fd_w, &buffer, 1);
		size += 1;
	}
	return size;

}
