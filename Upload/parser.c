///Nicholas Almeder

//TODO characters as parameters are wrong, fix them.

#include "lexical.h"
#include <string.h>

#define MAX_SIZE 100

int expression();
int statement();
int functionOrDeclaration();

//passing values back from functions
float rval;
char* varName;
int pass = 1;
//int conditionalValue = 0;  //when a conditional evaluates to true, it skips all other statements in its scope
int entryPoint = 0;		// becomes 1 when main detected

int param_value[MAX_SIZE];
int param_ptr = 0;

int bracket_table_index[MAX_SIZE];	
int bracket_table_value[MAX_SIZE];
int bracket_table_ptr = 0;

int bracket_stack[MAX_SIZE];		// stack with depth 100
int bracket_stack_ptr = MAX_SIZE-1;	// stack pointer at top

int jump_stack[MAX_SIZE];
int jump_stack_ptr = MAX_SIZE - 1;

int lastAssignment;

typedef struct
{
	char name[MAX_SIZE];
	char type[20];
	int scope;	//position on stack ptr maybe?
	int scopeStart; //functions get a start to their scope for local variables
	int category; 	//0 variable, 1 function? could be char[] also if refactoring
	float value;
	int ptr;

} Symbol;


Symbol symbol_table[MAX_SIZE];
int symbol_table_ptr = 0;
Symbol currentSymbol;



void pushBracket()
{
	if(pass == 1)
	{
		int stk = getPtr();
		bracket_stack[bracket_stack_ptr] = stk;	// bracket_stack[bracket_stack_ptr -1];
		bracket_stack_ptr --;
	}
}

void popBracket()
{
	if(pass == 1)
	{
		int oldptr = getPtr();
		next();
		int pointingTo = getPtr();	// "<{> points to token after <}>"
		changePtr(oldptr);		//put it back
		bracket_table_value[bracket_table_ptr] = pointingTo;	
		//printf("BRACKET TABLE VALUE:%d\n",bracket_table_value[bracket_table_ptr]);		// ptr address of }
		bracket_stack_ptr ++;		// increment stack ptr to pop
		bracket_table_index[bracket_table_ptr] = bracket_stack[bracket_stack_ptr];  // ptr address of {
		bracket_table_ptr ++;		// increment table ptr after both { and } have been saved
	}
}

void copyString(Token* token, char mode)
{
	int i = 0;
	for(i; i<token->length; i ++)
	{
		if(mode == 'n')
			symbol_table[symbol_table_ptr].name[i] = token->str[i];
		else if(mode == 't')
			symbol_table[symbol_table_ptr].type[i] = token->str[i];
		//printf("%c\n",token->str[i]);
	}
}

int findBracket()
{
	int jumpTo = getPtr();
	int i = 0;
	while(jumpTo > bracket_table_value[i])	//when ptr is between { and }
	{
		i++;
		if(i == MAX_SIZE)
		{
			printf("BRACKET_TABLE_EXCEEDED\n");
			break;
		}
	}
	return bracket_table_value[i];
	//jumpTo = bracket_table_value[i];
}

void printSymbolTable()
{
	int i;
	printf("\n\n Symbol Table: \n");
	for(i = 0; i < symbol_table_ptr; i++)
	{
		if(symbol_table[i].category)
			printf("variable ");
		else
			printf("function ");
		
		printf("%s %s ",symbol_table[i].type, symbol_table[i].name);
		
		if(symbol_table[i].scope == 0)
			printf("global\n");
		else
			printf("local to %d\n",symbol_table[i].scope);
		
		printf("PTR WAS AT %d WHEN DECLARED\n",symbol_table[i].ptr); //back to ptr
		if(symbol_table[i].scopeStart > 0)
		{
			printf("symbol scope starts at %d\n",symbol_table[i].scopeStart);
		}
		printf(" value of: %f\n", symbol_table[i].value);
	}
	printf("\n");
}

int findSymbol(char* str)
{
	if(entryPoint)
	{
		int checkPtr = getPtr();
		int checkBracket = findBracket();
		//int i = symbol_table_ptr+1;
		int i = 99;
		while(1)	//for now, goes through every element
		{
			if(strcmp(symbol_table[i].name,str) == 0 && symbol_table[i].scope <= checkPtr && checkPtr < findBracket())
			{
				
				//printf("SCOPE HERE: %d ",symbol_table[i].scope);
				break;
			}
			else
			{
				if(i > 0)
					i--;//if local,going backwards will find the variable faster than going up from 0
				else
				{
					printf("symbol %s not found: len %ld\n",str, strlen(str));
					//printSymbolTable();
					return 0;
					break;
				}
			}
		}
		currentSymbol = symbol_table[i];
		return 1;
	}
}

