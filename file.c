#include "file.h"

int STR_COUNT = 0;

void runFromFile(char* path) {
	int i = 0;
	STR_COUNT = 0;
	char symbol;
	Array* array;
	Expression* expression;
	char str[MAX_EXPRESSION];
	char* string;
	if (path == NULL) {
		error2("���� �� ������.");
		return;
	}
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		error2("���������� ������ 䠩�.");
		return;
	}
	symbol = getc(file);
	while (1) {
		str[i] = symbol;
		if (symbol == '\n' || symbol == EOF) {
			str[i] = '\0';
			if (i > MAX_EXPRESSION) {
				error2("���� �� ����� ���� ᫨誮� ����让.");
				STR_COUNT = 0;
				return;
			}
			if (i > 0) {
				string = (char*)malloc(sizeof(char) * (i+1));
				if (string == NULL) {
					error2("�訡�� �뤥����� �����.");
					STR_COUNT = 0;
					return;
				}
					for (int j = 0; j < i+1; j++) {
						string[j] = str[j];
					}
				STR_COUNT++;
				array = tokenize(string);
				if (array != NULL) {
					expression = parse(array);
					if (expression != NULL) {
						run(expression);
					}
				}
				free(string);
			}
			i = -1;
		}
		if (symbol == EOF) {
			break;
		}
		symbol = getc(file);
		i++;
	};
	fclose(file);
}