#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "file_util.h"

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
   - assignment wants lexical + syntax totals at the end
   - semantic included too because your instructions mention it
   ========================================================= */
void parser_finish(void)
{
    fprintf(listingFile, "\n\nLexical Errors: %d\n", lexicalErrorCount);
    fprintf(listingFile, "Syntax Errors: %d\n", syntaxErrorCount);
    fprintf(listingFile, "Semantic Errors: %d\n", semanticErrorCount);
}

/* =========================================================
   advance_token
   - destructive scanner call
   - stores next token and its lexeme into parser lookahead
   ========================================================= */
static void advance_token(void)
{
    lookahead = scanner(lookaheadBuffer, inputFile, outputFile, listingFile);
}

/* =========================================================
   next_token
   - non-destructive from parser's perspective
   - returns saved lookahead token and copies lexeme out
   ========================================================= */
Token next_token(char *buffer)
{
    strcpy(buffer, lookaheadBuffer);
    return lookahead;
}

/* =========================================================
   match
   - checks whether current lookahead == expected
   - prints expected token and actual token to output file
   - appends token text to statement buffer
   - if matched, advances lookahead
   - if semicolon matched, prints completed statement
   ========================================================= */
int match(Token expected, char *buffer)
{
    int result = 1; 

    /* Give caller the current lexeme text */
    strcpy(buffer, lookaheadBuffer);

    /* Required parser output: expected token vs actual token */
    fprintf(outputFile,
            "Parser match -> expected: %-14s actual: %-14s lexeme: %s\n",token_to_string(expected),
            token_to_string(lookahead),lookaheadBuffer);

    /* Save token text into statement buffer */
    append_statement(lookaheadBuffer);

    /* Mismatch = syntax error */
    if (lookahead != expected)
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: expected %s but found %s (lexeme: %s) on line %d\n",token_to_string(expected),
                token_to_string(lookahead),lookaheadBuffer,currentLine);
        syntaxErrorCount++;
        result = 0;  // mark failure
    }

    /* If statement ended, print it now */
    if (lookahead == SEMICOLON)
    {
        print_completed_statement();
        clear_statement_buffer();
    }

    /* Advance token only if not EOF and last match succeeded */
    if (result)
    {
        advance_token();
    }
    return result; 
}

/* =========================================================
   recover_to_semicolon
   - keep scanning until semicolon or a safe block-ending token
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

    /* If we stopped at semicolon, consume it so parsing can continue */
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
   <system goal> -> <program> SCANEOF
   ========================================================= */
int system_goal(char *buffer)
{
    int ok = 1;

    if (!program(buffer))
        ok = 0;

    if (!match(SCANEOF, buffer))
        ok = 0;

    return ok;
}

/* =========================================================
   program
   production 1:
   <program> -> BEGIN <statement list> END
   ========================================================= */
