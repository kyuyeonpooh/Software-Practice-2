//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Code for PA #0
// September 17, 2017
//
// 2016314827 Kim Kyu Yeon
//
//-----------------------------------------------------------

#include "string_sw.h"
#include <stdlib.h>

int atoi2 (const char *str){

	int sign = 1;
	int num = 0;
	
	switch(*str){
		case '-':
			sign = -1;
		case '+':
			str++;
			break;
	}
	while(*str){
		num *= 10;
		num += *str - '0';
		str++;
	}
	return num * sign;

}

int atoi_port (const char* str){

	const char* start = str;

	while(*str){
		if(!(*str >= '0' && *str <= '9'))
			return -1;
		str++;
	}
	return atoi2(start);

}

long atol2 (const char *str){

	int sign = 1;
	long num = 0;
	
	switch(*str){
		case '-':
			sign = -1;
		case '+':
			str++;
			break;
	}
	while(*str){
		num *= 10;
		num += *str - '0';
		str++;
	}
	return num * (long) sign;

}

/* Conversions numeric formats to string */
char *int2str (char *dest, int num){
	
	int isNegative = 0;
	int index = 0;
	char temp[12];

	switch(num){
		case -2147483648:
			return strcpy(dest, "-2147483648");
		case 0:
			return strcpy(dest, "0");
		default:
			if(num < 0){
				isNegative = 1;
				num = -num;
			}
			while(num){
				temp[index++] = (num % 10) + '0';
				num /= 10;
			}
	}

	int i = 0;
	if(isNegative)
		dest[i++] = '-';
	while(index >= 0)
		dest[i++] = temp[--index];
	return dest;

}

/* String manipulation */
char *strcpy (char *dst, const char *src){
	
	size_t index = 0;

	while(*src){
		dst[index] = *src;
		index++, src++;
	}
	dst[index] = '\0';
	return dst;

}

char *strncpy (char *dst, const char *src, size_t count){
	
	size_t index = 0;

	while(index < count && *src != '\0'){
		dst[index++] = *src;
		src++;
	}
	while(index < count)
		dst[index++] = '\0';
	return dst;

}

char *strcat (char *dst, const char *src){
	
	size_t index = 0;

	while(dst[index])
		index++;
	while(*src){
		dst[index++] = *src;
		src++;
	}
	dst[index] = '\0';
	return dst;

}

char *strncat (char *dst, const char *src, size_t count){

	size_t index = 0;
	size_t counter = 0;	

	while(dst[index])
		index++;
	while(counter < count && *src){
		dst[index++] = *src;
		src++, counter++;
	}
	dst[index] = '\0';
	return dst;

}

char *strdup (const char *str){
	
	char* clone = (char*) malloc((strlen(str) + 1) * sizeof(char));
	
	if(clone)
		return strcpy(clone, str);
	return NULL;

}


/* String examination */
size_t strlen (const char *str){

	size_t len = 0;	

	while(*str)
		len++, str++;
	return len;

}

int strcmp (const char *lhs, const char *rhs){
	
	int dif;

	if(!lhs || !rhs)
		return -1;
	while(*lhs || *rhs){
		dif = *lhs - *rhs;
		if(dif)
			return dif;
		lhs++, rhs++;
	}
	return 0;

}

int strncmp (const char *lhs, const char *rhs, size_t count){

	size_t index = 0;
	int dif;
	
	if(!lhs || !rhs)
		return -1;
	while((lhs[index] || rhs[index]) && index < count){
		dif = lhs[index] - rhs[index];
		if(dif)
			return dif;
		index++;
	}
	return 0;

}

char *strchr (const char *str, int ch){
	
	while(*str){
		if(ch == *str)
			return (char*) str;
		str++;
	}
	return NULL;

}

char *strrchr (const char *str, int ch){

	char* p = NULL;

	while(*str){
		if(ch == *str)
			p = (char*) str;
		str++;
	}
	return p;

}

char *strpbrk (const char *str, const char *accept){

	while(*str){
		char* p = (char*) accept;
		while(*p){
			if(*str == *p)
				return (char*) str;
			p++;
		}
		str++;
	}
	return NULL;

}

int strcnt (const char* str, int ch){

	int count = 0;

	while(*str){
		if(ch == *str)
			count++;
		str++;
	}
	return count;

}

char *strstr (const char *str, const char *substr){
	
	char* p;
	char* p_return;
	char* p_sub;
	int isFound = 0;

	while(*str){
		p_return = p = (char*) str;
		p_sub = (char*) substr;
		while(*p && *p_sub){
			if(*p != *p_sub)
				break;
			p++, p_sub++;
		}
		if(!*p_sub) // checked whole substring
			isFound = 1;
		else if(!*p && *p_sub) // if substring length is greater than original
			return NULL;
		if(isFound)
			return p_return;
		str++;
	}
	return NULL;

}

int strstr_n (const char* str, const char* substr){

	int count = 0;
	int substrlen = strlen(substr);
	char* p = (char*) str;

	while((p = strstr(p, substr))){
		count++;
		if(*(p + substrlen) == '\0')
			break;
		p += substrlen;
	}
	return count;

}

char *strtok (char *str, const char *delim){

	static char *p;
	
	return strtok_r(str, delim, &p);

}

char *strtok_r (char *str, const char *delim, char **saveptr){
	
	/* str: pointer of the token */
	/* (*saveptr): current position of iterator */
	/* (**saveptr): address of *saveptr */
	if(str)
		*saveptr = str;
	else
		str = *saveptr;
	if(!str || !*str) // str is null-terminated or null-pointer
		return NULL;
	while(**saveptr){
		for(size_t i = 0; delim[i]; i++){
			if(**saveptr == delim[i] && *saveptr != str){
				**saveptr = '\0';
				(*saveptr)++;
				return str;
			}
			else if(**saveptr == delim[i] && *saveptr == str){
				**saveptr = '\0';
				str = *saveptr + 1;
				break;
			}
		}
		(*saveptr)++;
	}
	if(*str) // *saveptr reached at null-terminator
		return str;
	else
		return NULL;
}

/* return immediately even though token is empty */
char* strtok_i (char* str, const char* delim){

	static char* p;

	return strtok_i_r(str, delim, &p);

}

char *strtok_i_r (char *str, const char *delim, char **saveptr){
	
	/* str: pointer of the token */
	/* (*saveptr): current position of iterator */
	/* (**saveptr): address of *saveptr */
	if(str)
		*saveptr = str;
	else
		str = *saveptr;
	if(!str || !*str) // str is null-terminated or null-pointer
		return NULL;
	while(**saveptr){
		for(size_t i = 0; delim[i]; i++){
			if(**saveptr == delim[i]){
				**saveptr = '\0';
				(*saveptr)++;
				return str;
			}
		}
		(*saveptr)++;
	}
	if(*str) // *saveptr reached at null-terminator
		return str;
	else
		return NULL;
}


/* Character array manipulation */
void *memcpy (void *dest, const void *str, size_t n){
	
	char* p_dest = (char*) dest;
	char* p_str = (char*) str;

	for(size_t i = 0; i < n; i++)
		p_dest[i] = p_str[i];
	return dest;
	
}

void *memset (void *dest, int ch, size_t count){

	char* p_dest = (char*) dest;

	for(size_t i = 0; i < count; i++)
		p_dest[i] = ch;
	return dest;

}

