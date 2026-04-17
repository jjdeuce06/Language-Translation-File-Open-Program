#include <stdio.h>
#include <string.h>
#include <time.h>
#include "action.h"
#include "file_util.h"

#define MAX_STACK 200
#define MAX_TEXT  128
#define MAX_SYMS  200
#define MAX_CODE  500
#define MAX_LINE  256

/* expression stack */
static char expr_stack[MAX_STACK][MAX_TEXT];
static int expr_top = -1;

/* symbol table for declarations */
static char symbols[MAX_SYMS][MAX_TEXT];
static int symbol_count = 0;

/* generated code lines */
static char code_lines[MAX_CODE][MAX_LINE];
static int code_count = 0;

/* temp numbering */
static int temp_num = 0;

/* while support placeholder */
static int tmp_open = 0;

/* ---------- helpers ---------- */
static void push_expr(const char *text);
static int pop_expr(char *out);
static int symbol_exists(const char *name);
static void add_symbol(const char *name);
static void add_code(const char *line);
static void reset_codegen(void);

/* ---------- helper implementations ---------- */
static void push_expr(const char *text)
{
    if (expr_top < MAX_STACK - 1)
    {
        expr_top++;
        strncpy(expr_stack[expr_top], text, MAX_TEXT - 1);
        expr_stack[expr_top][MAX_TEXT - 1] = '\0';
    }
}

static int pop_expr(char *out)
{
    int success = 0;

    if (expr_top >= 0)
    {
        strcpy(out, expr_stack[expr_top]);
        expr_top--;
        success = 1;
    }
    return success;
}

static int symbol_exists(const char *name)
{
    int found = 0;
    int i;

    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbols[i], name) == 0)
        {
            found = 1;
        }
    }
    return found;
}

static void add_symbol(const char *name)
{
    if (name == NULL || name[0] == '\0')
    {
        return;
    }

    if (!symbol_exists(name) && symbol_count < MAX_SYMS)
    {
        strncpy(symbols[symbol_count], name, MAX_TEXT - 1);
        symbols[symbol_count][MAX_TEXT - 1] = '\0';
        symbol_count++;
    }
}

static void add_code(const char *line)
{
    if (code_count < MAX_CODE)
    {
        strncpy(code_lines[code_count], line, MAX_LINE - 1);
        code_lines[code_count][MAX_LINE - 1] = '\0';
        code_count++;
    }

    /* keep temp file behavior alive too */
    if (tempFile2 != NULL)
    {
        fprintf(tempFile2, "%s\n", line);
    }
}

static void reset_codegen(void)
{
    expr_top = -1;
    symbol_count = 0;
    code_count = 0;
    temp_num = 0;
    tmp_open = 0;
}

/* ---------- required action routines ---------- */

void start(void)
{
    reset_codegen();
}

void finish(void)
{
    int i;
    time_t now;
    char *stamp;
    printf("---------DEBUG: finish called----------\n");

    now = time(NULL);
    stamp = ctime(&now);

    fprintf(outputFile, "/*\n");
    fprintf(outputFile, "C program of MICRO program %s\n", inputFileName);
    if (stamp != NULL)
    {
        fprintf(outputFile, "%s", stamp);
    }
    fprintf(outputFile, "*/\n");
    fprintf(outputFile, "#include<stdio.h>\n");
    fprintf(outputFile, "main()\n");
    fprintf(outputFile, "{\n");

    for (i = 0; i < symbol_count; i++)
    {
        fprintf(outputFile, "int %s;\n", symbols[i]);
    }

    for (i = 0; i < code_count; i++)
    {
        fprintf(outputFile, "%s\n", code_lines[i]);
    }

    fprintf(outputFile, "return 0;\n");
    fprintf(outputFile, "}\n");
    fprintf(outputFile, "/* PROGRAMED COMPILED WITH NO ERRORS. */\n");
}

void process_id(const char *id)
{
    add_symbol(id);
    push_expr(id);
}

void process_literal(const char *literal)
{
    push_expr(literal);
}

void process_op(const char *op)
{
    if (strcmp(op, "=") == 0)
    {
        push_expr("==");
    }
    else if (strcmp(op, "<>") == 0)
    {
        push_expr("!=");
    }
    else if (strcmp(op, "and") == 0)
    {
        push_expr("&&");
    }
    else if (strcmp(op, "or") == 0)
    {
        push_expr("||");
    }
    else if (strcmp(op, "true") == 0)
    {
        push_expr("1");
    }
    else if (strcmp(op, "false") == 0)
    {
        push_expr("0");
    }
    else if (strcmp(op, "null") == 0)
    {
        push_expr("0");
    }
    else
    {
        push_expr(op);
    }
}

void gen_infix(void)
{
    char right[MAX_TEXT];
    char op[MAX_TEXT];
    char left[MAX_TEXT];
    char temp[MAX_TEXT];
    char line[MAX_LINE];

    int check = 1;

    if (!pop_expr(right)) check = 0;
    if (check && !pop_expr(op)) check = 0;
    if (check && !pop_expr(left)) check = 0;

    if(check){
        temp_num++;
        sprintf(temp, "Temp%d", temp_num);
        add_symbol(temp);

        sprintf(line, "%s = %s %s %s;", temp, left, op, right);
        add_code(line);

        push_expr(temp);
    }
}

void gen_condition(void)
{
    /* for now, same temp-generation behavior */
    gen_infix();
}

void assign(const char *lhs)
{
    char rhs[MAX_TEXT];
    char line[MAX_LINE];
    int ok = 1;
    if (!pop_expr(rhs)) ok = 0;
    if(ok){
        add_symbol(lhs);
        sprintf(line, "%s = %s;", lhs, rhs);
        add_code(line);
    }
}

void read_id(const char *id)
{
    char line[MAX_LINE];

    add_symbol(id);
    sprintf(line, "scanf(\"%%d\", &%s);", id);
    add_code(line);
}

void write_expr(void)
{
    char expr[MAX_TEXT];
    char line[MAX_LINE];
    int ok = 1;

    if (!pop_expr(expr)) ok =0;
    if(ok){
        sprintf(line, "printf(\"%%d\\n\", %s);", expr);
        add_code(line);
    }
}

void open_tmp(void)
{
    tmp_open = 1;
}

void write_temp(void)
{
    tmp_open = 0;
}