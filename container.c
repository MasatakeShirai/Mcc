#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Mcc.h"


typedef struct{
	void **data;
	int capacity;
	int len;
}Vector;

Vector *new_vector(){
	Vector *vec = malloc(sizeof(Vector));
	vec->data =  malloc(sizeof(void *) * 16);
	vec->capacity = 16; 
	vec->len = 0;
	return vec;
}

void vec_push(Vector *vec, void *elem){
	if(vec->capacity == vec->len){
		vec->capacity *= 2;
		vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);	
	}
	vec->data[vec->len++] = elem;
}

void expect(int line, int expected, int actual){
	if(expected == actual)
		return;
	fprintf(stderr, "%d: %d expeced, but got %d\n"
			,line ,expected ,actual);
	exit(1);

}

void runtest(){
	Vector *vec = new_vector();
	expect(__LINE__,0,vec->len);

	for(int i=0; i<100; i++)
		vec_push(vec, (void *)i);

	expect(__LINE__, 100, vec->len);
	expect(__LINE__, 0, (long) vec->data[0]);
	expect(__LINE__, 50, (long) vec->data[50]);
	expect(__LINE__, 99, (long) vec->data[99]);

	printf("OK\n");
}

void error(char *fmt,...){ 		//function that take variable-length argument
	va_list ap;			//convert any number of arguments into one veriable
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1); 			//exit(0) means abnormal termination
}

//report the location if the error
void error_at(char *loc, char *fmt, ...){
	va_list ap;		
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s" ,pos,  "");
	fprintf(stderr, "^");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

int CompareStr(char* str){
	char* c = "-test";

	for(int i=0; i<5; i++){
		if(str[i] != c[i])
			return 0;
	}

	return 1;
}

