#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../globals.h"
#include "lexer.h"

#define BUFFERLEN (MAXTOKENLEN + 1)

static char linebuf[BUFFERLEN];
static int bufsize;
static int linepos;

static int EOF_FLAG = FALSE;

typedef enum {
	/* STATES OF SCANNER DFA */
	START,  COMMENT,   IDENTIFIER, SYMBOL,
	NUMBER, OPERATION, DONE
} DFA_STATE;

struct token {
	TOKENTYPE token;
	char *str;
} reservedWords[MAXRESERVED] = {
    {ELSE, "else"},
	{END, "end"},
	{ELSE, "else"},
	{FI, "fi"},
	{FUNCTION, "function"},
    {IF, "if"},
	{INT, "int"},
    {PRINT, "print"},
	{READ, "read"},
	{REPEAT, "repeat"},
	{RETURN, "return"},
	{SHORT, "short"},
	{TEAPER, "teaper"},
	{THEN, "then"}
};

static TOKENTYPE keyword_search(char *string)
{
	for(int i = 0; i < MAXRESERVED; i++) {
		if (!strcmp(reservedWords[i].str, string)) {
				return reservedWords[i].token;
		}
    }
	return ID;
}

static int get_next_char()
{
	if (!(linepos < bufsize)) {
		linepos++;
		if (fgets(linebuf, BUFFERLEN - 1, source)) {
			linepos = 0;
			bufsize = strlen(linebuf);
			return linebuf[linepos++];
		} else {
			EOF_FLAG = TRUE;
			return EOF;
		}
	} else {
		return linebuf[linepos++];
	}
}

static void unget_next_char()
{
 	if (!EOF_FLAG)
        	linepos--;
}

static int islanguagesymbol(int ch, TOKENTYPE *current_token)
{
	int result = TRUE;
	if (ch == ';') {
		*current_token = SEMICOLON;
	} else if (ch == '{') {
		*current_token = RIGHTCRLYBRCE;
	} else if (ch == '}') {
		*current_token = LEFTCRLYBRCE;
	} else if (ch == '\"') {
		*current_token = QUOTE;
	} else if (ch == '(') {
		*current_token = LEFTPAREN;
	}else if (ch == ')') {
		*current_token = RIGHTPAREN;
	}
	else
		result = FALSE;
	return result;
		
}

static int isoperation(int ch, TOKENTYPE *current_token)
{
	int result = TRUE;
	if (ch == '*') {
		*current_token = MULTIPLY;
	} else if (ch == '/') {
		*current_token = DIVIDE;
	} else if (ch == '+') {
		*current_token = ADD;
	} else if (ch == '-') {
		*current_token = SUBTRACT;
	} else if (ch == '%'){
		*current_token = MOD;
	} else if (ch == '=') {
		*current_token = ASSIGN;
	}
	else
		result = FALSE;
	return result;
}

static int islogicaloperation(int ch, TOKENTYPE *current_token)
{
	int result = TRUE;
	if (ch == '~') {
		*current_token = XOR;
	} else if(ch == '&') {
		*current_token = AND;
	} else if(ch == '|') {
		*current_token = OR;
	} else if(ch == '!') {
		*current_token = NOT;
	}
	else
		result = FALSE;
	return result;
}

static int iscomparisonoperation(int ch, TOKENTYPE *current_token)
{
	int result = TRUE;
	if (ch == '<')
		*current_token = LESSTHAN;
	else if (ch == '>')
		*current_token = GREATERTHAN;
	else
		result = FALSE;
	return result;
}

static int iscomment(int ch)
{
	int result = FALSE;
	int next;
	if (ch == '/') {
		if ((next = get_next_char()) == '*')
			result = TRUE;
		unget_next_char();
	} else if (ch == '*') {
		if ((next = get_next_char()) == '/')
			result = TRUE;
		unget_next_char();
	}
	return result;
}

/*
 * Implementation of the DFA
 */
TOKENTYPE get_token()
{
	int token_string_index = 0;
	int save;
	int lexeme;

	TOKENTYPE *current_token;
	DFA_STATE state = START;
	while (state != DONE) {		
		lexeme = get_next_char();
		save = TRUE;
		switch(state) {
			case START:
				if (isalpha(lexeme))
					state = IDENTIFIER;
				else if (isdigit(lexeme))
					state = NUMBER;
				else if (islanguagesymbol(lexeme, current_token))
					state = DONE;
				else if (isoperation(lexeme, current_token))
					state = DONE;
				else if (islogicaloperation(lexeme, current_token))
					state = DONE;
				else if (iscomparisonoperation(lexeme, current_token))
					state = DONE;
				else if (isspace(lexeme))
					save = FALSE;
				else if (iscomment(lexeme)) {
					state = COMMENT;
					save = FALSE;
				}
				else { //EOF or ERROR
					state = END;
					save = FALSE;
					unget_next_char(); //prevent skipping over error token
				}
				break;

			case COMMENT:
				save = FALSE;
				if (lexeme == '*' && iscomment(lexeme)) {
					lexeme = get_next_char();
					state = START;
				}
				break;
				
			case IDENTIFIER:
				if (!isalnum(lexeme)) {
					unget_next_char();
					*current_token = ID;
					save = FALSE;
					state = DONE;
				}
				break;
				
			case NUMBER:
				if (!isdigit(lexeme)) {
					unget_next_char();
					save = FALSE;
					state = DONE;
					*current_token = NUM;
				}
				break;
				
			case END:
				switch(lexeme) {
					case EOF:
						save = FALSE;
						state = DONE;
						*current_token = ENDFILE;
						break;
					default:
						save = FALSE;
						state = DONE;
						*current_token = ERROR;
						break;
				}
				break;
			default:
					printf("Error during lexical analysis with input: %c\n", lexeme);
					printf("Contents of buffer\n\t: %s", token_string);
					state = DONE;
					*current_token = ERROR;
				break;
		}
		if (save && token_string_index <= MAXTOKENLEN)
			token_string[token_string_index++] = (char)lexeme;
		if (state == DONE) {
			token_string[token_string_index] = '\0';
			if (*current_token == ID)
				*current_token = keyword_search(token_string);
		}
	}
	return *current_token;
}
