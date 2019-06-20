//from parse.c
extern char *user_input;

void tokenize();

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

Node *expr();


//from codegen.c
void gen(Node *node);

//from container.c
void runtest();
void error(char *fmt,...);
void error_at(char *loc, char *msg);
int CompareStr(char* str);
