#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "file_util.h"
#include "action.h"

int syntaxErrorCount = 0;
int semanticErrorCount = 0;

/* =========================================================
   parser_startup
   - resets parser counters
   - reuses scanner startup so currentLine / lexicalErrorCount
     and scanner state are initialized correctly
   - grabs first token into lookahead
   ========================================================= */
void parser_startup(void)
{
    syntaxErrorCount = 0;
    semanticErrorCount = 0;

    clear_statement_buffer();

    /* Reuse scanner startup function */
    start_up(inputFile, outputFile, listingFile);

    /* Prime the LL(1) lookahead */
    advance_token();
}

/* =========================================================
   parser_finish
   - prints totals to the listing file
   ========================================================= */
void parser_finish(void)
{
    fprintf(listingFile, "\n\nLexical Errors: %d\n", lexicalErrorCount);
    fprintf(listingFile, "Syntax Errors: %d\n", syntaxErrorCount);
    fprintf(listingFile, "Semantic Errors: %d\n", semanticErrorCount);
}

/* =========================================================
   advance_token
   ========================================================= */
static void advance_token(void)
{
    lookahead = scanner(lookaheadBuffer, inputFile, outputFile, listingFile);
}

/* =========================================================
   next_token
   ========================================================= */
Token next_token(char *buffer)
{
    strcpy(buffer, lookaheadBuffer);
    return lookahead;
}

/* =========================================================
   match
   ========================================================= */
int match(Token expected, char *buffer)
{
    int result = 1;

    strcpy(buffer, lookaheadBuffer);

    fprintf(outputFile,
            "Parser match -> expected: %-14s actual: %-14s lexeme: %s\n",
            token_to_string(expected),
            token_to_string(lookahead),
            lookaheadBuffer);

    append_statement(lookaheadBuffer);

    if (lookahead != expected)
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: expected %s but found %s (lexeme: %s) on line %d\n",
                token_to_string(expected),
                token_to_string(lookahead),
                lookaheadBuffer,
                currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    if (lookahead == BEGIN ||
        lookahead == SEMICOLON ||
        lookahead == THEN ||
        lookahead == ELSE ||
        lookahead == ENDIF ||
        lookahead == ENDWHILE ||
        lookahead == END ||
        lookahead == SCANEOF)
    {
        print_completed_statement();
        clear_statement_buffer();
    }

    if (result)
    {
        advance_token();
    }

    return result;
}

/* =========================================================
   recover_to_semicolon
   ========================================================= */
int recover_to_semicolon(char *buffer)
{
    int result = 1;

    while (lookahead != SEMICOLON &&
           lookahead != SCANEOF &&
           lookahead != END &&
           lookahead != ENDIF &&
           lookahead != ENDWHILE)
    {
        advance_token();
    }

    if (lookahead == SEMICOLON)
    {
        result = match(SEMICOLON, buffer) && result;
    }

    clear_statement_buffer();
    return result;
}

/* =========================================================
   system_goal
   production 44:
   <system goal> -> <program> SCANEOF #finish
   ========================================================= */
int system_goal(char *buffer)
{
    int ok = 1;

    if (!program(buffer))
        ok = 0;

    if (!match(SCANEOF, buffer))
        ok = 0;

    finish();

    return ok;
}

/* =========================================================
   program
   production 1:
   <program> -> #start BEGIN <statement list> END
   ========================================================= */
int program(char *buffer)
{
    int ok = 1;

    start();

    if (!match(BEGIN, buffer))
        ok = 0;

    if (!statement_list(buffer))
        ok = 0;

    if (!match(END, buffer))
        ok = 0;

    return ok;
}

/* =========================================================
   statement_list
   ========================================================= */
int statement_list(char *buffer)
{
    int ok = 1;
    Token t = next_token(buffer);

    while (is_statement_start(t))
    {
        if (!statement(buffer))
        {
            ok = 0;
            recover_to_semicolon(buffer);
        }

        t = next_token(buffer);
    }

    return ok;
}

/* =========================================================
   statement
   ========================================================= */