// It's not D.R.Y code, but it works by magic. So had to keep it this way. Global variable problems
int changeVar()
{	
	if(entryPoint)
	{
		int checkPtr = getPtr();
		int checkBracket = findBracket();
		int i = bracket_table_ptr+1;
		while(1)	//for now, goes through every element
		{
			if(strcmp(symbol_table[i].name,varName) == 0 && symbol_table[i].scope <= checkPtr && checkPtr < findBracket())
			{
				
				//printf("\nSCOPE HERE: %d ",symbol_table[i].scope);
				break;
			}
			else
			{
				if(i > 0)
					i--;//if local,going backwards will find the variable faster than going up from 0
				else
				{
					printf("symbol %s not found: len %ld\n",varName, strlen(varName));
					return 0;
					break;
				}
			}
		}
		symbol_table[i].value = rval;
	}
}

void addSymbol(int cat)
{
	if(pass == 1)
	{
		symbol_table[symbol_table_ptr].category = cat;	// category
		symbol_table_ptr ++;
	}
	
}



int evaluate(int conditionalValue)	//if statement is true, or else, run code. If false, jump to next bracket 
{
	if(entryPoint)
	{		
		int jumpTo = findBracket();
		if(conditionalValue == 0)
		{
			if(lastAssignment == 1)
			{
				//printf("EVALUATED TO TRUE :)\n");
				//conditionalValue = 1;
				return 1; 	// continue parsing
			}
			else if(lastAssignment == 0)
			{
				//printf("%d\n",getPtr());
				//printf("EVAL TO FALSE :(\n");
				changePtr(jumpTo);	
				//printf("%d\n",getPtr());
				return 0;	// end parsing
			}
			else
			{
				//printf("%d\n",getPtr());
				//printf("ELSE \n");
				changePtr(jumpTo);	
				//printf("%d\n",getPtr());
				return 0;	// end parsing
			}
		}
		else
		{
			printf("CONDITIONAL IS 1\n");
			changePtr(jumpTo);
			Token token;
			identifyNextToken(&token);
			return 0;	// end parsing
		}
	}
	else
	{
		return 1;
	}
}


void checkMain()	// if the function being saved to the symbol table is "main", entryPoint = 1
{
	Token token;
	identifyNextToken(&token);
	if(pass == 2)
	{
		if(token.str[0] == 'm' && token.str[1] == 'a' && token.str[2] == 'i' && token.str[3] == 'n')
		{
			entryPoint = 1;
			printf("\nENTRYPOINT FOUND\n");
		}
	}
}


void popLocalVariables()
{
	int i = symbol_table_ptr + 1;
	while(i > 0)
	{
		
		if(symbol_table[i].scope == 0)
			break;
		printf("POPPING %s %f\n",symbol_table[symbol_table_ptr].name, symbol_table[symbol_table_ptr].value);
	}
}





// "do it yourself" mentality
float generateNumber(Token* token, int floating)
{
	
	int i = 0;		//iterator
	float v = 1.0; 		//value
	int decimal = 0; 	//boolean
	float total = 0.0;	//I guess I can't delcare in switch cases
	int tot = 0;		//I guess I can't delcare in switch cases
	//printf("token: %s floating? %d\n",token->str,floating);
	switch(floating)	//true or false boolean
	{
		case 0:	//false, int, uses tot
			while(token->str[i] != '\0')
			{
				tot = tot * 10 + token->str[i]-'0';
				i++;
			}
			return tot;
			break;
		case 1:	//true, float, uses total
			
			while(token->str[i] != '\0')
			{
				
				switch(token->str[i])
				{
					case '.':
						i ++;
						decimal = 1;
						printf(".\n");
						break;
					case ' ':
						i ++;
						break;
					default:
						if(decimal)
							v *= 10.0;
						total = total * 10.0 + token->str[i]-'0';
						//printf("total: %d\n",token->str[i]-'0');
						i ++;
						break;
						
						
				}
			}
			total = total/v;
			return total;
			break;
		default:	//error
			return -1;
			break;
	}
	
}

