#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	int type;
	char str[100];
	int length;
} Token;

enum
{
	TYPE_INTEGER,		//0
	TYPE_FLOAT,		//1
	TYPE_STRING,		//2
	TYPE_CHAR,		//3
	TYPE_COMMENT,		//4
	TYPE_TYPE,		//5
	TYPE_RESERVED,		//6
	TYPE_OPERATOR,		//7
	TYPE_IDENTIFIER		//8

};

void resetPtr();
int getPtr();
void changePtr();
int LL(int x, char* str, int type);
int identifyNextToken(Token* token);
int matchtype(int type);
int match(char* string, int type);
void next();
int doneWithInput();
int checkRegex(const char* token);
int checkReserved(char* str, int same);
void prettyPrint(Token* token);
void processFiles(char** argv);
void lexical();

