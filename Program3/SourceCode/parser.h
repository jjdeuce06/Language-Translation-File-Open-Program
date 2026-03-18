#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "scanner.h"
#include "file_util.h"

/*
 * Parser-wide counters.
 */
extern int syntaxErrorCount;
extern int semanticErrorCount;

/*
 * Startup / finish routines for parser program 3.
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

int c_expression(char *buffer);
int c_term(char *buffer);
int c_factor(char *buffer);
int c_primary(char *buffer);

static char statementBuffer[1024];

/* ---------- private helper prototypes ---------- */
static void advance_token(void);
static int is_statement_start(Token t);
static int is_add_op(Token t);
static int is_mult_op(Token t);

static int is_rel_op(Token t);
static int is_logical_op(Token t);

static void clear_statement_buffer(void);
static void append_statement(const char *text);
static void print_completed_statement(void);
static Token lookahead;
static char lookaheadBuffer[128];


#endif