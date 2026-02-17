#include "file_util.h"
#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*
 * KEY RULE:
 * - Listing file is for humans (echo of input)
 * - buffer is for the compiler (lexeme of returned token)
 *
 * EVERY token you return MUST have its lexeme in buffer.
 */

/*
* listingFile is a human readable copy of the input program.
* buffer is the token buffer the compiler uses
* Originally, we were echoing characters into the listingFile, but not sotring the exact token text into the buffer
* this fix: for every token returned, we build it in the buffer using add_char and str_copy. Listing echo is separate
*/

static int newLine = 1;
static int errorLineFlag = 0;
static char errors[128];

/*
* These functions are essential to the fix
* getc_echo(): consume a character and echo it to the listing file
* use it when we know that the character is consumed as part of a token
* getc_raw(): this is a look ahead read, we use it when we ungetc() the character, echo it (fputc) AFTER decidign it belongs to the token
*main fix: ensures buffer is the actual token test
*/
static int getc_echo(FILE *in_file, FILE *list_file)
{
    int ch = fgetc(in_file);
    if (ch != EOF)
        fputc(ch, list_file);
    return ch;
}

/* Lookahead char (do NOT echo yet) */
static int getc_raw(FILE *in_file)
{
    return fgetc(in_file);
}

//Function to remove the character from the file stream and push it back
static void ungetc_safe(int ch, FILE *in_file)
{
    if (ch != EOF)
        ungetc(ch, in_file);
}


//start up function, initializes global variables and state for the scanner
void start_up(FILE *in, FILE *out, FILE *list)
{
    (void)in;
    (void)out;
    (void)list;

    currentLine = 1;
    lexicalErrorCount = 0;

    newLine = 1;
    errorLineFlag = 0;
    errors[0] = '\0';
}

/* digits after the first digit is already in buffer */
Token scan_digits(char *buffer, FILE *in_file, FILE *list_file)
{
    //first digit is already in buffer, now we retrieve the next character to check if it is also a digit
    int next = getc_raw(in_file);

    //while the next character is a digit, consume it and add it to the buffer, while also echoing to the listing file
    while (isdigit((unsigned char)next))
    {
        fputc(next, list_file);      // now it is consumed -> echo once
        add_char(buffer, (char)next);
        next = getc_raw(in_file);
    }

    //once we have a non-digit character, push it back to the file stream
    ungetc_safe(next, in_file);
    //return the token for integer literals
    return INTLITERAL;
}

Token scanner(char *buffer, FILE *in_file, FILE *out_file, FILE *list_file)
{
    (void)out_file;

    //clear our buffer
    clear_buffer(buffer);

    /* Print line number at start of each line */
    if (newLine)
    {
        fprintf(list_file, "%d  ", currentLine);
        newLine = 0;
    }

    //create a character variable and initialize by consuming first character from input file and echoing it to the listing file
    int ch = getc_echo(in_file, list_file);

    /* Skip whitespace (no breaks) */
    while (ch != EOF && isspace((unsigned char)ch))
    {
        //when we encounter a newline, check for errors on that line and print them and reset the error flag
        if (ch == '\n')
        {
            if (errorLineFlag)
            {
                fprintf(list_file, "Error. %s not recognized in line %d.\n",
                        errors, currentLine);
                errorLineFlag = 0;
            }

            //increment the line number and print it to the listing file
            currentLine++;
            fprintf(list_file, "%d  ", currentLine);
        }

        //consume the next character and echo it to the listing file
        ch = getc_echo(in_file, list_file);
    }

    //if the character is the EOF, return the SCANEOF token and set the buffer to "EOF"
    if (ch == EOF)
    {
        strcpy(buffer, "EOF");
        return SCANEOF;
    }

    /* Identifiers / reserved words */
    //if the first character is a letter, it is an identifer or reserved word
    if (isalpha((unsigned char)ch))
    {
        //first letter of identifier is stored in the buffer
        add_char(buffer, (char)ch);

        //get the next character to check if it is also a letter or digit
        int next = getc_raw(in_file);
        while (isalnum((unsigned char)next))
        {
            //while the character is a letter or digit, consume it and add to the buffer
            fputc(next, list_file); 
            add_char(buffer, (char)next);
            next = getc_raw(in_file);
        }

        //otherwise, this character is not valid, push it back and do not echo it, since it does not belong to the token
        ungetc_safe(next, in_file);
        return check_reserved(buffer);
    }

    /* Integers */
    //if the first character is a digit, it is an integer literal
    if (isdigit((unsigned char)ch))
    {
        //first digit is stored in the buffer
        add_char(buffer, (char)ch);
        //store remaining digits in buffer
        return scan_digits(buffer, in_file, list_file);
    }

    /* Operators / punctuation */
    //switch statement to check for single character operators and punctuation, and handle multi-character operators with lookahead
    switch (ch)
    {
        //single character tokens: add to buffer and return corresponding token
        case '(': add_char(buffer, '('); return LPAREN;
        case ')': add_char(buffer, ')'); return RPAREN;
        case ';': add_char(buffer, ';'); return SEMICOLON;
        case ',': add_char(buffer, ','); return COMMA;
        case '+': add_char(buffer, '+'); return PLUSOP;
        case '*': add_char(buffer, '*'); return MULTOP;
        case '/': add_char(buffer, '/'); return DIVOP;
        case '!': add_char(buffer, '!'); return NOTOP;
        case '=': add_char(buffer, '='); return EQUALOP;

        /* looking for the :=  operator*/
        case ':':
        {
            int next = getc_raw(in_file);
            if (next == '=')
            {
                fputc(next, list_file);
                strcpy(buffer, ":=");
                return ASSIGNOP;
            }

            ungetc_safe(next, in_file);
            strcpy(buffer, ":");
            lexical_error(buffer, 0, list_file);
            return ERROR;
        }

        /* - or comment -- ... end of line */
        case '-':
        {
            int next = getc_raw(in_file);

            if (next == '-')
            {
                fputc(next, list_file);

                /* consume rest of line as comment */
                int c = getc_echo(in_file, list_file);
                while (c != '\n' && c != EOF)
                    c = getc_echo(in_file, list_file);

                if (c == '\n')
                {
                    if (errorLineFlag)
                    {
                        fprintf(list_file, "Error. %s not recognized in line %d.\n",
                                errors, currentLine);
                        errorLineFlag = 0;
                    }
                    currentLine++;
                    newLine = 1;
                }

                return scanner(buffer, in_file, out_file, list_file);
            }

            ungetc_safe(next, in_file);
            add_char(buffer, '-');
            return MINUSOP;
        }

        /* <, <=, <> */
        case '<':
        {
            add_char(buffer, '<');
            int next = getc_raw(in_file);

            if (next == '=')
            {
                fputc(next, list_file);
                add_char(buffer, '=');
                return LESSEQUALOP;
            }
            else if (next == '>')
            {
                fputc(next, list_file);
                add_char(buffer, '>');
                return NOTEQUALOP;
            }

            ungetc_safe(next, in_file);
            return LESSOP;
        }

        /* >, >= */
        case '>':
        {
            add_char(buffer, '>');
            int next = getc_raw(in_file);

            if (next == '=')
            {
                fputc(next, list_file);
                add_char(buffer, '=');
                return GREATEREQUALOP;
            }

            ungetc_safe(next, in_file);
            return GREATEROP;
        }

        default:
            add_char(buffer, (char)ch);
            lexical_error(buffer, 0, list_file);
            return ERROR;
    }
}

