#include "file_util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token scanner(char *buffer, FILE *in_file, FILE *out_file, FILE *list_file)
{
    char ch;
    Token ans;
    char errors[128];

    clear_buffer(buffer);

    //while (peek == blank) advance
    ch = fgetc(in_file);
    while (isspace(ch)) {
        ch = fgetc(in_file);
    }

    //EOF
    if (ch == EOF) {
        strcpy(buffer, "EOF");
        return SCANEOF;
    }

    //if digit--ScanDigits 
    if (isdigit(ch)) {
        ungetc(ch, in_file);
        ans = scan_digits(buffer, in_file);
        return ans;
    }

    /*advance already done, switch(ch) */
    switch (ch) {

        /* case {a,b,...,z} */
        default:
            if (isalpha(ch)) {
                add_char(buffer, ch);

                ch = fgetc(in_file);
                while (isalnum(ch)) {
                    add_char(buffer, ch);
                    ch = fgetc(in_file);
                }

                ungetc(ch, in_file);
                return check_reserved(buffer);// look for i, f, p
            }

            add_char(buffer, ch);
            lexical_error(buffer, 0, list_file);
            return ERROR;

        case ':':
            ch = fgetc(in_file);
            if (ch == '=') {
                strcpy(buffer, ":=");
                return ASSIGNOP;
            }
            ungetc(ch, in_file);
            add_char(buffer, ':');
            lexical_error(buffer, 0, list_file);
            return ERROR;

        case '+':
            add_char(buffer, '+');
            return PLUSOP;

        case '-':
            add_char(buffer, '-');
            return MINUSOP;

        case ';':
            add_char(buffer, ';');
            return SEMICOLON;
    }
}

Token scan_digits(char *buffer, FILE *in_file)
{
    char ch;
    clear_buffer(buffer);

    ch = fgetc(in_file);
    while (isdigit(ch)) {
        add_char(buffer, ch);
        ch = fgetc(in_file);
    }

    ungetc(ch, in_file);
    return INTLITERAL;
}

void clear_buffer(char *buffer)
{
    printf("clearing buffer: %s", buffer);
    buffer[0] = '\0';
    printf("buffer after clear: %s", buffer);
}

//returns the token of a string
Token check_reserved(char *buffer)
{
    printf("checking if reserved: %s\n", buffer);
}
//add character that caused error to an array
int lexical_error(char *buffer, int flag, FILE *list_file)
{
    if (flag == 0)
    {
        //should go to the listing file and print the error message there, not the console
        printf("Lexical Error: '%s' not recognized\n", buffer);
    }
    
}

//meant to identify the string for the given token
void token_ident(Token token, char *buffer)
{
    printf("Token: %d, Buffer: %s\n", token, buffer);
    
}

//adds a character to the buffer
void add_char(char *buffer, char ch)
{
    int len = strlen(buffer);
    buffer[len] = ch;
    buffer[len + 1] = '\0';
}