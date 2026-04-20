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
#define MAX_LOOPS 50

extern int syntaxErrorCount;
extern int semanticErrorCount;

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

static int loop_stack[MAX_LOOPS];
static int loop_top = -1;
static int building_while_condition = 0;

/* while support buffers */
static char while_condition_lines[50][256];
static int while_condition_count = 0;

static char while_body_lines[200][256];
static int while_body_count = 0;

static int capturing_condition = 0;
static int capturing_body = 0;

static char last_condition_temp[50];

/* ---------- helpers ---------- */
static void push_expr(const char *text);
static int pop_expr(char *out);
static int symbol_exists(const char *name);
static void add_symbol(const char *name);
static void add_code(const char *line);
static void reset_codegen(void);

static int is_condition_operator(const char *op);
static void insert_code_line(int index, const char *line);

/* Added helper for while body buffering */
static void add_while_body_code(const char *line);

/* ---------- helper implementations ---------- */
static void push_expr(const char *text)
{
    /* push value (id, literal, or temp) onto expression stack */
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

    /* pop top value from expression stack */
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

    /* check if variable already exists in symbol table */
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
    /* add variable or temp to symbol table if not already present */
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
    /* store generated line of C code */
    if (code_count < MAX_CODE)
    {
        strncpy(code_lines[code_count], line, MAX_LINE - 1);
        code_lines[code_count][MAX_LINE - 1] = '\0';
        code_count++;
    }

    /* also write to temp file for debugging/assignment requirement */
    if (tempFile2 != NULL)
    {
        fprintf(tempFile2, "%s\n", line);
    }
}

/* helper used when building while body */
static void add_while_body_code(const char *line)
{
    /* store lines that belong inside while loop body */
    if (while_body_count < 200)
    {
        strncpy(while_body_lines[while_body_count], line, 255);
        while_body_lines[while_body_count][255] = '\0';
        while_body_count++;
    }
}

static void reset_codegen(void)
{
    /* reset all structures before starting new program */
    expr_top = -1;
    symbol_count = 0;
    code_count = 0;
    temp_num = 0;
    tmp_open = 0;
    loop_top = -1;
    building_while_condition = 0;

    while_condition_count = 0;
    while_body_count = 0;
    capturing_condition = 0;
    capturing_body = 0;
    last_condition_temp[0] = '\0';
}

static void insert_code_line(int index, const char *line)
{
    int i;

    /* insert a line into generated code at a specific position */
    if (code_count >= MAX_CODE || index < 0 || index > code_count)
    {
        return;
    }

    for (i = code_count; i > index; i--)
    {
        strcpy(code_lines[i], code_lines[i - 1]);
    }

    strncpy(code_lines[index], line, MAX_LINE - 1);
    code_lines[index][MAX_LINE - 1] = '\0';
    code_count++;
}

static int is_condition_operator(const char *op)
{
    /* check if operator is relational/logical */
    return (strcmp(op, "==") == 0 ||
            strcmp(op, "!=") == 0 ||
            strcmp(op, "<") == 0 ||
            strcmp(op, "<=") == 0 ||
            strcmp(op, ">") == 0 ||
            strcmp(op, ">=") == 0 ||
            strcmp(op, "&&") == 0 ||
            strcmp(op, "||") == 0);
}

/* ---------- required action routines ---------- */

/* initialize code generation */
void start(void)
{
    /* initialize code generation */
    reset_codegen();
}

/* generate final C code output, including headers, main function, declarations, and body */
void finish(void)
{
    int i;
    time_t now;
    char *stamp;

    /* generate timestamp */
    now = time(NULL);
    stamp = ctime(&now);

    /* output program header */
    fprintf(cFile, "/*\n");
    fprintf(cFile, "C program of MICRO program %s\n", inputFileName);
    if (stamp != NULL)
    {
        fprintf(cFile, "%s", stamp);
    }
    fprintf(cFile, "*/\n");

    /* begin main function */
    fprintf(cFile, "#include<stdio.h>\n");
    fprintf(cFile, "int main(void)\n");
    fprintf(cFile, "{\n");

    /* declare all variables and temps */
    for (i = 0; i < symbol_count; i++)
    {
        fprintf(cFile, "int %s;\n", symbols[i]);
    }

    /* output generated code */
    for (i = 0; i < code_count; i++)
    {
        fprintf(cFile, "%s\n", code_lines[i]);
    }

    /* close program */
    fprintf(cFile, "return 0;\n");
    fprintf(cFile, "}\n");

    /* print success message if no errors */
    if (lexicalErrorCount == 0 && syntaxErrorCount == 0 && semanticErrorCount == 0){
        fprintf(cFile, "/* PROGRAMED COMPILED WITH NO ERRORS. */\n");
    }
}

/* handle identifier: add to symbol table and push onto expression stack */
void process_id(const char *id)
{
    /* handle identifier: store and push to stack */
    add_symbol(id);
    push_expr(id);
}

/* handle numeric literal: just push to stack */
void process_literal(const char *literal)
{
    /* handle numeric literal */
    push_expr(literal);
}

