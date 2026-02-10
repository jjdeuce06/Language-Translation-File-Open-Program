#include "file_util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Sudo code from lecture 3-- Gaining Understanding*/

/*
function Scanner() returns Token{

    while( s.peek() == blank){

        s.advance()

        if (s.EOF()){
            ans = $
        }else{
            if(s.peek()={0, 1,... 9}){
                ans = ScanDigits()
            }else{
                ch = s.advance()
                
                switch(ch){
                    case{a,b,  ..., z}-{i,f,p} remove these i, f, p
                        ans.type = id
                        ans.val = ch
                    case f:
                        ans.type = floatdcl
                    case i:
                        ans.type = intdcl
                    case p:
                        ans.type = print
                    case =
                        ans.type = assign
                    case +
                        ans.type = plus
                    case -
                        ans.type = minus
                    case default
                    LexicalError() 
                }
                return(ans)   
            }  
        }
    }
}
*/
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



/*
function ScanDigits() return token{
    tok.val = ""

    while(s.peek()= {0, 1, ...., 9}){
        tok.val = tok.val + s.advance()


        if(s.peek != "."){
            tok.type = inum
        }else{
            tok.type = fnum
            tok.val = tok.val + s.advance()
            while(s.peek()= {0, 1, ...., 9}){
                tok.val = tok.val + s.advance()
            }
        }
        return (tok)
    }
}
*/
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

