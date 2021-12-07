#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

//unfinished: typeWords / reservedWords immediately following newlines behave strangely


const char* typeWords[] = {"void","const","char","int","float"};

const char* reservedWords[] =	{
					"sizeof","enum","case","default",
					"if","else","switch","while","do","for",
					"goto","continue","break","return","typedef",
				};

typedef struct
{
	int type;
	char str[100];
	int length;
} Token;

int ptr = 0;

char* input;
int input_length;

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
	TYPE_IDENTIFIER	//8
	
};

void resetPtr()
{
	ptr = 0;
}

int getPtr()
{
	return ptr;
}

void changePtr(int x)		// Only to be used for LL(x) parsing
{
	ptr = x;
}

int checkRegex(const char t)
{
	if(
	t == 'q' || t == 'w' || t == 'e' || t == 'r' || t == 't' || t == 'y' || 
	t == 'u' || t == 'i' || t == 'o' || t == 'p' || t == 'a' || t == 's' ||
	t == 'd' || t == 'f' || t == 'g' || t == 'h' || t == 'j' || t == 'k' ||
	t == 'l' || t == 'z' || t == 'x' || t == 'c' || t == 'v' || t == 'b' ||
	t == 'n' || t == 'm' || t == 'Q' || t == 'W' || t == 'E' || t == 'R' ||
	t == 'T' || t == 'Y' || t == 'U' || t == 'I' || t == 'O' || t == 'P' ||
	t == 'A' || t == 'S' || t == 'D' || t == 'F' || t == 'G' || t == 'H' ||
	t == 'J' || t == 'K' || t == 'L' || t == 'Z' || t == 'X' || t == 'C' ||
	t == 'V' || t == 'B' || t == 'N' || t == 'M' || t == '_' ||
	t == '\n'|| t == '\t'|| t == '\b'|| t == '\r')
	{
		return TYPE_CHAR;
	}
	else if(
	t == '0' ||t == '1' ||t == '2' ||t == '3' ||t == '4' ||t == '5' ||t == '6' ||
	t == '7' ||t == '8' ||t == '9')
	{
		return TYPE_INTEGER;
	}
	else if(t == '.' )
	{
		return TYPE_FLOAT;
	}
	else if(t == '\"' || t == '\'')
	{
		return TYPE_STRING;
	}
	else if(t == '/')
	{
		return TYPE_COMMENT;
	}
	else if(
	t == '+' || t == '=' || t == '>' || t == '*' || t == '&' || t == '|' || 
	t == '^' || t == '!' || t == '<' || t == '%' || t == '~' || t == '?' || 
	t == ',' || t == ';' || t == '{' || t == '}' || t == '(' || t == ')' || 
	t == ':' || t == '-' || t == '[' || t == ']' || t == '\\')
	{
		return TYPE_OPERATOR;
	}
}
 
int checkReserved(char* str, int same)
{
	int i;
	//printf("Debug: %s\n",str);
	for(i=0; i< 14; i++)
	{
		const char* typeword = typeWords[i%4];
		const char* reservedword = reservedWords[i];
		if(strcmp(typeword, str) == 0) // %size of typeWords to avoid seg fault
		{
			return TYPE_TYPE;
			
		}
		else if(strcmp(reservedword,str) == 0)
		{
			return TYPE_RESERVED;
			
		}
	}
	return same;
	
}



void accept(Token* token, int tempptr, int type)
{
	int i;
	int len = tempptr - ptr + 1;
	tempptr = ptr;
	token->length = len;  // current in length of ints not bytes
	//put token into token.str
	if((strlen(token->str) == 1) && token->str[0] == '\t')
	{
		printf("CHARACTER IN QUESTION %d",token->str[0]);
	}
	for(i = 0; i < len; i++)
	{
		if(i >= 99)
		{
			break;  // avoid error just in case
		}
		token->str[i] = input[tempptr];
		tempptr++;
		//printf("len: %d, token.str[i]: %c\n",len, token->str[i]);
	}
	if(token->str[i-1] == '\n')
	{
		token->str[i-1] = '\0';
	}
	else
		token->str[i] = '\0';
	token->type = checkReserved(token->str, type); //check if reserved or type
	
	
}

