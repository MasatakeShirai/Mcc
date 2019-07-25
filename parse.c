#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Mcc.h"

//where to tokenize
//values representing the type of token

//type of token
struct Token{
	TokenKind kind;	//type of token
	Token *next;
	int val;	
	char *str;
	int len;	
};

Token *token;

bool consume(char op){
	if(token->kind != TK_RESERVED || 
	   strlen(op) != token->len ||
	   !memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

void expect(char op){
	if(token->kind != TK_RESERVED || 
	   strlen(op) != token->len ||
	   !memcmp(token->str, op, token->len))
		error_at(token->str, "It is not %c",op);
	token = token->next;
}

int  expect_num(){
	if(token->kind != TK_NUM)
		error(token->str, "It is not number");
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
	tok->len = strlen(str);
	tok->next = tok;
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
				cur = new_token(TK_EQ, cur, "==");
				p++;
			}
			continue;
		}

		if(*p=='!'){
			if(*p+1 == '='){
				cur = new_token(TK_NE, cur, "!=");
				p++;
			}
			continue;
		}

		if(*p=='<'){
			p++;
			if(*p=='='){		
				cur = new_token(TK_LE, cur, "<=");
				p++;
				continue;
			}
			cur = new_token(TK_L, cur, "<");
			continue;
		}

		if(*p=='>'){
			p++;
			if(*p=='='){		
				cur = new_token(TK_GE, cur, ">=");
				p++;
				continue;
			}
			cur = new_token(TK_G, cur, ">");
			continue;
		}

		if(*p=='+' || *p=='-'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(*p=='*' || *p=='/'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(*p=='(' || *p==')'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("error:can not tokenize");
	}
	
	new_token(TK_EOF, cur, p);
	return head.next;
}


//where to parse

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
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
			node = new_node(ND_ADD, node, mul());

		else if(consume('-'))
			node = new_node(ND_SUB, node, mul());
		
		else
			return node;
	}
}

Node *mul(){
	Node *node = unary();

	for(;;){
		if(consume('*'))
			node = new_node(ND_MUL, node,unary());

		else if(consume('/'))
			node = new_node(ND_DIV, node,unary());
		
		else
			return node;
	}
}

Node *unary(){

	if(consume('+'))
		return term();
	if(consume('-'))
		return new_node(ND_SUB, new_node_num(0), term());

	return term();
}

Node *term(){
	if(consume('(')){
		Node *node = expr();
		if(!consume(')'))
			error_at(token->str, 
				"error:open parenthesis has no corresponding closing parenthesis");
		return node;
	}

	if(token->kind == TK_NUM)
		return new_node_num(token[pos++].val);

	error_at(token->str, "this is not number or open parenthesis");
	exit(0);
}


