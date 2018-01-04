void cmdchk(char* input);
int parse(char* args[], char* cmd, char* filein, char* fileout);
int evalRedir(const char* cmd);
void writeInvalid(void);
void init_args(char* args[]);
void free_args(char* args[]);