int pushJump()	
{
	if(entryPoint)
	{
		jump_stack[jump_stack_ptr] = getPtr();
		printf("jump stack info: jumpstack: %d, jumpto %d\n",jump_stack_ptr, jump_stack[jump_stack_ptr]);
		if(jump_stack_ptr > 90)	//TODO return this to 0
		{
			jump_stack_ptr --;
			return 1;
		}
		else
		{
			printf("\nstack overflow\n");
			return 0;
		}
	}
	
}
void popJump()
{
	if(entryPoint)
	{
		if(jump_stack_ptr < (MAX_SIZE -1))
		{
			jump_stack_ptr ++;
			changePtr(jump_stack[jump_stack_ptr]);
			//printf("MOVING TO %d\n",jump_stack[jump_stack_ptr]);
			next();
		}
	}
}

void panicDump()
{
	for(int i = 99; i > jump_stack_ptr; i --)
	{
		popJump();
	}
}



void gotoFunc(char* function)
{
	if(entryPoint)
	{
		char* fix = varName;
		int retPoint = getPtr();
		if(pushJump())
		{
			if(findSymbol(function))
			{
				changePtr(currentSymbol.ptr);
				functionOrDeclaration();	
				popJump();
				varName = fix;
			}
			else
			{
				printf("\nCANT find symbol %s\n",function);
			}
		}
		else{
			printf("NOT PUSHJUMP\n");
		}
	}
}

void handleFunc(char* function)
{
	//printf("CSYMBOL: %s\n",currentSymbol.name);
	//int saveCond = conditionalValue;
	//conditionalValue = 0;
	Symbol currentFunc = currentSymbol;
	next();
	Token token;
	param_ptr = 0;
	while(!match(")",TYPE_OPERATOR))
	{
		identifyNextToken(&token);
		if(expression())
		{	
			findSymbol(token.str);
			//printf("EXPRESSION HERE: %s %f\n",currentSymbol.name,currentSymbol.value);
			//rval = currentSymbol.value;
			//printf("currentSymbol.value %f\n",rval);
			
			param_value[param_ptr] = rval; //currentSymbol.value;
			
			for(int j = 0; j < 3; j++)
			{
				printf("\n param: %d\t",param_value[j]);
			}
			
			identifyNextToken(&token);
			printf("%s\n",token.str);
			
		}
		if(match(",", TYPE_OPERATOR))
		{
			param_ptr ++;
			next();
		}
	}
	//next();
	printf("found ')' for function\n");
	if(entryPoint)
	{
		gotoFunc(function);
	//	conditionalValue = saveCond;
	}
	else
		next();
}

//1 if success, 0 if failure
int primaryExpression()
{
	Token token;
	identifyNextToken(&token);
	if(
	matchtype(TYPE_IDENTIFIER) || matchtype(TYPE_STRING) || matchtype(TYPE_CHAR))
	{	
		
		if(matchtype(TYPE_CHAR))
		{
			printf("parsing Char: ");
			rval = token.str[0];
			printf("%c or %d\n",token.str[0],token.str[0]);
		}
		else	// shortcut, may want to call expression() here
		{
			printf("parsing identifier %s\n",token.str);
			findSymbol(token.str);
			//printSymbolTable();
			printf("%s %f\n",currentSymbol.name, currentSymbol.value);
			rval = currentSymbol.value;
		}
		next();
		if(match("(",TYPE_OPERATOR))
		{
			printf("A FUNCTION WAS CALLED\n");
			handleFunc(token.str);
		}
	}
	else if(matchtype(TYPE_FLOAT)) 
	{
		printf("parsing float\n");
		rval = generateNumber(&token, 1);
		printf("%f\n", rval);
		next();	
	}
	else if(matchtype(TYPE_INTEGER))
	{
		printf("parsing int: ");
		rval = generateNumber(&token, 0);
		printf("%f\n", rval);
		next();
	}
	else if(match("(",TYPE_OPERATOR))
	{
		next();
		expression();
		//next();	//this might be causing an error somewhere?
		
	}
	else if(match(")",TYPE_OPERATOR))
	{
		//next();
	}
	else if(match(";",TYPE_OPERATOR))
	{
		return 1;
	}
	else
	{
		printf("syntax error in primary expression %s\n",token.str);
		return 0;
	}
	return 1;
}


