#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "lexical/lexer.h"

FILE *source;
FILE *listing;
char token_string[4097];

char buffer[37][15] = {
	"ID", "NUM","ELSE","END","ESLE", "FI","FUNCTION","IF","INT",
	"PRINT","READ","REPEAT","RETURN","SHORT","TEAPER",
	"THEN","LEFTPAREN","RIGHTPAREN","MULTIPLY","SUBTRACT",
	"ADD","DIVIDE","MOD","ASSIGN","LESSTHAN","GREATERTHAN","QUOTE","SEMICOLON","RIGHTCRLYBRCE",
	"LEFTCRLYBRCE","QUOTE","AND","OR","XOR","NOT","ENDFILE","ERROR"
};

int main(int argc, char **argv)
{
	TOKENTYPE token;
	if(argc >= 2) {
		source = fopen(argv[1], "r");
		while((token = get_token()) != ENDFILE) {
			printf("TokenType: %s, Value: %s\n",buffer[token], token_string);
		}		
	}
	return 0;
}
