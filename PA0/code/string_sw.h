//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #0
// September 6, 2017
//
// Ha-yun Lee, Jong-won Park
// Embedded Software Laboratory
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include <stddef.h>

/* Conversions string to numeric formats */
extern int atoi2 (const char *str);
extern long atol2 (const char *str);

/* Conversions numeric formats to string */
extern char *int2str (char *dest, int num);

/* String manipulation */
extern char *strcpy (char *dst, const char *src);
extern char *strncpy (char *dst, const char *src, size_t count);
extern char *strcat (char *dst, const char *src);
extern char *strncat (char *dst, const char *src, size_t count);
extern char *strdup (const char *str);

/* String examination */
extern size_t strlen (const char *str);
extern int strcmp (const char *lhs, const char *rhs);
extern int strncmp (const char *lhs, const char *rhs, size_t count);
extern char *strchr (const char *str, int ch);
extern char *strrchr (const char *str, int ch);
extern char *strpbrk (const char *str, const char *accept);
extern char *strstr (const char *str, const char *substr);
extern char *strtok (char *str, const char *delim);
extern char *strtok_r (char *str, const char *delim, char **saveptr);

/* Character array manipulation */
extern void *memcpy (void *dest, const void *str, size_t n);
extern void *memset (void *dest, int ch, size_t count);
