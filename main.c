#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Mcc.h"

int main(int argc, char **argv){
	if(argc != 2){
		error("error:incorrect number of arguments");
		return 1;
	};

	user_input = argv[1];

	if(CompareStr(user_input)){
		runtest();
		return 0;
	}

	tokenize();		//tokenizing
	Node *node = expr();	//parsing

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}