//primary expression, not postfix
// { <unary_operator> }? <primary expression>
//+ | - etc
int unaryExpression()
{
	int op = 1;
	int oldval = 0;
	if(match("+",TYPE_OPERATOR))
	{
		printf("parse unary +\n");
		oldval = rval;
		next();
	}
	else if (match("-",TYPE_OPERATOR))
	{
		printf("parse unary -\n");
		op = -1;
		oldval = rval;
		next();
	}else if (match("!", TYPE_OPERATOR))
	{
		printf("parse unary !\n");
		op = 0;
		oldval = rval;
		next();
	}
	else
	{
		//next();
		//printf("Not a valid unary operator\n");
		//return 0;
	}
	if(!primaryExpression())
		return 0;
	
	//modify rval based on unary operator or lack thereof
	if(op == 0)
	{
		if(rval == 0)
			rval = 1;
		else
			rval = 0;
	}
	else
	{
		rval = rval * op;
	}
	//printf("unary rval: %f\n",rval);
	return 1;
	
}

int multiplicativeExpression()
{
	int op = 0;
		
	float oldval = 1;	
	if(match("*",TYPE_OPERATOR))
	{
		oldval = rval;
		printf("parse mult *\n");
		next();
	}
	else if (match("/",TYPE_COMMENT))	//technically forward slash is a comment
	{
		oldval = rval;
		printf("parse mult / \n");
		op = 1;		
		next();
	}else if (match("%", TYPE_OPERATOR))
	{
		oldval = rval;
		printf("parse mult %c \n",'%');
		op = 2;			
		next();
	}
	if(!unaryExpression())
		return 0;
	
	if(op == 2)
		rval = (int)oldval % (int)rval;
	else if(op)
		rval = oldval / rval;
	else
		rval = oldval * rval;
		
	printf("result of multiplicative parse: %f\n",rval);
	if(match("*",TYPE_OPERATOR) || match("/",TYPE_COMMENT) || match("%",TYPE_OPERATOR))
		multiplicativeExpression();
		
	return 1;
	
}


int additiveExpression()
{
	int op = 1;
	float oldval = 0;	
	if(match("+",TYPE_OPERATOR))
	{
		oldval = rval;
		printf("parse ad +\n");
		next();
	}
	else if (match("-",TYPE_OPERATOR))	
	{
		oldval = rval;
		printf("parse ad -\n");
		op = -1;		
		next();
	}
		
	if(!multiplicativeExpression())
		return 0;
	
	rval = oldval + op*rval;
		
	printf("result of additive parse: %f\n",rval);
	if(match("+",TYPE_OPERATOR) || match("-",TYPE_COMMENT))
		additiveExpression();
		
	return 1;
	
}

int relationalExpression()		//Question: this will make all previous parsings 1 or 0
					//Do we only call this particular times?
{
	int op = 6;			//by default, 0!=1
	float oldval = 0;	
	if(match("<",TYPE_OPERATOR))
	{
		oldval = rval;
		op = 0;
		printf("parse relation <\n");
		next();
	}
	else if (match(">",TYPE_OPERATOR))	
	{
		oldval = rval;
		op = 1;
		printf("parse relation >\n");		
		next();
	}
	else if (match(">=",TYPE_OPERATOR))	
	{
		oldval = rval;
		op = 2;
		printf("parse relation >=\n");		
		next();
	}
	else if (match("<=",TYPE_OPERATOR))	
	{
		oldval = rval;
		op = 3;
		printf("parse relation <=\n");		
		next();
	}
	else if (match("!=",TYPE_OPERATOR))	
	{
		oldval = rval;
		op = 4;
		printf("parse relation !=\n");		
		next();
	}
	else if (match("==",TYPE_OPERATOR))	
	{
		oldval = rval;
		op = 5;
		printf("parse relation ==\n");		
		next();
	}
		
	if(!additiveExpression())
		return 0;
	
	if(op == 0)
		rval = oldval < rval;
	else if(op == 1)
		rval = oldval > rval;
	else if(op == 2)
		rval = oldval >= rval;
	else if(op == 3)
		rval = oldval <= rval;
	else if(op == 4)
		rval = oldval != rval;
	else if(op == 5)
		rval = oldval == rval;
		
	printf("result of relational parse: %f\n",rval);
	lastAssignment = rval;
	if(
	match("<",TYPE_OPERATOR) || match(">",TYPE_OPERATOR) ||
	match(">=",TYPE_OPERATOR) || match("<=",TYPE_OPERATOR) ||
	match("!=",TYPE_OPERATOR) || match("==",TYPE_OPERATOR)
	)
	{
		relationalExpression();
	}
	return 1;
}

