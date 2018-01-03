//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #1
// September 22, 2017
//
// Kim Kyu Yeon
// Sungkyunkwan University
//
//-----------------------------------------------------------

#define E_INPUT_NOT_EXIST	1
#define E_INPUT_NOT_ALLOWED	2
#define E_OUTPUT_EXIST	3
#define E_OUTPUT_NOT_ALLOWED	4

extern int convert(const char *input);
extern char getFileType(int fd);
extern int isSourceCode(const char* input);
extern int unix2dos(int fd_r, int fd_w);
extern int dos2unix(int fd_r, int fd_w, const char* input);
extern int copy(int fd_r, int fd_w);
