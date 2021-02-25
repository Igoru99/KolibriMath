/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "interpretator.h"

cvector* variables;
cvector* functions;

char* stringInput;

Result* runArithmetic(Arithmetic* arithmetic);
Result* runOperand(Operand* operand);
Result* runUnar(Operand* operand);
Result* runFunction(Function* function);

void getNumberFromStr(Result* result){
	int i = 0;
	int count = 0;
	enum ResultType type = ResultType_Int;
	while(stringInput[i] != '\0'){
		if(stringInput[i] == '-' && i == 0){
			i++;
			continue;
		}else if(isalpha(stringInput[i])){
			type = ResultType_Undefined;
			break;
		}else if(isdigit(stringInput[i])){
			i++;
			continue;
		}else if(stringInput[i] == '.' && count == 0){
			count++;
			type = ResultType_Float;
		}else{
			type = ResultType_Undefined;
			break;
		}
		i++;
	}
	if(i == 0){
		type = ResultType_Undefined;
	}
	result->type = type;
	if(type == ResultType_Int){
		(*result).value = (double)atoi(stringInput);
	}else if(type == ResultType_Float){
		(*result).value = (double)atof2(stringInput);
	}
}

Variable_* findVariable(char* name) {
	char* var;
	for (int i = 0; i < cvector_size(variables); i++) {
		var = (char*)((Variable_*)cvector_get(variables, i))->name;
		if (!strcmp(var, name)) {
			return cvector_get(variables, i);
		}
	}
	return NULL;
}

void deleteVariable(Variable_* variable) {
	char* var;
	for (int i = 0; i < cvector_size(variables); i++) {
		var = (char*)((Variable_*)cvector_get(variables, i))->name;
		if (!strcmp(var, variable->name)) {
			vector_delete(variables, i);
		}
	}
}

void addVariable(Variable_* variable) {
	cvector_push_back(variables, variable);
}

void print2(Result* result) {
	switch ((*(Result*)result).type) {
	case ResultType_Int:
		printf("%d", (int)(*(Result*)result).value);
		break;
	case ResultType_Float:
		printf("%f", (*(Result*)result).value);
		break;
	case ResultType_Undefined:
		printf("undefined");
		break;
		case ResultType_Spec:
		break;
	}
}

Result* runStr(Operand* operand){
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	char* data = (char*)((Operand*)operand)->data;
	result->str = data;
	result->type = ResultType_Str;
	return result;
}

Result* runVariable(Operand* operand) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	char* name = (char*)((Operand*)operand)->data;
	Variable_* variable = findVariable(name);
	if (variable == NULL || (*(Variable_*)variable).type == VarType_Undefined) {
		(*(Result*)result).value = 0;
		(*(Result*)result).type = VarType_Undefined;
		return result;
	}
	else if ((*(Variable_*)variable).type == VarType_Float){
		(*(Result*)result).value = (*(Variable_*)variable).value;
	}
	else if ((*(Variable_*)variable).type == VarType_Int) {
		(*(Result*)result).value = (int)(*(Variable_*)variable).value;
	}
	(*(Result*)result).type = variable->type;
	return result;
}

Result* runNumber(Const* number) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	if ((*(Const*)number).type == VarType_Float) {
		(*(Result*)result).type = VarType_Float;
		(*(Result*)result).value = *(double*)(*(Const*)number).data;
	}
	else if ((*(Const*)number).type == VarType_Int) {
		(*(Result*)result).type = VarType_Int;
		(*(Result*)result).value = *(double*)(*(Const*)number).data;
	}
	else {
		(*(Result*)result).type = VarType_Undefined;
		(*(Result*)result).value = 0;
	}
	return result;
}

Result* runOperand(Operand* operand) {
	switch ((*(Operand*)operand).type) {
	case OperandType_Arithmetic:
		return runArithmetic((Arithmetic*)operand->data);
	case OperandType_Function:
		return runFunction((Function*)operand->data);
	case OperandType_Number:
		return runNumber((Const*)operand->data);
	case OperandType_Unar:
		return runUnar((Operand*)operand->data);
	case OperandType_Variable:
		return runVariable((Operand*)operand);
		break;
	case OperandType_Str:
		return runStr((Operand*)operand);
		break;
	}
}

