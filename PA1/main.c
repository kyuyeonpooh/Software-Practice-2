//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #1
// September 13, 2017
//
// Jong-Won Park
// Embedded Software Laboratory
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include "convert.h"
#include <unistd.h>

int main(int argc, char** argv){
	
	int ret = 0;
    char error[] = "This shall never happen.\n";
	
	ret = convert(argv[1]);
	
    switch (ret){
        case 0:
            break;
        case E_INPUT_NOT_EXIST:
            ret = -1;
            break;
        case E_INPUT_NOT_ALLOWED:
            ret = -2;
            break;
        case E_OUTPUT_EXIST:
            ret = -3;
            break;
        case E_OUTPUT_NOT_ALLOWED:
            ret = -4;
            break;

        default:
            write(2, error, sizeof(error)-1);
            ret = 1;
    }

	return ret; 
	
}