/* convert MICRO operators to C equivalents and push onto stack */
void process_op(const char *op)
{
    /* convert MICRO operators to C equivalents */
    if (strcmp(op, "=") == 0)
        push_expr("==");
    else if (strcmp(op, "<>") == 0)
        push_expr("!=");
    else if (strcmp(op, "and") == 0)
        push_expr("&&");
    else if (strcmp(op, "or") == 0)
        push_expr("||");
    else if (strcmp(op, "true") == 0)
        push_expr("1");
    else if (strcmp(op, "false") == 0)
        push_expr("0");
    else if (strcmp(op, "null") == 0)
        push_expr("0");
    else
        push_expr(op);
}

/* generate code for infix expression, using stack to manage components */
void gen_infix(void)
{
    char right[MAX_TEXT];
    char op[MAX_TEXT];
    char left[MAX_TEXT];
    char temp[MAX_TEXT];
    char line[MAX_LINE];

    int check = 1;

    /* pop expression components */
    if (!pop_expr(right)) check = 0;
    if (check && !pop_expr(op)) check = 0;
    if (check && !pop_expr(left)) check = 0;

    if (check)
    {
        /* create temporary variable */
        temp_num++;
        sprintf(temp, "Temp%d", temp_num);
        add_symbol(temp);

        /* build expression */
        sprintf(line, "%s = %s %s %s;", temp, left, op, right);

        /* if building while condition, store separately */
        if (capturing_condition && is_condition_operator(op))
        {
            if (while_condition_count < 50)
            {
                strcpy(while_condition_lines[while_condition_count], line);
                while_condition_count++;
            }

            /* remember temp for while header */
            strcpy(last_condition_temp, temp);
        }
        else
        {
            /* switch to body mode after condition */
            if (tmp_open && capturing_condition)
            {
                capturing_condition = 0;
                capturing_body = 1;
            }

            /* store code in correct place */
            if (capturing_body)
                add_while_body_code(line);
            else
                add_code(line);
        }

        /* push temp back onto stack */
        push_expr(temp);
    }
}

/* conditions are generated using same logic as expressions, but we also need to track them for while loops */
void gen_condition(void)
{
    /* condition uses same logic as expression */
    gen_infix();
}

/* assignment and io actions also need to handle while body buffering */
void assign(const char *lhs)
{
    char rhs[MAX_TEXT];
    char line[MAX_LINE];
    int ok = 1;

    /* get result of expression */
    if (!pop_expr(rhs)) ok = 0;

    if(ok){
        add_symbol(lhs);

        /* build assignment */
        sprintf(line, "%s = %s;", lhs, rhs);

        /* switch to body mode if needed */
        if (tmp_open && capturing_condition)
        {
            capturing_condition = 0;
            capturing_body = 1;
        }

        /* store appropriately */
        if (capturing_body)
            add_while_body_code(line);
        else
            add_code(line);
    }
}

/* generate code for reading input into an identifier, also add to symbol table */
void read_id(const char *id)
{
    char line[MAX_LINE];

    add_symbol(id);

    /* generate input statement */
    sprintf(line, "scanf(\"%%d\", &%s);", id);

    if (tmp_open && capturing_condition)
    {
        capturing_condition = 0;
        capturing_body = 1;
    }

    if (capturing_body)
        add_while_body_code(line);
    else
        add_code(line);
}

/* generate code for writing an expression result, also handle while body buffering */
void write_expr(void)
{
    char expr[MAX_TEXT];
    char line[MAX_LINE];
    int ok = 1;

    /* get expression value */
    if (!pop_expr(expr)) ok =0;

    if(ok){
        /* generate output statement */
        sprintf(line, "printf(\"%%d\\n\", %s);", expr);

        if (tmp_open && capturing_condition)
        {
            capturing_condition = 0;
            capturing_body = 1;
        }

        if (capturing_body)
            add_while_body_code(line);
        else
            add_code(line);
    }
}

/* ---------- if/while hooks ---------- */
void begin_if(void)
{
    char cond[MAX_TEXT];
    char line[MAX_LINE];

    /* generate if header */
    if (pop_expr(cond))
    {
        snprintf(line, sizeof(line), "if (%s) {", cond);
        add_code(line);
    }
}

/* generate else block */
void emit_else(void)
{
    /* generate else block */
    add_code("} else {");
}

/* close if block */
void end_if(void)
{
    /* close if block */
    add_code("}");
}

/* open while loop: start capturing condition and body code separately */
void open_tmp(void)
{
    /* begin processing while loop */

    capturing_condition = 1;
    capturing_body = 0;

    /* reset buffers */
    while_condition_count = 0;
    while_body_count = 0;
    last_condition_temp[0] = '\0';

    tmp_open = 1;
}

/* close while loop: output condition and body in correct order, then reset state */
void write_temp(void)
{
    int i;
    char line[MAX_LINE];

    /* output condition before loop */
    for (i = 0; i < while_condition_count; i++)
    {
        add_code(while_condition_lines[i]);
    }

    /* generate while header */
    if (last_condition_temp[0] != '\0')
    {
        snprintf(line, sizeof(line), "while (%s) {", last_condition_temp);
        add_code(line);
    }

    /* output loop body */
    for (i = 0; i < while_body_count; i++)
    {
        add_code(while_body_lines[i]);
    }

    /* re-evaluate condition */
    for (i = 0; i < while_condition_count; i++)
    {
        add_code(while_condition_lines[i]);
    }

    /* close loop */
    if (last_condition_temp[0] != '\0')
    {
        add_code("}");
    }

    /* reset state */
    tmp_open = 0;
    building_while_condition = 0;
    capturing_condition = 0;
    capturing_body = 0;
}