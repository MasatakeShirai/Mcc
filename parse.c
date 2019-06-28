#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Mcc.h"

//where to tokenize
//values representing the type of token
typedef enum{
	TK_RESERVED,	//synbol token
	TK_NUM = 256,	//integer token
	TK_EOF,		//EOF token
	TK_EQ,		//equal
	TK_NE,		//not equal
	TK_L,		//less
	TK_LE,		//less than or equal
	TK_G,		//greater
	TK_GE,		//greater than or equal
}TokenKind;

typedef struct Token Token;

//type of token
struct Token{
	TokenKind kind;	//type of token
	Token *next;
	int vail;	
	char *str;	
} ;


bool consume(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op )
		return 0;
	token = token->next;
	return 1;
}

bool expect(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op )
		error("It is not %c",op);
	token = token->next;
}

bool expect_num(){
	if(token->kind != TK_NUM)
		error("It is not number");
	int val = token->val;
	token = token->next;
	return val;
}

bool eof(){
	return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->str = str;
	return tok;
}

Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;
		
	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p=='='){
			p++;
			if(*p=='='){
				tokens[i].ty = TK_EQ;
				tokens[i].input = p;
				p++;
				i++;
			}
			continue;
		}

		if(*p=='!'){
			p++;
			if(*p=='='){
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
			continue;i
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
			cur = new_token(TK_RESERVED, cur, p++)
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
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p, "error:can not tokenize");
	}
	
	new_token(TK_EOF, cur, p);
	return head.next;
}


//where to parse

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
	exit(0);
}


