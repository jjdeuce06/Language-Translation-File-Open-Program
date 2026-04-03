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
static char errors[512];

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

void runScanner(Token token, FILE* inputFile, FILE* outputFile, FILE* listingFile, char* buffer){
    do{
		token = scanner(buffer, inputFile, outputFile, listingFile);

		fprintf(outputFile,
				"token number:%5d    token type: %-12s    actual token: ",
				(int)token,
				token_to_string(token));

		print_actual_token(outputFile, token, buffer);
		fprintf(outputFile, "\n");

	} while (token != SCANEOF);
	
	fprintf(listingFile, "\n\n%d 	Lexical Errors.\n", lexical_error(buffer, 1, listingFile));
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

    Token tok = ERROR; // default token

    // clear our buffer
    clear_buffer(buffer);

    /* Print line number at start of each line */
    if (newLine)
    {
        fprintf(list_file, "%d  ", currentLine);
        newLine = 0;
    }

    // create a character variable and initialize by consuming first character from input file and echoing it
    int ch = getc_echo(in_file, list_file);

    /* Skip whitespace (no breaks) */
    while (ch != EOF && isspace((unsigned char)ch))
    {
        // handle errors at end of line
        if (ch == '\n')
        {
            if (errorLineFlag)
            {
                fprintf(list_file, "ERROR %s not recognized in line %d.\n",
                        errors, currentLine);
                errorLineFlag = 0;
                errors[0] = '\0';
            }

            currentLine++;
            fprintf(list_file, "%d  ", currentLine);
        }

        ch = getc_echo(in_file, list_file);
    }

    /* Handle EOF */
    if (ch == EOF)
    {
        if (errorLineFlag)
        {
            fprintf(list_file, "\nERROR %s not recognized in line %d.\n",
                    errors, currentLine);
            errorLineFlag = 0;
            errors[0] = '\0';
        }
        strcpy(buffer, "EOF");
        tok = SCANEOF;
        return tok; // single return at end is fine here
    }

    /* Identifiers / reserved words */
    if (isalpha((unsigned char)ch))
    {
        add_char(buffer, (char)ch);

        int next = getc_raw(in_file);
        while (isalnum((unsigned char)next))
        {
            fputc(next, list_file);
            add_char(buffer, (char)next);
            next = getc_raw(in_file);
        }
        ungetc_safe(next, in_file);
		
		to_lower_str(buffer); //Turns the buffer contents into lowercase so the check reserved word function works properly.
        tok = check_reserved(buffer);
        return tok; // identifiers handled here
    }

    /* Integers */
    if (isdigit((unsigned char)ch))
    {
        add_char(buffer, (char)ch);
        tok = scan_digits(buffer, in_file, list_file);
        return tok;
    }

    /* Operators / punctuation */
    else{
        tok = handle_operator(buffer, ch, in_file, out_file, list_file);
    }

    return tok;
}

Token handle_operator(char *buffer, int ch, FILE *in_file, FILE *out_file, FILE *list_file)
{
    Token tok = ERROR; // default token

    // single-character tokens
    if (ch == '(')       tok = (add_char(buffer, '('), LPAREN);
    else if (ch == ')')  tok = (add_char(buffer, ')'), RPAREN);
    else if (ch == ';')  tok = (add_char(buffer, ';'), SEMICOLON);
    else if (ch == ',')  tok = (add_char(buffer, ','), COMMA);
    else if (ch == '+')  tok = (add_char(buffer, '+'), PLUSOP);
    else if (ch == '*')  tok = (add_char(buffer, '*'), MULTOP);
    else if (ch == '/')  tok = (add_char(buffer, '/'), DIVOP);
    else if (ch == '!')  tok = (add_char(buffer, '!'), NOTOP);
    else if (ch == '=')  tok = (add_char(buffer, '='), EQUALOP);

    // multi-character / lookahead operators
    else if (ch == ':')
    {
        int next = getc_raw(in_file);
        if (next == '=')
        {
            fputc(next, list_file);
            strcpy(buffer, ":=");
            tok = ASSIGNOP;
        }
        else
        {
            ungetc_safe(next, in_file);
            strcpy(buffer, ":");
            lexical_error(buffer, 0, list_file);
            tok = ERROR;
        }
    }
    else if (ch == '-')
    {
        int next = getc_raw(in_file);
        if (next == '-') // comment
        {
            fputc(next, list_file);
            int c = getc_echo(in_file, list_file);
            while (c != '\n' && c != EOF)
                c = getc_echo(in_file, list_file);

            if (c == '\n')
            {
                if (errorLineFlag)
                {
                    fprintf(list_file, "ERROR %s not recognized in line %d.\n",
                            errors, currentLine);
                    errorLineFlag = 0;
                    errors[0] = '\0';
                }
                currentLine++;
                newLine = 1;
            }

            tok = scanner(buffer, in_file, out_file, list_file);
        }
        else
        {
            ungetc_safe(next, in_file);
            add_char(buffer, '-');
            tok = MINUSOP;
        }
    }
    else if (ch == '<')
    {
        add_char(buffer, '<');
        int next = getc_raw(in_file);

        if (next == '=')
        {
            fputc(next, list_file);
            add_char(buffer, '=');
            tok = LESSEQUALOP;
        }
        else if (next == '>')
        {
            fputc(next, list_file);
            add_char(buffer, '>');
            tok = NOTEQUALOP;
        }
        else
        {
            ungetc_safe(next, in_file);
            tok = LESSOP;
        }
    }
    else if (ch == '>')
    {
        add_char(buffer, '>');
        int next = getc_raw(in_file);
        if (next == '=')
        {
            fputc(next, list_file);
            add_char(buffer, '=');
            tok = GREATEREQUALOP;
        }
        else
        {
            ungetc_safe(next, in_file);
            tok = GREATEROP;
        }
    }
    else // unknown character
    {
        add_char(buffer, (char)ch);
        lexical_error(buffer, 0, list_file);
        tok = ERROR;
    }

    return tok;
}


