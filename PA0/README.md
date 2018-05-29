# PA 0: Making my own string.h

## Introduction
Before jumping into unix programming, and for slight warming up, I implemented my own `<string.h>`. These source codes are also used in future project assginments due to the restriction that only system call function libraries must be used for implementation.

## Function implementation
**Conversions string to numeric formats**
- int **atoi2**(const char* str): **atoi** is actually implemented in **<stdlib.h>** not **<string.h>**. To avoid overlap of function name, I used **atoi2** instead of **atoi**.
- long **atol2**(const char* str): **atol** is actually implemented in **<stdlib.h>** not **<string.h>**. To avoid overlap of function name, I used **atol2** instead of **atol**.

**Conversions numeric formats to string**
- char* **int2str**(char* dest, int num): convert integer number to string (string is saved into **dest** buffer).

**String manipulation**
- char* **strcpy**(char* dst, const char* src)
- char* **strncpy**(char* dst, const char* src, size_t count)
- char* **strcat**(char* dst, const char* src)
- char* **strncat**(char* dst, const char* src, size_t count)
- char* **strdup**(const char* str): duplicate parameter string into dynamically allocated charater array, then return it.

**String examination**
- size_t **strlen**(const char* str)
- int **strcmp**(const char* lhs, const char* rhs)
- int **strncmp**(const char* lhs, const char* rhs, size_t count)
- char* **strchr**(const char* str, int ch)
- char* **strrchr**(const char* str, int ch)
- char* **strpbrk**(const char* str, const char* accept)
- char* **strstr**(const char* str, const char* substr)
- char* **strtok**(char* str, const char* delim)
- char* **strtok_r**(char* str, const char* delim, char** saveptr): instead of using static variable for token, **strtok_r** use **saveptr** parameter for next tokenization.

**Array manipulation**
- void* **memcpy**(void* dest, const void* str, size_t n)
- void* **memset**(void* dest, int ch, size_t count)

More information for each function can be found by `man` command:
```
$ man [function_name]
```

## How to use & Compilation
Type `#include "string_sw.h"` at the top of your source code to include this library.<br>
For compilation after including it into your code, type:
```
$ gcc -o [executable_output] [your_code.c] string_sw.c
```
For example, to compile `main.c`, which is in this folder, compilation command is:
```
$ gcc -o test main.c string_sw.c
```

## Warning
Yet functions are not fully well-implemented, these source codes may produce errors during execution, or may have different outputs compared to real function use. These problem would be constantly updated as future works.
