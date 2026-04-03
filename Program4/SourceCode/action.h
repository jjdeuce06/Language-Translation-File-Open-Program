#ifndef ACTION_H
#define ACTION_H

#include <stdio.h>

/* Code output file */
extern FILE *codeFile;

/* Temp variable counter */
extern int tempCount;

/* Stack for expression handling */
void push(char *str);
char* pop(void);

/* Actions */
void start(void);
void finish(void);

void process_id(char *lexeme);
void process_literal(char *lexeme);
void process_op(char *op);

void gen_infix(void);
void assign(char *id);

void read_id(char *id);
void write_expr(void);

#endif