int program(char *buffer)
{
    int ok = 1;

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
   production 2:
   <statement list> -> <statement> {<statement list>}
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
   productions:
   3. ID := <expression>;
   4. READ ( <id list> );
   5. WRITE ( <expr list> );
   6. IF ( <condition> ) THEN <StatementList> <IFTail>
   9. WHILE ( <condition> ) <StatementList> ENDWHILE
   ========================================================= */
int statement(char *buffer)
{
    Token t = next_token(buffer);
    int result = 0; // default failure

    if (t == ID)
    {
        result = match(ID, buffer) &&
                 match(ASSIGNOP, buffer) &&
                 expression(buffer) &&
                 match(SEMICOLON, buffer);
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
        result = match(WHILE, buffer) &&
                 match(LPAREN, buffer) &&
                 condition(buffer) &&
                 match(RPAREN, buffer) &&
                 statement_list(buffer) &&
                 match(ENDWHILE, buffer);
    }
    else
    {
        fprintf(listingFile,"\nSYNTAX ERROR: invalid start of statement on line %d\n", currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    return result; 
}
/* =========================================================
   if_tail
   productions:
   7. <IFTail> -> ELSE <StatementList> ENDIF
   8. <IFTail> -> ENDIF
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
    else if (t == ENDIF){
        result = match(ENDIF, buffer);
    }
    else
    {
        fprintf(listingFile,"\nSYNTAX ERROR: expected ELSE or ENDIF on line %d\n",currentLine);
        syntaxErrorCount++;
        result = 0;
    }
    return result; 
}

/* =========================================================
   id_list
   production 10:
   <id list> -> ID {, <id list>}
   practical loop version:
   ID { , ID }
   ========================================================= */
int id_list(char *buffer)
{
    int result = 1;
    if (!match(ID, buffer))
    {
        result = 0;
    }
    while (result && next_token(buffer) == COMMA)
    {
        if (!match(COMMA, buffer))
        {
            result = 0;
        }
        if (result && !match(ID, buffer))
        {
            result = 0;
        }
    }
    return result;
}

/* =========================================================
   expr_list
   production 11:
   <expr list> -> <expression> {, <expr list>}
   practical loop version:
   expression { , expression }
   ========================================================= */
int expr_list(char *buffer)
{
    int result = 1; 
    if (!expression(buffer))
    {
        result = 0;
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
    }
    return result;
}

/* =========================================================
   expression
   production 12:
   <expression> -> <term> {<add_op> <term>}
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
        Token t = next_token(buffer);

        if (!match(t, buffer))
        {
            result = 0;
        }

        if (result && !term(buffer))
        {
            result = 0;
        }
    }
    return result; 
}

/* =========================================================
   term
   production 13:
   <term> -> <factor> {<mult_op> <factor>}
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
        Token t = next_token(buffer);

        if (!match(t, buffer))
        {
            result = 0;
        }

        if (result && !factor(buffer))
        {
            result = 0;
        }
    }

    return result; 
}

/* =========================================================
   factor
   productions handled here:
   14. ( <expression> )
   15. - <factor>
   16. ID
   17. INTLITERAL
   ========================================================= */
int factor(char *buffer)
{
    Token t = next_token(buffer);
    int result = 0; // default

    if (t == ID)
    {
        result = match(ID, buffer);
    }
    else if (t == INTLITERAL)
    {
        result = match(INTLITERAL, buffer);
    }
    else if (t == LPAREN)
    {
        result = match(LPAREN, buffer) &&
                 expression(buffer) &&
                 match(RPAREN, buffer);
    }
    else if (t == MINUSOP)
    {
        result = match(MINUSOP, buffer) &&
                 factor(buffer);
    }
    else if (t == NOTOP)
    {
        result = match(NOTOP, buffer) &&
                 factor(buffer);
    }
    else if (t == TRUEOP)
    {
        result = match(TRUEOP, buffer);
    }
    else if (t == FALSEOP)
    {
        result = match(FALSEOP, buffer);
    }
    else if (t == NULLOP)
    {
        result = match(NULLOP, buffer);
    }
    else
    {
        fprintf(listingFile,"\nSYNTAX ERROR: invalid factor '%s' on line %d\n",buffer, currentLine);
        syntaxErrorCount++;
        result = 0;
    }

    return result;
}
/* =========================================================
   condition
   parse an expression, then allow relational/logical chaining.
   ========================================================= */
int condition(char *buffer)
{
    int result = 1;

    if (!expression(buffer))
    {
        result = 0;
    }

    while (result && is_rel_or_logical_op(next_token(buffer)))
    {
        Token t = next_token(buffer);

        if (!match(t, buffer))
        {
            result = 0;
        }

        if (result && !expression(buffer))
        {
            result = 0;
        }
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
   helper: relational or logical operators for condition()
   ========================================================= */
static int is_rel_or_logical_op(Token t)
{
    return (t == LESSOP ||
            t == LESSEQUALOP ||
            t == GREATEROP ||
            t == GREATEREQUALOP ||
            t == EQUALOP ||
            t == NOTEQUALOP ||
            t == ANDOP ||
            t == OROP);
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
    /* Avoid buffer overflow */
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
    fprintf(outputFile, "Completed statement: %s\n", statementBuffer);
}