int identifyNextToken(Token* token)
{
	//Token token; // comment if needed
	int state;
	int tempptr = ptr;
	int done = 0; // break while loop if done
	char next = 'a'; //arbitrary
	int type; // if accept needs non-state
	char inp = input[tempptr];
	state = checkRegex(inp);
	
	//printf("%d\n",state); 
	
	while(!done)
	{	
		switch(state)
		{
			case TYPE_INTEGER: // 0
				tempptr ++;
				inp = input[tempptr];
				switch(checkRegex(inp))
				{
					case 0: // number
						done = 0;
						break;
					case 1: // .
						state = TYPE_FLOAT;
						break;
					default:
						accept(token, tempptr-1, state);
						done = 1;
						break;
				}
				break;
				
			case TYPE_FLOAT: // 1
				tempptr ++;
				inp = input[tempptr];
				switch(checkRegex(inp))
				{
					case 0: // number
						done = 0;
						break;
					default:
						accept(token,tempptr,state);	
						done = 1;
						break;
				}
				break;
				
			case TYPE_STRING:  // 2
				tempptr ++;
				inp = input[tempptr];
				switch(checkRegex(inp))
				{
					case 3:
						done = 0;  // pass
						state = TYPE_IDENTIFIER;
						break;
					default:
						accept(token, tempptr-1, state);
						done = 1;
						break;
				}
				break;
			
			case TYPE_CHAR:  // 3
				tempptr ++;
				inp = input[tempptr];
				switch(checkRegex(inp))
				{
					case 3:
						if(inp == '\n')
						{
							
							accept(token, tempptr-1, state); // or tempptr -1
							done = 1;
						}
						else
						{
							done = 0;  // pass
							state = TYPE_IDENTIFIER;
						}
						break;
					default:
						accept(token, tempptr-1, state);	//or tempptr -1
						done = 1;
						break;
				}
				break;
			case TYPE_COMMENT:  // 4
				tempptr ++;
				inp = input[tempptr];
				switch(inp)
				{
					case '/':
						while(inp != '\n') //while not new line
						{
							tempptr ++;
							inp = input[tempptr];
						}
						break;
					case '*': /* if multi-line, like this */
						while(next != '/')
						{
							if(inp == '*') //only check for '/' if '*'
							{
								next = input[tempptr+1];
							}
							tempptr ++;
							inp = input[tempptr];
						}
						break;
					
					default:
						accept(token, tempptr-1, state);
						done = 1;
						break;
				}
				break;
				
			case TYPE_OPERATOR:  // 7
				tempptr ++;
				inp = input[tempptr];
				switch(checkRegex(inp))
				{
					case TYPE_OPERATOR: // **, >=, <=, etc
						if(inp == '*' || inp == '=' || inp == '+' || inp == '-')
						{
							accept(token, tempptr, state);
						}
						else
						{
							accept(token, tempptr-1, state);
						}
						done = 1;
						break;
					default:
						accept(token, tempptr-1, state);
						done = 1;
						break;
				}
				break;
			
			case TYPE_IDENTIFIER: // 8
				tempptr ++;
				inp = input[tempptr];
				switch(checkRegex(inp))
				{
					case TYPE_INTEGER:
						done = 0;
						break;
					case TYPE_CHAR:
						if(inp == '\n')
						{
							//printf("here");
							accept(token, tempptr-1, state);
							done = 1;
						}
						else
							done = 0;
						break;
					case TYPE_STRING:
						accept(token, tempptr, TYPE_STRING);
						done = 1;
						break;
					default:
						accept(token, tempptr-1, state);
						done = 1;
						break;
				}
				break;
		}
	}
}


void next()
{
	Token token;
	while(input[ptr] == ' ' || input[ptr] == '\n' || input[ptr] == '\t')
	{
		ptr ++;
	}
	identifyNextToken(&token);
	ptr += token.length;
	while(input[ptr] == ' ' || input[ptr] == '\n' || input[ptr] == '\t')
	{
		ptr ++;
	}
	
}

int findColor(Token* token)
{
	if((token->type == 0) || (token->type == 1))		// float or integer
	{
		return 0;
	}
	else if((token->type == 2) || token->type == 3)	// string or char
	{
		return 1;
	}
	else							// anything else
	{
		return token->type - 2;
	}
}

void prettyPrint(Token* token)
{
	switch(findColor(token))
	{
		case 0:	// Numerical
			printf("\033[0;31m %s\033[0m",token->str);
			break;
		case 1:	// Alphabetical
			printf("\033[0;32m %s\033[0m",token->str);
			break;
		case 2:	// Comment
			printf("\033[0;33m %s\033[0m",token->str);
			break;
		case 3:	// Type
			printf("\033[0;34m %s\033[0m",token->str);
			break;
		case 4:	// Reserved
			printf("\033[0;35m %s\033[0m",token->str);
			break;
		case 5:	// Operator
			printf("\033[0;36m %s\033[0m",token->str);
			break;
		case 6:	// Identifier
			printf("\033[1;37m %s\033[0m",token->str);
			break;
		break;
	}
	
}

int matchtype(int type)
{
	Token token;
	identifyNextToken(&token);
	if(token.type != type)
		return 0;
	else
		return 1;
}

int match(char* string, int type)
{
	Token token;
	identifyNextToken(&token);
	if(token.type != type)
		return 0;
	else
	{
		if(strcmp(token.str,string) == 0)
			return 1; // 1 if true 0 if false
		else
			return 0;
		
	}
}

//deprecated
int LL(int x, char* str, int type)
{
	int oldptr = ptr;
	int ret;
	for(x; x>0; x--)
	{
		next();
	}
	if(match(str,type))
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	ptr = oldptr;	//put pointer back
	return ret;
}

void processFiles(char** argv)
{
	FILE* inputfile;
	input_length=0;
	char* infile = argv[1];
	const char* mode = "r";
	inputfile=fopen(infile, mode);
	//inputfile=fopen("inputfile.txt", mode);
	
	while(!feof(inputfile))
	{
		fgetc(inputfile);
		input_length++;
	}
	input_length--;
	input=(char*)malloc(input_length+1);
	rewind(inputfile);
	for(int i=0; i<input_length; i++)
	{
		input[i]=fgetc(inputfile);
		//printf("%c", input[i]);
		
	}
	input[input_length]='\0';
	fclose(inputfile);
}

void lexical()
{
	while(input[ptr] != input[input_length])
	{	
		Token token;
		identifyNextToken(&token);
		//printf("token:");
		prettyPrint(&token);
		//printf("\ntoken type: %d\n",token.type);
		next();
		//printf("\ndebug: type: %d, length: %d\n", token.type, token.length);
		
	}
	ptr = 0;
}


