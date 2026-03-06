#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "scanner.h"
#include "file_util.h"

/*
 * Parser-wide counters.
 * lexicalErrorCount and currentLine already exist in file_util.
 * These two are new for parser program 3.
 */
extern int syntaxErrorCount;
extern int semanticErrorCount;

/*
 * Startup / finish routines for parser program 3.
 * main should call parser_startup(), then system_goal(), then parser_finish().
 */
void parser_startup(void);
void parser_finish(void);

/*
 * Support routines.
 * next_token() = non-destructive lookahead from parser's point of view.
 * match()      = checks expected token against current lookahead and advances if matched.
 * recover_to_semicolon() = simple syntax error recovery.
 */
Token next_token(char *buffer);
int match(Token expected, char *buffer);
int recover_to_semicolon(char *buffer);

/*
 * Recursive descent grammar functions.
 * One function per production (or grouped productions where practical).
 */
int system_goal(char *buffer);
int program(char *buffer);
int statement_list(char *buffer);
int statement(char *buffer);
int if_tail(char *buffer);
int id_list(char *buffer);
int expr_list(char *buffer);
int expression(char *buffer);
int term(char *buffer);
int factor(char *buffer);
int condition(char *buffer);

#endif