#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "file_util.h"

/*
 * Parser-specific counters.
 * lexicalErrorCount is already tracked by scanner/file_util.
 */
int syntaxErrorCount = 0;
int semanticErrorCount = 0;

/*
 * LL(1) parser lookahead:
 * instead of rewinding the scanner, we keep one token saved here.
 * This makes next_token() non-destructive from the parser's perspective.
 */
static Token lookahead;
static char lookaheadBuffer[128];

/*
 * Holds the current statement text so that when ';' is matched,
 * we can print the completed statement to the output file.
 */
static char statementBuffer[1024];

/* ---------- private helper prototypes ---------- */
static void advance_token(void);
static int is_statement_start(Token t);
static int is_add_op(Token t);
static int is_mult_op(Token t);
static int is_rel_or_logical_op(Token t);
static void clear_statement_buffer(void);
static void append_statement(const char *text);
static void print_completed_statement(void);

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
    /* Give caller the current lexeme text */
    strcpy(buffer, lookaheadBuffer);

    /* Required parser output: expected token vs actual token */
    fprintf(outputFile,
            "Parser match -> expected: %-14s actual: %-14s lexeme: %s\n",
            token_to_string(expected),
            token_to_string(lookahead),
            lookaheadBuffer);

    /* Save token text into statement buffer */
    append_statement(lookaheadBuffer);

    /* Mismatch = syntax error */
    if (lookahead != expected)
    {
        fprintf(listingFile,
                "\nSYNTAX ERROR: expected %s but found %s (lexeme: %s) on line %d\n",
                token_to_string(expected),
                token_to_string(lookahead),
                lookaheadBuffer,
                currentLine);

        syntaxErrorCount++;
        return 0;
    }

    /* If statement ended, print it now */
    if (lookahead == SEMICOLON)
    {
        print_completed_statement();
        clear_statement_buffer();
    }

    /* Advance after successful match */
    advance_token();
    return 1;
}

/* =========================================================
   recover_to_semicolon
   - simple panic-mode recovery
   - keep scanning until semicolon or a safe block-ending token
   - this follows the assignment direction to continue instead of quitting
   ========================================================= */
int recover_to_semicolon(char *buffer)
{
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
        match(SEMICOLON, buffer);
    }

    clear_statement_buffer();
    return 1;
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
   Practical implementation:
   keep parsing statements while next token begins a statement
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

    switch (t)
    {
        case ID:
            if (!match(ID, buffer)) return 0;
            if (!match(ASSIGNOP, buffer)) return 0;
            if (!expression(buffer)) return 0;
            if (!match(SEMICOLON, buffer)) return 0;
            return 1;

        case READ:
            if (!match(READ, buffer)) return 0;
            if (!match(LPAREN, buffer)) return 0;
            if (!id_list(buffer)) return 0;
            if (!match(RPAREN, buffer)) return 0;
            if (!match(SEMICOLON, buffer)) return 0;
            return 1;

        case WRITE:
            if (!match(WRITE, buffer)) return 0;
            if (!match(LPAREN, buffer)) return 0;
            if (!expr_list(buffer)) return 0;
            if (!match(RPAREN, buffer)) return 0;
            if (!match(SEMICOLON, buffer)) return 0;
            return 1;

        case IF:
            if (!match(IF, buffer)) return 0;
            if (!match(LPAREN, buffer)) return 0;
            if (!condition(buffer)) return 0;
            if (!match(RPAREN, buffer)) return 0;
            if (!match(THEN, buffer)) return 0;
            if (!statement_list(buffer)) return 0;
            if (!if_tail(buffer)) return 0;
            return 1;

        case WHILE:
            if (!match(WHILE, buffer)) return 0;
            if (!match(LPAREN, buffer)) return 0;
            if (!condition(buffer)) return 0;
            if (!match(RPAREN, buffer)) return 0;
            if (!statement_list(buffer)) return 0;
            if (!match(ENDWHILE, buffer)) return 0;
            return 1;

        default:
            fprintf(listingFile,
                    "\nSYNTAX ERROR: invalid start of statement on line %d\n",
                    currentLine);
            syntaxErrorCount++;
            return 0;
    }
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

    if (t == ELSE)
    {
        if (!match(ELSE, buffer)) return 0;
        if (!statement_list(buffer)) return 0;
        if (!match(ENDIF, buffer)) return 0;
        return 1;
    }
    else if (t == ENDIF)
    {
        return match(ENDIF, buffer);
    }

    fprintf(listingFile,
            "\nSYNTAX ERROR: expected ELSE or ENDIF on line %d\n",
            currentLine);
    syntaxErrorCount++;
    return 0;
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
    if (!match(ID, buffer))
        return 0;

    while (next_token(buffer) == COMMA)
    {
        if (!match(COMMA, buffer)) return 0;
        if (!match(ID, buffer)) return 0;
    }

    return 1;
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
    if (!expression(buffer))
        return 0;

    while (next_token(buffer) == COMMA)
    {
        if (!match(COMMA, buffer)) return 0;
        if (!expression(buffer)) return 0;
    }

    return 1;
}

/* =========================================================
   expression
   production 12:
   <expression> -> <term> {<add_op> <term>}
   ========================================================= */
int expression(char *buffer)
{
    if (!term(buffer))
        return 0;

    while (is_add_op(next_token(buffer)))
    {
        Token t = next_token(buffer);

        if (!match(t, buffer)) return 0;
        if (!term(buffer)) return 0;
    }

    return 1;
}

/* =========================================================
   term
   production 13:
   <term> -> <factor> {<mult_op> <factor>}
   ========================================================= */
int term(char *buffer)
{
    if (!factor(buffer))
        return 0;

    while (is_mult_op(next_token(buffer)))
    {
        Token t = next_token(buffer);

        if (!match(t, buffer)) return 0;
        if (!factor(buffer)) return 0;
    }

    return 1;
}

/* =========================================================
   factor
   productions handled here:
   14. ( <expression> )
   15. - <factor>
   16. ID
   17. INTLITERAL

   Also supports !, TRUE, FALSE, NULL because your scanner has them
   and your grammar later includes primary forms for them.
   ========================================================= */
int factor(char *buffer)
{
    Token t = next_token(buffer);

    switch (t)
    {
        case ID:
            return match(ID, buffer);

        case INTLITERAL:
            return match(INTLITERAL, buffer);

        case LPAREN:
            if (!match(LPAREN, buffer)) return 0;
            if (!expression(buffer)) return 0;
            if (!match(RPAREN, buffer)) return 0;
            return 1;

        case MINUSOP:
            if (!match(MINUSOP, buffer)) return 0;
            return factor(buffer);

        case NOTOP:
            if (!match(NOTOP, buffer)) return 0;
            return factor(buffer);

        case TRUEOP:
            return match(TRUEOP, buffer);

        case FALSEOP:
            return match(FALSEOP, buffer);

        case NULLOP:
            return match(NULLOP, buffer);

        default:
            fprintf(listingFile,
                    "\nSYNTAX ERROR: invalid factor '%s' on line %d\n",
                    buffer, currentLine);
            syntaxErrorCount++;
            return 0;
    }
}

/* =========================================================
   condition
   Basic practical condition parser for your tokens.
   This keeps things workable with your grammar and scanner:
   parse an expression, then allow relational/logical chaining.
   ========================================================= */
int condition(char *buffer)
{
    if (!expression(buffer))
        return 0;

    while (is_rel_or_logical_op(next_token(buffer)))
    {
        Token t = next_token(buffer);

        if (!match(t, buffer)) return 0;
        if (!expression(buffer)) return 0;
    }

    return 1;
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