int statement(char *buffer)
{
    Token t = next_token(buffer);
    int result = 0;

    if (t == ID)
    {
        char lhs[128];
        strcpy(lhs, lookaheadBuffer);

        result = match(ID, buffer) &&
                 match(ASSIGNOP, buffer) &&
                 expression(buffer);

        if (result)
        {
            assign(lhs);
        }

        if (result)
        {
            result = match(SEMICOLON, buffer);
        }
    }
    else if (t == READ)
    {
        result = match(READ, buffer) &&
                 match(LPAREN, buffer) &&
                 id_list(buffer) &&
                 match(RPAREN, buffer) &&
                 match(SEMICOLON, buffer);
    }
    else if (t == WRITE)
    {
        result = match(WRITE, buffer) &&
                 match(LPAREN, buffer) &&
                 expr_list(buffer) &&
                 match(RPAREN, buffer) &&
                 match(SEMICOLON, buffer);
    }
    else if (t == IF)
    {
        result = match(IF, buffer) &&
                 match(LPAREN, buffer) &&
                 condition(buffer) &&
                 match(RPAREN, buffer) &&
                 match(THEN, buffer) &&
                 statement_list(buffer) &&
                 if_tail(buffer);
    }
    else if (t == WHILE)
    {
        open_tmp();

        result = match(WHILE, buffer) &&
                 match(LPAREN, buffer) &&
                 condition(buffer) &&
                 match(RPAREN, buffer);

        if (result)
        {
            print_completed_statement();
            clear_statement_buffer();
        }

        if (result)
        {
            result = statement_list(buffer) &&
                     match(ENDWHILE, buffer);
        }

        if (result)
        {
            write_temp();
        }
    }
    else
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: invalid start of statement on line %d\n",
                currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    return result;
}

/* =========================================================
   if_tail
   ========================================================= */
int if_tail(char *buffer)
{
    Token t = next_token(buffer);
    int result = 0;

    if (t == ELSE)
    {
        result = match(ELSE, buffer) &&
                 statement_list(buffer) &&
                 match(ENDIF, buffer);
    }
    else if (t == ENDIF)
    {
        result = match(ENDIF, buffer);
    }
    else
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: expected ELSE or ENDIF on line %d\n",
                currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    return result;
}

/* =========================================================
   id_list
   production 10:
   <id list> -> <ident> #read_id {, <id list>}
   ========================================================= */
int id_list(char *buffer)
{
    int result = 1;

    if (next_token(buffer) == ID)
    {
        char idname[128];
        strcpy(idname, lookaheadBuffer);

        if (!match(ID, buffer))
        {
            result = 0;
        }
        else
        {
            read_id(idname);
        }
    }
    else
    {
        result = 0;
    }

    while (result && next_token(buffer) == COMMA)
    {
        if (!match(COMMA, buffer))
        {
            result = 0;
        }

        if (result && next_token(buffer) == ID)
        {
            char idname[128];
            strcpy(idname, lookaheadBuffer);

            if (!match(ID, buffer))
            {
                result = 0;
            }
            else
            {
                read_id(idname);
            }
        }
        else if (result)
        {
            result = 0;
        }
    }

    return result;
}

/* =========================================================
   expr_list
   production 11:
   <expr list> -> <expression> #write_expr {, <expr list>}
   ========================================================= */
int expr_list(char *buffer)
{
    int result = 1;

    if (!expression(buffer))
    {
        result = 0;
    }
    else
    {
        write_expr();
    }

    while (result && next_token(buffer) == COMMA)
    {
        if (!match(COMMA, buffer))
        {
            result = 0;
        }

        if (result && !expression(buffer))
        {
            result = 0;
        }
        else if (result)
        {
            write_expr();
        }
    }

    return result;
}

/* =========================================================
   expression
   production 12:
   <expression> -> <term> {<add_op> <term>} #gen_infix
   ========================================================= */
