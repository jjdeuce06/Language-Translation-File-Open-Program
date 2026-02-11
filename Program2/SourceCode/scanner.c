#include "file_util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token scanner(char *buffer, FILE *in_file, FILE *out_file, FILE *list_file)
{
    char ch;
    Token ans;

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

void clear_buffer(char *){}
Token check_reserved(char *){}
int lexical_error(char *, int flag, FILE *){}
void token_ident(Token, char *){}
void add_char(char *, char){}