int logicalExpression()		
{
	int op = 2;
	float oldval = 2;	
	if(match("&&",TYPE_OPERATOR))
	{
		oldval = rval;
		printf("parse logical &&\n");
		op = 0;
		next();
	}
	else if (match("||",TYPE_OPERATOR))	
	{
		oldval = rval;
		printf("parse logical ||\n");	
		op = 1;	
		next();
	}
		
	if(!relationalExpression())
		return 0;
	
	if(op == 2);	//do nothing
	else if(op)
	{
		rval = oldval || rval;
	}
	else if(!op)
	{
		rval = oldval && rval;
	}
	
	printf("result of logical parse: %f\n",rval);
	if(match("&&",TYPE_OPERATOR) || match("||",TYPE_COMMENT))
		logicalExpression();
		
	return 1;
	
}


int expression()		
{
	int op = 3;
	float oldval = 0;	
	if(matchtype(TYPE_IDENTIFIER))
	{
		Token token;
		identifyNextToken(&token);
		varName = token.str;
		primaryExpression();	
		if(match("=",TYPE_OPERATOR))
		{
			oldval = rval;
			printf("parse expression =\n");	
			op = 0;
			next();
		}
		else if(match("+=",TYPE_OPERATOR))
		{
			oldval = rval;
			printf("parse expression +=\n");
			op = 1;
			next();
		}
		else if(match("-=",TYPE_OPERATOR))
		{
			oldval = rval;
			printf("parse expression -=\n");
			op = 2;
			next();
		}
	}
		
	if(!logicalExpression())
		return 0;
	
	if(op == 2)
		rval = oldval -= rval;
	else if(op)
	{
		rval = oldval += rval;
	}
	
	if(op != 3)
	{
		//printf("changing VAR \n");
		changeVar();
	}
	
	printf("result of expression parse: %f\n",rval);
	if(match("+=",TYPE_OPERATOR) || match("-=",TYPE_COMMENT) || match("=",TYPE_COMMENT))
		logicalExpression();
		
	return 1;
	
}

int expressionStatement()
{
	expression();
	if(match(";",TYPE_OPERATOR))
	{
		next();
		printf("end of statement\n");
		printf("return value: %f\n", rval);
		return 1;
	}
	else if(rval == 10.0)  //line feed	//TODO: temporary solution, fix this later
	{
		next();
		return 1;
	}
	else	
	{
		printf("\nERROROROROR error in expressionStatement()\n");
		return 0;
	}
}
/*<jump-statement> ::= continue ;
			| break ;
			| return { <expression> }? ;
*/
int jumpStatement()	//Matchtype + switch:case would probably work better here, if refactoring.
{
	if(match("continue",TYPE_RESERVED))
	{
		next();
		if(match(";",TYPE_OPERATOR))
		{
			printf("Found continue\n");
			next();
			return 1;
		}
		else
		{
			printf("expected ';' \n");
			return 0;
		}
		
	}
	else if(match("break",TYPE_RESERVED))
	{
		next();
		if(match(";",TYPE_OPERATOR))
		{
			printf("Found break \n");
			next();
			return 1;
		}
		else
		{
			printf("expected ';' \n");
			return 0;
		}
	}
	else if(match("return",TYPE_RESERVED))
	{
		Token token;
		next();
		if(!match(";",TYPE_OPERATOR))	//no <;>
		{
			//currentSymbol.value = rval;
			if(!expressionStatement())	//not expression statement either.
			{
				printf("Expected ';' or expression statement\n");
				return 0;
			}
			else				//is <expression>;
			{
				printf("%s\n",varName);
				findSymbol(varName);
				currentSymbol.value = rval;
				printf("currentSymbol::%s %f\n",currentSymbol.name, currentSymbol.value);
				printf("Found return <expression>;\n");
				if(entryPoint)
				{
					//panicDump();	//TODO: this one
					//printf("NEW PTR: %d\n",getPtr());
					//next();
				}
				
				return 1;
			}
		}
		else				//does have ; just no <expression>
		{
			printf("Found return;\n");
			if(entryPoint)
			{
				changePtr(findBracket());
				//next();
			}
			//next();
			return 1;
		}
	}
	else					//Doesn't match any reserved words
	{
		printf("expected reserved word\n");
		return 0;
	}
	
}

/*
	<while-statement> ::= while ( <expression> ) <statement>
*/
int whileStatement()
{
	int jumpBack = getPtr();	//save the location to jump back to
	if(match("while",TYPE_RESERVED))
	{
		next();
		if(match("(",TYPE_OPERATOR))
		{
			next();
			if(expression())
			{
				if(!rval)	//if rval false
				{
					changePtr(findBracket());
					printf("found while conditional\n");
					return 1;
				}
				if(match(")",TYPE_OPERATOR))
				{
					next();
					if(statement())
					{
						if(entryPoint)
						{
							changePtr(jumpBack);
							whileStatement();
						}
						else
						{
							return 1;
						}
					}
					else
					{
						printf("parsed while with no statement\n");
						return 0;
					}
				}
				else
				{
					printf("expected ')' \n");
					return 0;
				}
			}
			else
			{
				printf("expected expression\n");
				return 0;
			}
		}
		else
		{
			printf("expected '(' \n");
			return 0;
		} 
	}
	else
	{
		printf("expected 'while'\n");
		return 0;
	}
}

