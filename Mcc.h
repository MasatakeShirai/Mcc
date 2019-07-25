//from parse.c
extern char *user_input;

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

Token *tokenize();

typedef enum{
	ND_NUM = 256,
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ,
	ND_NE,
	ND_L,
	ND_LE,
}NodeKind;

typedef struct Node Node;

struct Node{
	NodeKind kind;
	struct Node *lhs;
	struct Node *rhs;
	int val;
};

Node *expr();


//from codegen.c
void gen(Node *node);

//from container.c
void runtest();
void error(char *fmt,...);
void error_at(char *loc, char *fmt, ...);
int CompareStr(char* str);


//from main.c
char *user_input;

