#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "action.h"

FILE *codeFile;
int tempCount = 1;

/* -------- STACK -------- */
char *stack[100];
int top = -1;

void push(char *str) {
    stack[++top] = strdup(str);
}

char* pop() {
    return stack[top--];
}

/* -------- ACTIONS -------- */

void start() {
    codeFile = fopen("out.c", "w");

    fprintf(codeFile, "#include<stdio.h>\n");
    fprintf(codeFile, "main() {\n");
}

void finish() {
    fprintf(codeFile, "return 0;\n}\n");
    fclose(codeFile);
}

void process_id(char *lexeme) {
    fprintf(codeFile, "int %s;\n", lexeme);
    push(lexeme);
}

void process_literal(char *lexeme) {
    push(lexeme);
}

void process_op(char *op) {
    push(op);
}

void gen_infix() {
    char *right = pop();
    char *op    = pop();
    char *left  = pop();

    char temp[20];
    sprintf(temp, "Temp%d", tempCount++);

    fprintf(codeFile, "%s = %s %s %s;\n", temp, left, op, right);

    push(temp);
}

void assign(char *id) {
    char *expr = pop();
    fprintf(codeFile, "%s = %s;\n", id, expr);
}

void read_id(char *id) {
    fprintf(codeFile, "scanf(\"%%d\", &%s);\n", id);
}

void write_expr() {
    char *expr = pop();
    fprintf(codeFile, "printf(\"%%d\\n\", %s);\n", expr);
}