/*
 * test.c
 */

#include <7080S.H>
#include "libmemes.h"

#define	printf	((int(*)(const char*, ...))0x00007c7c)
#define	scanf	((int(*)(const char*, ...))0x00007cb8)

void main()
{
	char str[256];

	printf("MEMEs test\n");
	while(1) {
		printf("input strings -> ");
		scanf("%s", str);
		printf("%s\n", str);
	}
}