int expression(char *buffer)
{
    int result = 1;

    if (!term(buffer))
    {
        result = 0;
    }

    while (result && is_add_op(next_token(buffer)))
    {
        char op_text[32];
        Token t = next_token(buffer);

        strcpy(op_text, lookaheadBuffer);

        if (!match(t, buffer))
        {
            result = 0;
        }

        if (result)
        {
            process_op(op_text);
        }

        if (result && !term(buffer))
        {
            result = 0;
        }

        if (result)
        {
            gen_infix();
        }
    }

    return result;
}

/* =========================================================
   term
   production 13:
   <term> -> <factor> {<mult_op> <factor>} #gen_infix
   ========================================================= */
int term(char *buffer)
{
    int result = 1;

    if (!factor(buffer))
    {
        result = 0;
    }

    while (result && is_mult_op(next_token(buffer)))
    {
        char op_text[32];
        Token t = next_token(buffer);

        strcpy(op_text, lookaheadBuffer);

        if (!match(t, buffer))
        {
            result = 0;
        }

        if (result)
        {
            process_op(op_text);
        }

        if (result && !factor(buffer))
        {
            result = 0;
        }

        if (result)
        {
            gen_infix();
        }
    }

    return result;
}

/* =========================================================
   factor
   ========================================================= */
int factor(char *buffer)
{
    Token t = next_token(buffer);
    int result = 0;

    if (t == LPAREN)
    {
        result = match(LPAREN, buffer) &&
                 expression(buffer) &&
                 match(RPAREN, buffer);
    }
    else if (t == MINUSOP)
    {
        /* parse unary minus as usual */
        result = match(MINUSOP, buffer) &&
                 factor(buffer);
    }
    else if (t == ID)
    {
        char idname[128];
        strcpy(idname, lookaheadBuffer);

        result = match(ID, buffer);

        if (result)
        {
            process_id(idname);
        }
    }
    else if (t == INTLITERAL)
    {
        char lit[128];
        strcpy(lit, lookaheadBuffer);

        result = match(INTLITERAL, buffer);

        if (result)
        {
            process_literal(lit);
        }
    }
    else
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: invalid factor '%s' on line %d\n",
                lookaheadBuffer, currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    return result;
}

/* =========================================================
   condition
   ========================================================= */
int condition(char *buffer)
{
    int result = 0;
    Token t = next_token(buffer);

    if (t == LPAREN)
    {
        result = match(LPAREN, buffer) &&
                 condition(buffer) &&
                 match(RPAREN, buffer);

        if (result)
        {
            gen_condition();
        }
    }
    else if (t == MINUSOP)
    {
        result = match(MINUSOP, buffer) &&
                 condition(buffer);

        if (result)
        {
            gen_condition();
        }
    }
    else
    {
        result = c_expression(buffer);

        while (result && is_logical_op(next_token(buffer)))
        {
            char op_text[32];
            Token op = next_token(buffer);

            strcpy(op_text, lookaheadBuffer);

            if (!match(op, buffer))
                result = 0;

            if (result)
                process_op(op_text);

            if (result && !c_expression(buffer))
                result = 0;

            if (result)
                gen_condition();
        }
    }

    return result;
}

/* =========================================================
   c_expression
   production 21
   ========================================================= */
int c_expression(char *buffer)
{
    int result = 1;

    if (!c_term(buffer))
        result = 0;

    while (result && is_rel_op(next_token(buffer)))
    {
        char op_text[32];
        Token op = next_token(buffer);

        strcpy(op_text, lookaheadBuffer);

        if (!match(op, buffer))
            result = 0;

        if (result)
            process_op(op_text);

        if (result && !c_term(buffer))
            result = 0;

        if (result)
            gen_infix();
    }

    return result;
}

/* =========================================================
   c_term
   production 22
   ========================================================= */
int c_term(char *buffer)
{
    int result = 1;

    if (!c_factor(buffer))
        result = 0;

    while (result && is_add_op(next_token(buffer)))
    {
        char op_text[32];
        Token op = next_token(buffer);

        strcpy(op_text, lookaheadBuffer);

        if (!match(op, buffer))
            result = 0;

        if (result)
            process_op(op_text);

        if (result && !c_term(buffer))
            result = 0;

        if (result)
            gen_infix();
    }

    return result;
}

