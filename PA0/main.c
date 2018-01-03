#include <stdio.h>
#include "string_sw.h"
#include <limits.h>

int main(){
	char c[20] = "I LOVE YOU SO MUCH";
	char name[10] = " YEON JAE";
	char str1[20] = "HelloWorld!";
	char str2[20] = "LOVE";
	printf("strlen = %zu\n", strlen(c));
	char dest[20];
	printf("strncpy = %s\n", strncpy(dest, c, 6));
	printf("strncpy = %s\n", strncpy(str1, str2, 2));
	printf("strcpy = %s\n", strcpy(dest, c));
	printf("strcat = %s\n", strcat(c, name));
	printf("strncat = %s\n", strncat(c, name, 5));
	printf("strdup = %s\n", strdup(c));
	char a[10] = "abcd";
	char b[10] = "efgh";
	char c2[12] = "abcd";
	char d[10] = "abcde";
	char e[10] = "abcdf";
	char *p;
	printf("strcmp(abcd, efgh) = %d\n", strcmp(a, b));
	printf("strcmp(efgh, abcd) = %d\n", strcmp(b, c2));
	printf("strcmp(abcd, abcd) = %d\n", strcmp(a, c2));
	printf("strcmp(abcd, abcde) = %d\n", strcmp(a, d));
	printf("strncmp(abcd, abcde, 4) = %d\n", strncmp(a, d, 4));
	printf("strncmp(abcde, abcdf, 3) = %d\n", strncmp(d, e, 3));
	printf("strncmp(abcde, abcde, 10) = %d\n", strncmp(d, d, 10));
	printf("int2str = %s\n", int2str(p, -1234567890));
	printf("atoi(21123456) = %d\n", atoi2("21123456"));
	printf("atoi(INT_MIN) = %d\n", atoi2("-2147483648"));
	printf("atoi(-123) = %d\n", atoi2("-123"));
	printf("atoi(+123) = %d\n", atoi2("+123"));
	printf("strchr = %s\n", strchr(c, 'L'));
	printf("strrchr = %s\n", strrchr(c, 'O'));
	printf("strpbrk = %s\n", strpbrk(c, "AHYZ"));
	printf("strstr(YEON JAE) = %s\n", strstr(c, "YEON JAE"));
	printf("strstr(HIHI) = %s\n", strstr(c, "HIHI"));
	printf("strstr(I LOVE YOU SO MUCH YEON JAE YEON AAA) = %s\n", strstr(c, "I LOVE YOU SO MUCH YEON JAE YEON AAA"));
	printf("strstr( ) = %s\n", strstr(c, " "));
	printf("strstr(Y)  = %s\n", strstr(c, "Y"));
	char s[20] = "I love you.";
	char s2[20];
	printf("before memcpy: %s\n", s2);
	memcpy(s2, s, 20);
	printf("after memcpy(20): %s\n", s);
	memset(s, '*', 10);
	printf("after memset(10): %s\n", s);
	int n[3] = {10, 11, 12};
	printf("before memset: %d, %d, %d\n", n[0], n[1], n[2]);
	memset(n, 0, 12);
	printf("after memset(12): %d, %d, %d\n", n[0], n[1], n[2]);
	char sentence[] = "My name is, Kyu Yeon. Hello!!!! Yeon Jae! Hi.";
	char* token = strtok(sentence, " ,!.");
	printf("Token1: ");
	while(token){
		printf("%s ", token);
		token = strtok(NULL, " ,!.");
	}
	printf("\n");
	printf("Token2: ");
	char sentence2[] = " My name is, Kyu, Yeon. Hello!!! Yeon Jae! Hi.  Hi";
	token = strtok(sentence2, " ,!.");
	while(token){
		printf("%s ", token);
		token = strtok(NULL, " ,!.");
	}
	printf("\n");
	printf("Token3: ");
	char sentence3[] = "Hello, Yeonjae, Nice to Meet You. I love you!!!";
	token = strtok(sentence3, " ,!.");
	while(token){
		printf("%s ", token);
		token = strtok(NULL, " ,!.");
	}
	printf("\n");
}