Result* runUnar(Operand* operand) {
	Result* result = runOperand(operand);
	if (result != NULL) {
		if ((*(Result*)result).type == VarType_Float) {
			(*(Result*)result).value = -(double)(*(Result*)result).value;
		}
		else if ((*(Result*)result).type == VarType_Int) {
			(*(Result*)result).value = -(int)(*(Result*)result).value;
		}
	}
	return result;
}

Result* runFunction(Function* function) {
	Result* result;
	result = (Result*)malloc(sizeof(Result));
    memset(stringInput, 0, sizeof(stringInput));
	if (result == NULL) {
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	result->type = VarType_Undefined;
	result->value = 0.0;
	if ((*(Function*)function).count == 0) {
		if (!(strcmp((*(Function*)function).name, "help"))) {
			printf("���᮪ ����㯭�� ������:\n");
			printf("help() - ������� ᢥ����� � ��������.\n");
			printf("about() - ������� ᢮��� � �ணࠬ��.\n");
			printf("lang() - 㧭��� ���ଠ�� � �몥 KolibriMath.\n");
			result->type = ResultType_Spec;
		}
		else if (!(strcmp((*(Function*)function).name, "about"))) {
			printf("����� �ணࠬ��: %s.\n", ver);
			printf("���ࠡ��稪: �������� �����.\n");
			printf("(C) 2021 ���.\n");
			printf("������ ᪠���� �⤥�쭮� ᯠᨡ� ������� Kolibri OS �� ������!\n");
			result->type = ResultType_Spec;
		}else if (!(strcmp((*(Function*)function).name, "lang"))) {
			printf("=== ��ࠢ�� � �몥 KolibriMath ===\n");
			printf("��� KolibriMath ���� �������㥬� �몮� � �������᪮� ⨯���樥�.\n");
			printf("�᭮���� �����祭�� �몠 �� ����� ������ - �஢������ ���⮢.\n");
			printf("��� �஧����� ࠡ���� � ��६���묨 ���� ⨯��: int, float. ��� �������� �� ��ᢠ������ ��⮬���᪨.\n");
			printf("�� ��ᢠ������ �ᯮ������ ᫥����� �������: <��६�����> = <��ࠦ����>.\n");
			printf("����᪠���� ����樨 \'*=\', \'+=\', \'-=\', \'/=\'.\n");
			printf("�������� ��६�����: del <��६�����>.\n");
			printf("����㯭� ���� ������ ����樨: *, /, +, -.\n");
			printf("�������� ����ᠭ�� ��ࠦ���� �१ ������ � ���� ��ப�.\n");
			printf("����㯭� �㭪樨: int(<�᫮>), float(<�᫮>), sqrt(<�᫮>), pow2(<�᫮>),\n");
			printf("pow(<�᫮>, <�᫮>), sin(<�᫮>), cos(<�᫮>), abs(<�᫮>), \nfile(<���� � 䠩��>),\n");
			printf("input(), input(<���᪠��� � �����>).\n");
			result->type = ResultType_Spec;
		}else if (!(strcmp((*(Function*)function).name, "input"))) {
		    printf("<");
		    gets(stringInput);
			if (stringInput == NULL) {
				result->type = ResultType_Spec;
		        printf("\n��ப� �� ����� ���� ᫨誮� �������.");
		        return result;
	        }
			getNumberFromStr(result);
		}
		else {
			result->type = ResultType_Spec;
			printf("\n�㭪�� \'%s\' � ⠪�� �᫮ ��㬥�⮢ �� �������.\n", function->name);
			return result;
		}
	}
	else if ((*(Function*)function).count == 1) {
		if (!(strcmp((*(Function*)function).name, "int"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
				(*(Result*)result).value = (int)(*(Result*)operand).value;
				result->type = VarType_Int;
				free(operand);
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'int\'.");
				return result;
			}
		}else if (!(strcmp((*(Function*)function).name, "float"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
				(*(Result*)result).value = (*(Result*)operand).value;
				result->type = VarType_Float;
				free(operand);
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'float\'.");
				return result;
			}
		}
		else if (!(strcmp((*(Function*)function).name, "sqrt"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
					if ((*(Result*)operand).value > 0) {
						(*(Result*)result).value = sqrt((*(Result*)operand).value);
						free(operand);
					}
					else {
						if(operand != NULL){
					        free(operand);
				        }
						result->type = ResultType_Spec;
						error2("�㭪�� \'sqrt\' �ਭ����� ����⥫�� ��㬥��.");
						return result;
					}
				result->type = VarType_Float;
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'sqrt\'.");
				return result;
			}
		}
		else if (!(strcmp((*(Function*)function).name, "sin"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
				(*(Result*)result).value = sin((*(Result*)operand).value);
				result->type = VarType_Float;
				free(operand);
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'sin\'.");
				return result;
			}
		}
		else if (!(strcmp((*(Function*)function).name, "cos"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
				(*(Result*)result).value = cos((*(Result*)operand).value);
				result->type = VarType_Float;
				free(operand);
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'cos\'.");
				return result;
			}
		}
		else if (!(strcmp((*(Function*)function).name, "pow2"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
				if ((*(Result*)operand).value != 0) {
					if (operand->type == VarType_Float) {
						(*(Result*)result).value = pow((*(Result*)operand).value, 2);
						result->type = VarType_Float;
						free(operand);
					}
					else {
						(*(Result*)result).value = (int)pow((*(Result*)operand).value, 2);
						result->type = VarType_Int;
						free(operand);
					}
				}
				else {
					if(operand != NULL){
					free(operand);
				    }
					result->type = ResultType_Spec;
					error2("�㭪�� \'pow2\' �ਭ����� �㫥��� ��㬥��.");
					return result;
				}
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'pow2\'.");
				return result;
			}
		}else if (!(strcmp((*(Function*)function).name, "abs"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type != VarType_Undefined && operand->type != VarType_Str) {
					if (operand->type == VarType_Float) {
						(*(Result*)result).value = abs((*(Result*)operand).value);
						result->type = VarType_Float;
						free(operand);
					}
					else {
						(*(Result*)result).value = (int)abs((*(Result*)operand).value);
						result->type = VarType_Int;
						free(operand);
					}
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'abs\'.");
				return result;
			}
		}
		else if (!(strcmp((*(Function*)function).name, "file"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type == VarType_Str) {
					runFromFile((char*)((Result*)operand)->str);
					free(((Result*)operand)->str);
					result->type = ResultType_Spec;
					free(operand);
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'file\'.");
				return result;
			}
		}else if (!(strcmp((*(Function*)function).name, "input"))) {
			Result* operand = runOperand(function->operands);
			if (operand != NULL && operand->type == VarType_Str) {
				printf("%s", operand->str);
			    gets(stringInput);
				if (stringInput == NULL) {
					free(operand);
				    result->type = ResultType_Spec;
		            error2("��ப� �� ����� ���� ᫨誮� �������.");
		           return result;
	            }
			    getNumberFromStr(result);
				free(operand);
			}
			else {
				if(operand != NULL){
					free(operand);
				}
				result->type = ResultType_Spec;
				error2("�� 㤠���� �믮����� �㭪�� \'input\'.");
				return result;
			}
		}
		else {
			result->type = ResultType_Spec;
			printf("\n�㭪�� \'%s\' � ⠪�� �᫮ ��㬥�⮢ �� �������.\n", function->name);
			return result;
		}
	}
	else if ((*(Function*)function).count == 2) {
	if (!(strcmp((*(Function*)function).name, "pow"))) {
	Result* operand0 = runOperand(function->operands);
	Result* operand1 = runOperand(function->operands + 1);
	if (operand0 != NULL && operand1 != NULL && operand0->type != VarType_Undefined && operand1->type != VarType_Undefined && operand0->type != VarType_Str && operand1->type != VarType_Str) {
		if ((*(Result*)operand0).value != 0 && (*(Result*)operand1).value != 0) {
				(*(Result*)result).value = pow((*(Result*)operand0).value, (*(Result*)operand1).value);
				result->type = VarType_Float;
				free(operand0);
				free(operand1);
		}
		else {
			if(operand0 != NULL){
				free(operand0);
			}
			if(operand1 != NULL){
				free(operand1);
			}
			result->type = ResultType_Spec;
			error2("�㭪�� \'pow\' �ਭ����� ���� ��� ��᪮�쪮 �㫥��� ��㬥�⮢.");
			return result;
		}
	}
	else {
		if(operand0 != NULL){
			free(operand0);
		}
		if(operand1 != NULL){
			free(operand1);
		}
		result->type = ResultType_Spec;
		error2("�� 㤠���� �믮����� �㭪�� \'pow\'.");
		return result;
	}
		}
	else {
		result->type = ResultType_Spec;
		printf("\n�㭪�� \'%s\' � ⠪�� �᫮ ��㬥�⮢ �� �������.\n", function->name);
	return result;
		}
	}
	else {
	result->type = ResultType_Spec;
	printf("�㭪�� \'%s\' � ⠪�� �᫮ ��㬥�⮢ �� �������.\n", function->name);
	return result;
	}
	return result;
}

Result* runOperation(Result* one, Result* two, enum OpType type) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	double res;
	double oneDouble;
	double twoDouble;
	int oneInt;
	int twoInt;
	short oneIsFloat = 0;
	short twoIsFloat = 0;
	short IsNullDivided = 0;
	if (one->type == VarType_Undefined || two->type == VarType_Undefined) {
		result->value = 0.0;
		result->type = VarType_Undefined;
		return result;
	}
	else if (one->type == VarType_Float) {
		oneDouble = (*(Result*)one).value;
		oneIsFloat = 1;
	}
	else {
		oneInt = (int)(*(Result*)one).value;
		oneIsFloat = 0;
	}
	if (two->type == VarType_Float) {
		twoDouble = (*(Result*)two).value;
		twoIsFloat = 1;
	}
	else {
		twoInt = (int)(*(Result*)two).value;
		twoIsFloat = 0;
	}
	if (type == OpType_Div) {
		if (oneIsFloat && twoIsFloat) {
			if(twoDouble != 0){
			res = oneDouble / twoDouble;
			}else{
				IsNullDivided = 1;
			}
		}
		else if (!oneIsFloat && twoIsFloat) {
			if(twoDouble != 0){
			res = oneInt / twoDouble;
			}else{
				IsNullDivided = 1;
			}
		}
		else if (oneIsFloat && !twoIsFloat) {
			if(twoInt != 0){
			res = oneDouble / twoInt;
			}else{
				IsNullDivided = 1;
			}
		}
		else {
			if(twoInt != 0){
			res = (double)oneInt / twoInt;
			}else{
			IsNullDivided = 1;
			}
		}
				if(IsNullDivided){
			result->type = VarType_Undefined;
			(*(Result*)result).value = 0;
		}else{
					(*(Result*)result).value = res;
		result->type = VarType_Float;
		}
		return result;
	}else if (type == OpType_Mul) {
		if (oneIsFloat && twoIsFloat) {
			res = oneDouble * twoDouble;
		}
		else if (!oneIsFloat && twoIsFloat) {
			res = oneInt * twoDouble;
		}
		else if (oneIsFloat && !twoIsFloat) {
			res = oneDouble * twoInt;
		}
		else {
			res = oneInt * twoInt;
		}
		(*(Result*)result).value = res;
		result->type = VarType_Float;
		return result;
	}else if (type == OpType_Sub) {
		if (oneIsFloat && twoIsFloat) {
			res = oneDouble - twoDouble;
		}
		else if (!oneIsFloat && twoIsFloat) {
			res = oneInt - twoDouble;
		}
		else if (oneIsFloat && !twoIsFloat) {
			res = oneDouble - twoInt;
		}
		else {
			res = oneInt - twoInt;
		}
		if (!oneIsFloat && !twoIsFloat) {
			(*(Result*)result).value = (int)res;
			result->type = VarType_Int;
		}
		else {
			(*(Result*)result).value = res;
			result->type = VarType_Float;
		}
		return result;
	}
	else if (type == OpType_Add) {
		if (oneIsFloat && twoIsFloat) {
			res = oneDouble + twoDouble;
		}
		else if (!oneIsFloat && twoIsFloat) {
			res = oneInt + twoDouble;
		}
		else if (oneIsFloat && !twoIsFloat) {
			res = oneDouble + twoInt;
		}
		else {
			res = oneInt + twoInt;
		}
        if (!oneIsFloat && !twoIsFloat) {
			(*(Result*)result).value = (int)res;
			result->type = VarType_Int;
		}
		else {
			(*(Result*)result).value = res;
			result->type = VarType_Float;
		}
		return result;
	}
}

Result* runArithmetic(Arithmetic* arithmetic) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	cvector* operands = (cvector*)malloc(sizeof(cvector));
		if(operands == NULL){
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	cvector* operations = (cvector*)malloc(sizeof(cvector));
		if(operations == NULL){
			free(operands);
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	cvector* expression = (cvector*)malloc(sizeof(cvector));
		if(expression == NULL){
			free(operands);
			free(operations);
		error2("�訡�� �뤥����� �����.");
		return NULL;
	}
	cvector_init(expression);
	cvector_init(operands);
	cvector_init(operations);
	for (int j = 0; j < (*(Arithmetic*)arithmetic).operandsCount; j++) {
		cvector_push_back(operands, runOperand((Operand*)arithmetic->operands + j));
		if ((*(Result*)cvector_get(operands, j)).type == VarType_Undefined) {
			result->type = VarType_Undefined;
			(*(Result*)result).value = 0;
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			return result;
		}
	}
	for (int j = 0; j < (*(Arithmetic*)arithmetic).operationsCount; j++) {
		cvector_push_back(operations, (enum OpType*)arithmetic->operations + j);
	}
	int i = 0;
	if (cvector_size(operands) != 1) {
		cvector_push_back(expression, cvector_get(operands, i));
		while (i < cvector_size(operands)-1) {
			cvector_push_back(expression, cvector_get(operations, i));
			cvector_push_back(expression, cvector_get(operands, i+1));
			i++;
		}
		i = 0;
		while (i < cvector_size(expression)-2) {
			if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Mul) {
				cvector_set(expression, i, runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Mul));
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}else if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Div) {
				cvector_set(expression, i, runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Div));
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else {
				i += 2;
			}
		}
		i = 0;
		while (i < cvector_size(expression) - 2) {
			if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Sub) {
				cvector_set(expression, i, runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Sub));
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Add) {
				cvector_set(expression, i, runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Add));
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
		}
	}
	else {
		result = cvector_get(operands, 0);
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		return result;
	}
	result = cvector_get(expression, 0);
	cvector_free(operands);
	free(operands);
	cvector_free(operations);
	free(operations);
	cvector_free(expression);
	free(expression);
	return result;
}

void runSetting(Part* part) {
	Variable_* variable = findVariable((char*)(*(Operand*)((Setting*)part->data)->variable).data);
	if (variable == NULL) {
		variable = (Variable_*)malloc(sizeof(Variable_));
		if (variable == NULL) {
			error2("�訡�� �뤥����� �����.");
			return;
		}
		variable->name = (char*)(*(Operand*)((Setting*)part->data)->variable).data;
	}
		Result* result = runOperand((Operand*)((Setting*)part->data)->source);
		if (result != NULL) {
			(*(Variable_*)variable).value = (*(Result*)result).value;
			if ((*(Result*)result).type == ResultType_Float) {
				variable->type = VarType_Float;
			}
			else if ((*(Result*)result).type == VarType_Int) {
				variable->type = VarType_Int;
			}
			else {
				variable->type = VarType_Undefined;
				(*(Variable_*)variable).value = 0;
			}
			addVariable(variable);
		}
}

void runDel(Part* part) {
	Variable_* variable = findVariable((*(Operand*)part->data).data);
	/*if (variable == NULL) {
		error2("��६����� � ⠪�� ������ �� �������.");
		return;
	}*/
	deleteVariable(variable);
}

void runNewLine(){
	printf("\n");
}

void runPrintStr(Part* part){
	printf("%s", part->data);
}

void runPart(Part* part) {
	Result* result;
	switch (part->type) {
	case PartType_Set:
		runSetting(part);
		break;
	case PartType_Del:
		runDel(part);
		break;
	case PartType_Arithmetic:
		result = runOperand((Operand*)part->data);
		if (result != NULL) {
			print2(result);
			free(result);
		}
		break;
	case PartType_NewLine:
		runNewLine();
		break;
	case PartType_Str:
		runPrintStr(part);
		break;
	}
}

short initTables() {
	variables = (cvector*)malloc(sizeof(cvector));
	if(variables == NULL){
		error2("�訡�� �뤥����� �����.");
		return 0;
	}
	cvector_init(variables);
	stringInput = malloc(sizeof(char)*MAX_EXPRESSION);
	memset(stringInput, 0, sizeof(stringInput));
	return 1;
}

void run(Expression* expression) {
	for (int i = 0; i < expression->count; i++) {
		runPart(expression->parts + i);
	}
	freeExpression(expression);
}

void freeTables() {
	cvector_free(variables);
	free(variables);
}