/* IMPORTANT: remove debug printing */
void clear_buffer(char *buffer)
{
    buffer[0] = '\0';
}

/* IMPORTANT: remove debug printing */
Token check_reserved(char *buffer)
{
    if (strcmp(buffer, "begin") == 0) return BEGIN;
    if (strcmp(buffer, "end") == 0) return END;
    if (strcmp(buffer, "read") == 0) return READ;
    if (strcmp(buffer, "write") == 0) return WRITE;
    if (strcmp(buffer, "if") == 0) return IF;
    if (strcmp(buffer, "then") == 0) return THEN;
    if (strcmp(buffer, "else") == 0) return ELSE;
    if (strcmp(buffer, "endif") == 0) return ENDIF;
    if (strcmp(buffer, "while") == 0) return WHILE;
    if (strcmp(buffer, "endwhile") == 0) return ENDWHILE;
    if (strcmp(buffer, "true") == 0) return TRUEOP;
    if (strcmp(buffer, "false") == 0) return FALSEOP;
    if (strcmp(buffer, "null") == 0) return NULLOP;
    if (strcmp(buffer, "and") == 0) return ANDOP;
    if (strcmp(buffer, "or") == 0) return OROP;

    return ID;
}

/*
 * Converts token enum to readable string.
 * Used by G1P2.c when printing output file.
 */
const char* token_to_string(Token token)
{
    switch(token)
    {
        case BEGIN: return "BEGIN";
        case END: return "END";
        case READ: return "READ";
        case WRITE: return "WRITE";
        case IF: return "IF";
        case THEN: return "THEN";
        case ELSE: return "ELSE";
        case ENDIF: return "ENDIF";
        case WHILE: return "WHILE";
        case ENDWHILE: return "ENDWHILE";

        case ID: return "ID";
        case INTLITERAL: return "INTLITERAL";

        case FALSEOP: return "FALSEOP";
        case TRUEOP: return "TRUEOP";
        case NULLOP: return "NULLOP";

        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";

        case ASSIGNOP: return "ASSIGNOP";

        case PLUSOP: return "PLUSOP";
        case MINUSOP: return "MINUSOP";
        case MULTOP: return "MULTOP";
        case DIVOP: return "DIVOP";

        case NOTOP: return "NOTOP";

        case LESSOP: return "LESSOP";
        case LESSEQUALOP: return "LESSEQUALOP";
        case GREATEROP: return "GREATEROP";
        case GREATEREQUALOP: return "GREATEREQUALOP";

        case EQUALOP: return "EQUALOP";
        case NOTEQUALOP: return "NOTEQUALOP";

        case ANDOP: return "ANDOP";
        case OROP: return "OROP";

        case SCANEOF: return "SCANEOF";
        case ERROR: return "ERROR";

        default: return "UNKNOWN";
    }
}

//function to handle lexical errors
int lexical_error(char *buffer, int flag, FILE *list_file)
{
    (void)list_file;

    //if the flag is 0, we are recording a new error
    if (flag == 0)
    {
        strcpy(errors, buffer);
        errorLineFlag = 1;
        lexicalErrorCount++;
    }

    if (flag == 1)
        return lexicalErrorCount;

    return 1;
}

// void token_ident(Token token, char *buffer)
// {
//     (void)token;
//     (void)buffer;
// }

//helper function to add a character to the end of the buffer string
void add_char(char *buffer, char ch)
{
    //find the current length of the buffer, add the character at the end, and null terminate
    int len = (int)strlen(buffer);
    buffer[len] = ch;
    buffer[len + 1] = '\0';
}

//function to print the actual token text to the listing file. For reserved words, we print token name
void print_actual_token(FILE *out, Token tok, char *buffer)
{
    /* Reserved words should print uppercase */
    if (tok <= ENDWHILE)   // all reserved words appear first in enum
    {
        fprintf(out, "%s", token_to_string(tok));
    }
    else
    {
        fprintf(out, "%s", buffer);
    }
}