/*
<for-statement> ::= for ( {<expression>}? ; {<expression>}? ; {<expression>}? ) <statement>
*/
int forStatement()
{
	if(match("for",TYPE_RESERVED))
	{
		next();
		if(match("(",TYPE_OPERATOR))
		{
			next();
			int i = 0;
			for(i; i< 2; i++)				//Ironic forloop for counting semicolons
			{
				if(!match(";",TYPE_OPERATOR))		//if no semiclon found
				{
					if(!expressionStatement())	//and not expression statement
					{
						printf("expected expression-statement or ';'\n");
						return 0;
					}
				}
				else					// if just semicolon found
				{
					next();
				}
				
			}
			
			if(match(")",TYPE_OPERATOR))	//Either ')'
			{
				next();
				if(statement())
				{
					printf("found for statement\n");
					return 1;
				}
				else
				{
					printf("parsed 'for' without statement\n");
					return 0;
				}
			}
			else if(expression())		//Or expression
			{
				if(match(")",TYPE_OPERATOR))
				{
					next();
					if(statement())
					{
						printf("found for statement with final expression\n");
						return 1;
					}
					else
					{
						printf("parsed 'for' without statement\n");
						return 0;
					}
				}
				else
				{
					printf("expected ')'\n");
					return 0;
				}
			}
			else
			{
				printf("missing ')' or found ';' when none expected\n");
				return 0;
			}
		}
		else
		{
			printf("expected '(' \n");
			return 0;
		}
	}
	else
	{
		printf("expected 'for'\n");
		return 0;
	}
}


//<compound-statement> ::= {  { <declaration> | <statement> }*   }
int compoundStatement()
{
	if(!match("{",TYPE_OPERATOR))
	{
		return 0;
		
	}
	pushBracket();
	next();
	while(1)
	{
		Token token;
		identifyNextToken(&token);
		if(match("{",TYPE_OPERATOR))
		{
			compoundStatement(); 
			
		}
		if(match("}",TYPE_OPERATOR))
		{
			popBracket();
			printf("end of scope\n");
			popLocalVariables();
			next();
			return 1;
			break;
		}
		if(matchtype(TYPE_TYPE))
		{
			functionOrDeclaration();
		}
		else if(!statement())
		{
			printf("Not a statement, function, or declaration\n");
			return 0;
			break;
		}
		
		printf("\n");
	}
}



/*
<if-statement> ::= if ( <expression> ) <statement> { else <statement> }?
*/

int conditionalStatement(int conditional)	
{
	if(match("if",TYPE_RESERVED))
	{
		next();
		if(match("(",TYPE_OPERATOR))
		{
			next();
			if(expression())
			{
				conditional = evaluate(conditional);
				if(!conditional)		//If ptr jumps
				{
				
					if(match("else",TYPE_RESERVED))	// find else or
					{
						conditionalStatement(0);
						//printf("PASSING A 0 ALONG FOR THE RIDE \n");
						return 1;
					}
					else				//return
					{
						printf("Found end of conditional block \n");
						//conditionalValue = 0;	//reset for next conditional
						return 1;
					}
				}
				if(match(")",TYPE_OPERATOR))	//if ptr does not jump, keep parsing
				{
					next();	
					if(statement())	// at this point, this is a valid if statement
					{	
						if(match("else",TYPE_RESERVED))
						{
							conditionalStatement(1);
							//printf("PASSING A 1 ALONG FOR THE RIDE \n");
							return 1;
						}
						else
						{
							printf("found conditional \n");
							//conditionalValue = 0;	//reset for next conditional
							return 1;
						}
					}
					else
					{
						printf("'if' without statement\n");
						return 0;
					}
				}
				else
				{
					printf("expected a ')' in conditional\n");
					return 0;
				}
			}
			else
			{
				printf("expected a statement after '(' \n");
				return 0;
			}
		}
		else
		{
			printf("expected a '('\n");
			return 0;
		}
	}
	else if(match("else",TYPE_RESERVED))
	{
		next();
		if(conditional)		//If ptr jumped previously, conditional == 1
		{	
			if(entryPoint)
				changePtr(findBracket());
			return 1;
		}
		if(statement())		//If all previous were false, execute else block
		{
			printf("parsed else block\n");
			//if(!match("}",TYPE_OPERATOR))
				//next();
			return 1;
		}
		else
		{
			printf("expected statement after else \n");
			return 0;
		}
	}
	else
	{
		printf("expected 'if' \n");
		return 0;
	}
}

