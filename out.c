/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "out.h"
#include <string.h>
#include "globals.h"

void error(const char* msg, int pos) {
		if (STR_COUNT == 0) {
		printf("%s\n", String);
		for (int i = 0; i < pos; i++)
			printf(" ");
		printf("^");
	}
	else {
		printf("(��ப� %d, ᨬ��� %d)\n", STR_COUNT, pos);
	}
	printf("\n�ந��諠 �訡��: %s\n", msg);
}

void error2(const char* msg) {
	printf("\n�ந��諠 �訡��: %s\n", msg);
}