/* IMPORTANT: remove debug printing */
void clear_buffer(char *buffer)
{
    buffer[0] = '\0';
}

/* IMPORTANT: remove debug printing */
Token check_reserved(char *buffer)
{
    Token tok = ID;  // default value

    if (strcmp(buffer, "begin") == 0) tok = BEGIN;
    else if (strcmp(buffer, "end") == 0) tok = END;
    else if (strcmp(buffer, "read") == 0) tok = READ;
    else if (strcmp(buffer, "write") == 0) tok = WRITE;
    else if (strcmp(buffer, "if") == 0) tok = IF;
    else if (strcmp(buffer, "then") == 0) tok = THEN;
    else if (strcmp(buffer, "else") == 0) tok = ELSE;
    else if (strcmp(buffer, "endif") == 0) tok = ENDIF;
    else if (strcmp(buffer, "while") == 0) tok = WHILE;
    else if (strcmp(buffer, "endwhile") == 0) tok = ENDWHILE;
    else if (strcmp(buffer, "true") == 0) tok = TRUEOP;
    else if (strcmp(buffer, "false") == 0) tok = FALSEOP;
    else if (strcmp(buffer, "null") == 0) tok = NULLOP;
    else if (strcmp(buffer, "and") == 0) tok = ANDOP;
    else if (strcmp(buffer, "or") == 0) tok = OROP;

    return tok;
}

/*
 * Converts token enum to readable string.
 * Used by G1P2.c when printing output file.
 */
const char* token_to_string(Token token)
{
    const char* str = "UNKNOWN";  // default value

    switch(token)
    {
        case BEGIN:
            if (token == BEGIN) str = "BEGIN";
        case END:
            if (token == END) str = "END";
        case READ:
            if (token == READ) str = "READ";
        case WRITE:
            if (token == WRITE) str = "WRITE";
        case IF:
            if (token == IF) str = "IF";
        case THEN:
            if (token == THEN) str = "THEN";
        case ELSE:
            if (token == ELSE) str = "ELSE";
        case ENDIF:
            if (token == ENDIF) str = "ENDIF";
        case WHILE:
            if (token == WHILE) str = "WHILE";
        case ENDWHILE:
            if (token == ENDWHILE) str = "ENDWHILE";
        case ID:
            if (token == ID) str = "ID";
        case INTLITERAL:
            if (token == INTLITERAL) str = "INTLITERAL";
        case FALSEOP:
            if (token == FALSEOP) str = "FALSEOP";
        case TRUEOP:
            if (token == TRUEOP) str = "TRUEOP";
        case NULLOP:
            if (token == NULLOP) str = "NULLOP";
        case LPAREN:
            if (token == LPAREN) str = "LPAREN";
        case RPAREN:
            if (token == RPAREN) str = "RPAREN";
        case SEMICOLON:
            if (token == SEMICOLON) str = "SEMICOLON";
        case COMMA:
            if (token == COMMA) str = "COMMA";
        case ASSIGNOP:
            if (token == ASSIGNOP) str = "ASSIGNOP";
        case PLUSOP:
            if (token == PLUSOP) str = "PLUSOP";
        case MINUSOP:
            if (token == MINUSOP) str = "MINUSOP";
        case MULTOP:
            if (token == MULTOP) str = "MULTOP";
        case DIVOP:
            if (token == DIVOP) str = "DIVOP";
        case NOTOP:
            if (token == NOTOP) str = "NOTOP";
        case LESSOP:
            if (token == LESSOP) str = "LESSOP";
        case LESSEQUALOP:
            if (token == LESSEQUALOP) str = "LESSEQUALOP";
        case GREATEROP:
            if (token == GREATEROP) str = "GREATEROP";
        case GREATEREQUALOP:
            if (token == GREATEREQUALOP) str = "GREATEREQUALOP";
        case EQUALOP:
            if (token == EQUALOP) str = "EQUALOP";
        case NOTEQUALOP:
            if (token == NOTEQUALOP) str = "NOTEQUALOP";
        case ANDOP:
            if (token == ANDOP) str = "ANDOP";
        case OROP:
            if (token == OROP) str = "OROP";
        case SCANEOF:
            if (token == SCANEOF) str = "SCANEOF";
        case ERROR:
            if (token == ERROR) str = "ERROR";
    }
    return str;
}
//function to handle lexical errors
int lexical_error(char *buffer, int flag, FILE *list_file)
{
    (void)list_file;

    //if the flag is 0, we are recording a new error
    if (flag == 0)
    {
        if (errorLineFlag == 0)
        {
            errors[0] = '\0';   //clear for new line
        }

        if (strlen(errors) > 0)
        {
            strcat(errors, ", ");
        }

        strcat(errors, buffer);

        errorLineFlag = 1;
        lexicalErrorCount++;
    }

    if (flag == 1){
    	return lexicalErrorCount;
	}
    return 1;
}

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

void to_lower_str(char *s)
{
    int i = 0;
    while (s[i] != '\0')
    {
        s[i] = (char)tolower((unsigned char)s[i]);
        i++;
    }
}