/*
<statement> ::= <if-statement> | <jump-statement> | <while-statement> | <for-statement> 
				<expression-statement> | <compound-statement>
*/

int statement()	// probably get rid of "next()" for all of these
{
	if(match("if",TYPE_RESERVED))
	{
		if(conditionalStatement(0))
		{
			//next();
			return 1;
		}
	}
	else if(match("break",TYPE_RESERVED) || 
	     match("continue",TYPE_RESERVED) || match("return",TYPE_RESERVED) )
	{
		if(jumpStatement())
		{
			//next();
			return 1;
		}
		
	}
	else if(match("while",TYPE_RESERVED))
	{
		if(whileStatement())
		{
			//next();
			return 1;
		}
	}
	else if(match("for",TYPE_RESERVED))
	{
		if(forStatement())
		{
			//next();
			return 1;
		}
	}
	else if(match("{",TYPE_OPERATOR))
	{
		if(compoundStatement())
		{
			return 1;
		}
	}
	else
	{	
		Token token;
		identifyNextToken(&token);
		if(!expressionStatement())	//if its nothing above and not an expression statement, error
		{
			printf("expected expression-statement\n");
			return 0;
		}
	}
}

//<declaration-specifier> ::= <type> 

int declarationSpecifier()		
{
	if(matchtype(TYPE_TYPE))
	{
		/*Token token;
		identifyNextToken(&token);
		symbol_table[symbol_table_ptr].type =	token.type;	//add to symbol at STptr*/
		next();
		printf("found type ... ");
		return 1;
	}
	else
	{
		printf("expected type \n");
		return 0;
	}
}


/*
<declaration> ::= <declaration-specifier> <identifier> { [ <expression> ] }* { = <expression> }? ;
or <declaration> ::= <declaration-specifier> <identifier> { = <expression> }? ; // if ignoring arrays
*/

// Deprecated, left in just in for completeness
int declaration()	//symbol is variable, bracket stack is scope
{
	if(matchtype(TYPE_TYPE))
	{
		printf("found type ... ");
		next();
		if(matchtype(TYPE_IDENTIFIER) || matchtype(TYPE_CHAR))
		{
			next();
			return 1;
		}
		else
		{
			printf("expected identifier\n");
			return 0;
		}
	}
	else
	{
		printf("expected type\n");
		return 0;
	}
}

/*
<parameter-declaration> ::= <declaration-specifier> <identifier>
*/
int parameterDeclaration()		//TODO? MAYBE?
{
	Token token;
	identifyNextToken(&token);
	copyString(&token,'t');
	if(declarationSpecifier())
	{	
		identifyNextToken(&token);
		if(matchtype(TYPE_IDENTIFIER) || matchtype(TYPE_CHAR))
		{
			if(entryPoint)
			{
				if(symbol_table[symbol_table_ptr].name)
				{
					symbol_table_ptr ++;
				}
				copyString(&token,'n');
				symbol_table[symbol_table_ptr].value = param_value[param_ptr];
				param_ptr++;
				findSymbol(varName);
				symbol_table[symbol_table_ptr].scope = currentSymbol.scopeStart;
				symbol_table[symbol_table_ptr].category = 1;	// category
				symbol_table_ptr ++;
				//printSymbolTable();
			}
			next();
			printf("found parameter declaration\n");
			return 1;
			
		}
		else
		{
			printf("expected identifier or char\n");
			return 0;
		}
	}
	else
	{
		printf("expected declaration specifier\n");
		return 0;
	}
}

/*
<parameter-list> ::= <parameter-declaration> { , <parameter-declaration> }*
*/
int parameterList()
{
	if(parameterDeclaration())
	{
		if(match(",",TYPE_OPERATOR))
		{
			next();
			parameterList();
		}
		else
		{
			printf("found list of parameters\n");
			return 1;
		}
	}
	else
	{
		printf("expected parameter declaration\n");
		return 0;
	}
}

