#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ 0
#define WRITE 1

#define TRUE 1
#define FALSE 0

#define VALID 1
#define INVALID 0

#define CMD_LS 11
#define CMD_MAN 12
#define CMD_GREP 13
#define CMD_SORT 14
#define CMD_AWK 15
#define CMD_BC 16
#define CMD_PATH 17
#define CMD_HEAD 21
#define CMD_TAIL 22
#define CMD_CAT 23
#define CMD_CP 24
#define CMD_MV 31
#define CMD_RM 32
#define CMD_CD 33
#define CMD_PWD 41
#define CMD_EXIT 42

#define MAX_CMD_LENGTH 128
#define MAX_CMD_NUM 32
#define MAX_ARG_NUM 32
#define MAX_ARG_LENGTH 64

#define UNDEFINED 0
#define NO_RDIR 1
#define LEFT_RDIR 2
#define RIGHT_RDIR 4
#define LEFT_RIGHT_RDIR 6
#define RIGHT_APND_RDIR 8

#define BUF_SIZE 100000

#define NO_PIPE 0
#define PIPE_FRNT 1
#define PIPE_MID 2
#define PIPE_BACK 3

