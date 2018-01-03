#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
	int src;
	int dst;
	char buf[1];
	int r_creator=0;

	src=open(argv[1],O_RDONLY,0755);
	char* p = strrchr(argv[1],'.')-1;
	p[0]='R';
	char* destP;
	destP=strdup(argv[1]);
	dst=open(destP,O_WRONLY|O_CREAT,0755);
	
	while(read(src,&buf,1)){
		write(dst,buf,1);
		r_creator++;
		if(r_creator%20==0)	write(dst,"\r",1);
	}
	return 0;
}