/*
<function> ::= <declaration-specifier> <identifier> ( <parameter-list> ) <compound-statement>
*/
int functionOrDeclaration()
{
	int ptr = getPtr();	//n changes depending on where the stack pointer is, to avoid seg fault
	if(bracket_stack_ptr == MAX_SIZE-1)
		symbol_table[symbol_table_ptr].scope = 0;
	else
		symbol_table[symbol_table_ptr].scope = bracket_stack[bracket_stack_ptr + 1];

	if(matchtype(TYPE_TYPE))
	{
		Token token;
		identifyNextToken(&token);	
		varName = token.str;
		copyString(&token, 't'); 	//symbol at SymTablePtr gets type
		printf("found type ... ");
		next();
		if(matchtype(TYPE_IDENTIFIER) || matchtype(TYPE_CHAR))
		{
			identifyNextToken(&token); 	// might have to make a new one here if this breaks
			copyString(&token,'n');	// symbol gets a name
			checkMain();			// check for entrypoint
			next();
			if(match("(",TYPE_OPERATOR))		//function
			{
				if(!symbol_table[symbol_table_ptr].ptr)
					symbol_table[symbol_table_ptr].ptr = ptr;
				addSymbol(0);
				next();
				if(!match(")",TYPE_OPERATOR))	// If not func()
				{
					if(parameterList())
					{
						if(match(")",TYPE_OPERATOR))
						{
							next();
							symbol_table[symbol_table_ptr -1].scopeStart = getPtr();
							if(compoundStatement())
							{
								printf("found function\n");
								return 1;
							}
							else
							{
								printf("expected compound statement\n");
								return 0;
							}
						}
						else
						{
							printf("expected ')'\n");
							return 0;
						}
					}
					else
					{
						printf("expected list of parameters\n");
						return 0;
					}
				}
				else				//if func(), aka matched ')'
				{
					next();
					symbol_table[symbol_table_ptr -1].scopeStart = getPtr();
					if(compoundStatement())
					{
						printf("found function\n");
						return 1;
					}
					else
					{
						printf("expected compound statement\n");
						return 0;
					}
					
				}
			}
			else if(match("=",TYPE_OPERATOR))	// declaration TONS OF EDITS HERE
			{
				addSymbol(1);
				next();
				if(expression())
				{
					next();
					changeVar(varName);
					symbol_table[symbol_table_ptr].value = rval;
					printf("declaration with initialization %s\n",varName);
					printf("Symbol Table Stuff: %s %f\n ",symbol_table[symbol_table_ptr].name,symbol_table[symbol_table_ptr].value);
					//printSymbolTable();
					return 1;
				}
				else
				{
					printf("expected expression after '='\n");
					return 0;
				}
			}
			else if(match(";",TYPE_OPERATOR))	// also declaration
			{
				addSymbol(1);
				next();
				printf("found declaration\n");
				return 1;
			}
			else
			{
				printf("expected ';' or '=' or '(' \n");
				return 0;
			}
		}
		else
		{
			printf("expected identifier\n");
			return 0;
		}
	}
	else
	{
		Token token;
		identifyNextToken(&token);
		printf("expected type in function or declaration: not %c\n",token.str[0]);
		return 0;
	}
}
/*
<program> ::= { <function> | <declaration> }*
*/
int program()
{
	if(functionOrDeclaration())
		return 1;
	else if(match(";",TYPE_OPERATOR))
	{
		next();
		return 1;
	}
	else
		return 0;
}


int main(int argc, char** argv)
{
	while(pass < 3)
	{
		processFiles(argv);
		lexical();		//pretty-prints the input
		printf("\n");
		int i;
		
		while(program());		//normal code execution
		if(pass == 1)
		{
			printf("\n\n Bracket Table: \n");
			for(i = 0; i < bracket_table_ptr; i++)
			{
				printf("%d points to %d\n",bracket_table_index[i],bracket_table_value[i]);
			}
			
			printSymbolTable();
		
			printf("\n");
		}
		else if( pass == 2)
		{
			printf("\n\n Global Variable final values: \n");
			for(i = 0; i < symbol_table_ptr; i++)
			{
				if(symbol_table[i].scope == 0 && symbol_table[i].category == 1)
				{
					printf("%s\t",symbol_table[i].name);
					float val = symbol_table[i].value;
					switch(symbol_table[i].type[0])
					{
						case 'i':
							printf("%d\n",(int)val);
							break;
						case 'f':
							printf("%f\n",val);
							break;
						case 'c':
							printf("%c\n",(int)val);
							break;
					}
				}
			}
			
		}
		pass ++;
		resetPtr();
	}
}
