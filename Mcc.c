#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


//where to tokenize
//values representing the type of token
enum{
	TK_NUM = 256,	//integer token
	TK_EOF,		//EOF token
	TK_EQ,		//equal
	TK_NE,		//not equal
	TK_L,		//less
	TK_LE,		//less than or equal
	TK_G,		//greater
	TK_GE,		//greater than or equal
};

//type of token
typedef struct{
	int ty;		//type of token
	int val;	//if "ty" is "TK_NUM", "val" mewns value
	char *input;	//this is used to signal an error	
} Token;

//stored user input
char *user_input;

//stored torkenized user input
Token tokens[100];

void error(char *fmt,...){ 		//function that take variable-length argument
	va_list ap;			//convert any number of arguments into one veriable
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1); 			//exit(0) means abnormal termination
}

//report the location if the error
void error_at(char *loc, char *msg){
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s" ,pos,  "");
	fprintf(stderr, "^ %s\n" ,msg);
	exit(1);
}

//Tokenize the string stored in "inuser_input"
//and store it in "tokens"
void tokenize(){
	char *p = user_input;

	int i = 0;
	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p=='='){
			p++;
			if(*p='='){
				tokens[i].ty = TK_EQ;
				tokens[i].input = p;
				p++;
				i++;
			}
			continue;
		}

		if(*p=='!'){
			p++;
			if(*p='='){
				tokens[i].ty = TK_NE;
				tokens[i].input = p;
				p++;
				i++;
			}
			continue;
		}

		if(*p=='<'){
			p++;
			if(*p=='='){		
				tokens[i].ty = TK_LE;
				tokens[i].input = p;
				p++;
				i++;
				continue;
			}
			tokens[i].ty = TK_L;
			tokens[i].input = p;
			i++;
			continue;
		}

		if(*p=='>'){
			tokens[i].input = p;
			p++;
			if(*p=='='){		
				tokens[i].ty = TK_GE;
				tokens[i].input = p;
				p++;
				i++;
				continue;
			}
			tokens[i].ty = TK_G;
			i++;
			continue;
		}

		if(*p=='+' || *p=='-'){
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if(*p=='*' || *p=='/'){
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if(*p=='(' || *p==')'){
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if(isdigit(*p)){
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}

		error_at(p, "error:can not tokenize");
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
	
}


//where to parse
enum{
	ND_NUM = 256,
	ND_EQ,
	ND_NE,
	ND_L,
	ND_LE,
};

typedef struct Node{
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
}Node;

Node *new_node(int ty, Node *lhs, Node *rhs){
	Node *node = malloc(sizeof(Node));
	node->ty = ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

int pos = 0;
int consume(int ty){
	if(tokens[pos].ty != ty)
		return 0;
	pos++;
	return 1;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

Node *expr(){
	Node *node = equality();

	return node;
}

Node *equality(){
	Node *node = relational();

	for(;;){
		if(consume(TK_EQ))
			node = new_node(ND_EQ, node, relational());

		else if(consume(TK_NE))
			node = new_node(ND_NE, node, relational());

		else
			return node;
	}
}

Node *relational(){
	Node *node = add();

	for(;;){
		if(consume(TK_L))
			node = new_node(ND_L, node, add());

		else if(consume(TK_LE))
			node = new_node(ND_LE, node, add());

		else if(consume(TK_G))
			node = new_node(ND_L, add(), node);

		else if(consume(TK_GE))
			node = new_node(ND_LE, add(), node);
	
		else
			return node;
	}
}

Node *add(){
	
	Node *node = mul();

	for(;;){
		if(consume('+'))
			node = new_node('+', node, mul());

		else if(consume('-'))
			node = new_node('-', node, mul());
		
		else
			return node;
	}
}

Node *mul(){
	Node *node = unary();

	for(;;){
		if(consume('*'))
			node = new_node('*', node,unary());

		else if(consume('/'))
			node = new_node('/', node,unary());
		
		else
			return node;
	}
}

Node *unary(){

	if(consume('+'))
		return term();
	if(consume('-'))
		return new_node('-',new_node_num(0), term());

	return term();
}

Node *term(){
	if(consume('(')){
		Node *node = expr();
		if(!consume(')'))
			error_at(tokens[pos].input, 
				"error:open parenthesis has no corresponding closing parenthesis");
		return node;
	}

	if(tokens[pos].ty == TK_NUM)
		return new_node_num(tokens[pos++].val);

	error_at(tokens[pos].input, "this is not number or open parenthesis");
}

void gen(Node *node){
	if(node->ty == ND_NUM){
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->ty){
		case '+':
			printf("	add rax, rdi\n");
			break;
		case '-':
			printf("	sub rax, rdi\n");
			break;
		case '*':
			printf("	imul rdi\n");
			break;
		case '/':
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
		case ND_EQ:
			printf("	cmp rax, rdi\n");
			printf("	sete al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_NE:
			printf("	cmp rax, rdi\n");
			printf("	setne al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_L:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LE:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n");
			printf("	movzb rax, al\n");
	}

	printf("	push rax\n");

}



int main(int argc, char **argv){
	if(argc != 2){
		error("error:incorrect number of arguments");
		return 1;
	};

	user_input = argv[1];
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


