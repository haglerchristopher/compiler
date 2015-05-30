#ifndef LEXER_H
#define LEXER_H

#define MAXTOKENLEN 4096

extern char token_string[MAXTOKENLEN + 1];

/*
 * Place the next token into token string and then
 * return the TOKENTYPE of the lexeme in the tokenstring buffer
 */
TOKENTYPE get_token();

#endif /* lexer.h */