/* =========================================================
   c_factor
   production 23
   ========================================================= */
int c_factor(char *buffer)
{
    int result = 1;

    if (!c_primary(buffer))
        result = 0;

    while (result && is_mult_op(next_token(buffer)))
    {
        char op_text[32];
        Token op = next_token(buffer);

        strcpy(op_text, lookaheadBuffer);

        if (!match(op, buffer))
            result = 0;

        if (result)
            process_op(op_text);

        if (result && !c_factor(buffer))
            result = 0;

        if (result)
            gen_infix();
    }

    return result;
}

/* =========================================================
   c_primary
   ========================================================= */
int c_primary(char *buffer)
{
    Token t = next_token(buffer);
    int result = 0;

    if (t == LPAREN)
    {
        result = match(LPAREN, buffer) &&
                 c_expression(buffer) &&
                 match(RPAREN, buffer);
    }
    else if (t == MINUSOP)
    {
        result = match(MINUSOP, buffer) &&
                 c_primary(buffer);
    }
    else if (t == NOTOP)
    {
        result = match(NOTOP, buffer) &&
                 c_primary(buffer);
    }
    else if (t == ID)
    {
        char idname[128];
        strcpy(idname, lookaheadBuffer);

        result = match(ID, buffer);

        if (result)
        {
            process_id(idname);
        }
    }
    else if (t == INTLITERAL)
    {
        char lit[128];
        strcpy(lit, lookaheadBuffer);

        result = match(INTLITERAL, buffer);

        if (result)
        {
            process_literal(lit);
        }
    }
    else if (t == FALSEOP)
    {
        char op_text[32];
        strcpy(op_text, lookaheadBuffer);

        result = match(FALSEOP, buffer);

        if (result)
        {
            process_op(op_text);
        }
    }
    else if (t == TRUEOP)
    {
        char op_text[32];
        strcpy(op_text, lookaheadBuffer);

        result = match(TRUEOP, buffer);

        if (result)
        {
            process_op(op_text);
        }
    }
    else if (t == NULLOP)
    {
        char op_text[32];
        strcpy(op_text, lookaheadBuffer);

        result = match(NULLOP, buffer);

        if (result)
        {
            process_op(op_text);
        }
    }
    else
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: invalid c_primary '%s' on line %d\n",
                lookaheadBuffer, currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    return result;
}

/* =========================================================
   helper: does token begin a statement?
   ========================================================= */
static int is_statement_start(Token t)
{
    return (t == ID ||
            t == READ ||
            t == WRITE ||
            t == IF ||
            t == WHILE);
}

/* =========================================================
   helper: add operators
   ========================================================= */
static int is_add_op(Token t)
{
    return (t == PLUSOP || t == MINUSOP);
}

/* =========================================================
   helper: multiply operators
   ========================================================= */
static int is_mult_op(Token t)
{
    return (t == MULTOP || t == DIVOP);
}

/* =========================================================
   helper: relational operators for condition()
   ========================================================= */
static int is_rel_op(Token t)
{
    return (t == LESSOP ||
            t == LESSEQUALOP ||
            t == GREATEROP ||
            t == GREATEREQUALOP ||
            t == EQUALOP ||
            t == NOTEQUALOP);
}

/* =========================================================
   helper: logical operators for condition()
   ========================================================= */
static int is_logical_op(Token t)
{
    return (t == ANDOP || t == OROP);
}

/* =========================================================
   helper: clear statement text
   ========================================================= */
static void clear_statement_buffer(void)
{
    statementBuffer[0] = '\0';
}

/* =========================================================
   helper: append token text to current statement
   ========================================================= */
static void append_statement(const char *text)
{
    if (strlen(statementBuffer) + strlen(text) + 2 < sizeof(statementBuffer))
    {
        strcat(statementBuffer, text);
        strcat(statementBuffer, " ");
    }
}

/* =========================================================
   helper: print completed statement to output file
   ========================================================= */
static void print_completed_statement(void)
{
    fprintf(outputFile, "\nCompleted statement: %s\n\n", statementBuffer);
}