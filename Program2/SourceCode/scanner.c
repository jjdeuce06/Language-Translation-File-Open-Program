#include "file_util.h"
#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int newLine = 1;
static int errorLineFlag = 0;
static char errors[128];


void start_up(FILE *in, FILE *out, FILE *list)
{
    currentLine = 1;
    lexicalErrorCount = 0;
}

//IDEA to have one return we can iterate through the cases and add the correct token be added to a variable and return the varible at the end 
//DELETE THESE TWO LINES BEFORE UPLOAD
Token scanner(char *buffer, FILE *in_file, FILE *out_file, FILE *list_file)
{	
	long filePos = ftell(in_file); //stores file position pointer for future use
    char ch;
    Token ans;
    
	
    clear_buffer(buffer);

    //while (peek == blank) advance
    ch = fgetc(in_file);
    
	if (ch == EOF){
		strcpy(buffer, "EOF");
     	return SCANEOF;
	}
       
     // prints line number in listing file
    if (newLine){
        fprintf(list_file, "%d  ", currentLine);
        newLine = 0;
    }

    //puts the character into listing file always
    fputc(ch, list_file);

    //handles newline
    if (ch == '\n'){
    	
    	if(errorLineFlag){
    		
    		fprintf(list_file, "Error.  %s not recognized in line %d.\n", errors, currentLine);
    		errorLineFlag = 0;
		}
        currentLine++;
        newLine = 1;
        return scanner(buffer, in_file, out_file, list_file);
    }
    
    // handles whitespace
    while (isspace(ch))
    {
        ch = fgetc(in_file);

        if (ch == EOF)
            return SCANEOF;

        fputc(ch, list_file);

        if (ch == '\n'){
        	
            currentLine++;
            newLine = 1;
            return scanner(buffer, in_file, out_file, list_file);
        }
    }
    
   
    //if digit--ScanDigits 
    if (isdigit(ch)) {
        ungetc(ch, in_file);
        ans = scan_digits(buffer, in_file, list_file);
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
                	fputc(ch, list_file);
                    add_char(buffer, ch);
                    ch = fgetc(in_file);
                }

                ungetc(ch, in_file);
                return check_reserved(buffer);// look for i, f, p
            }

            add_char(buffer, ch);
            lexical_error(buffer, 0, list_file);
            return ERROR;
		
		case '(':
            add_char(buffer, '(');
            return LPAREN;
            
        case ')':
            add_char(buffer, ')');
            return RPAREN;
            
		case ';':
            add_char(buffer, ';');
            return SEMICOLON;
            
        case ',':
            add_char(buffer, ',');
            return COMMA;
            
        case ':':
            ch = fgetc(in_file);
            if (ch == '=') {
            	fputc(ch, list_file);
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
        	ch = fgetc(in_file);
        	
            if (ch == '-') {
            	
            	fputc(ch, list_file); //puts second dash into listing file
            	
            	
                while((ch = fgetc(in_file)) != '\n' && ch != EOF){
                	fputc(ch, list_file);
				}
				
				if (ch == '\n'){
                    fputc('\n', list_file);
                    currentLine++;
                    newLine = 1;
                }
                
				return scanner(buffer, in_file, out_file, list_file); //scans for next token and returns it after ignoring comment
            }
            
            ungetc(ch, in_file);
            add_char(buffer, '-');
            return MINUSOP;

        case '*':
            add_char(buffer, '*');
            return MULTOP;
            
        case '/':
            add_char(buffer, '/');
            return DIVOP;
            
        case '!':
            add_char(buffer, '!');
            return NOTOP;
            
        case '<':
            ch = fgetc(in_file);
            if (ch == '=') {
            	fputc(ch, list_file);
                strcpy(buffer, "<=");
                return LESSEQUALOP;
            }
            if (ch == '>') {
            	fputc(ch, list_file);
                strcpy(buffer, "<>");
                return NOTEQUALOP;
            }
            ungetc(ch, in_file);
            add_char(buffer, '<');
            return LESSOP;
            
            
        case '>':
            ch = fgetc(in_file);
            if (ch == '=') {
            	fputc(ch, list_file);
                strcpy(buffer, ">=");
                return GREATEREQUALOP;
            }
            
            ungetc(ch, in_file);
            add_char(buffer, '>');
            return GREATEROP;
            
        case '=':
    		add_char(buffer, '=');
    		return EQUALOP;
            
    }
}

Token scan_digits(char *buffer, FILE *in_file, FILE *list_file)
{
    char ch;
    //clear_buffer(buffer); shouldnt clear buffer after passed

    ch = fgetc(in_file);
    
    while (isdigit(ch)){
    	
        add_char(buffer, ch);
        fputc(ch, list_file);
        ch = fgetc(in_file);
    }

    ungetc(ch, in_file);
    return INTLITERAL;
}

void clear_buffer(char *buffer)
{
    printf("clearing buffer: %s\n", buffer);
    buffer[0] = '\0';
    printf("buffer after clear: %s\n", buffer);
}

//returns the token of a string
Token check_reserved(char *buffer)
{
    printf("checking if reserved: %s\n", buffer);
    
    //SHould this only Check lowercase letters??  should we convert the buffer to lowercase just incase?
    if (strcmp(buffer, "begin") == 0)
        return BEGIN;

    else if (strcmp(buffer, "end") == 0)
        return END;

    else if (strcmp(buffer, "read") == 0)
        return READ;

    else if (strcmp(buffer, "write") == 0)
        return WRITE;

    else if (strcmp(buffer, "if") == 0)
        return IF;

    else if (strcmp(buffer, "then") == 0)
        return THEN;

    else if (strcmp(buffer, "else") == 0)
        return ELSE;

    else if (strcmp(buffer, "endif") == 0)
        return ENDIF;

    else if (strcmp(buffer, "while") == 0)
        return WHILE;

    else if (strcmp(buffer, "endwhile") == 0)
        return ENDWHILE;

    else if (strcmp(buffer, "true") == 0)
        return TRUEOP;

    else if (strcmp(buffer, "false") == 0)
        return FALSEOP;

    else if (strcmp(buffer, "null") == 0)
        return NULLOP;

    else if (strcmp(buffer, "and") == 0)
        return ANDOP;

    else if (strcmp(buffer, "or") == 0)
        return OROP;

    else
        return ID; // if the word is not any reserved words then it is an ID
}

//add character that caused error to an array
int lexical_error(char *buffer, int flag, FILE *list_file)
{
    if (flag == 0)
    {
    
        strcpy(errors, buffer);
        errorLineFlag = 1;
        lexicalErrorCount++;
    }

    
    if(flag == 1){
    	return lexicalErrorCount;	
	}
    
    return 1;
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

const char* token_to_string(Token token)
{
    switch(token)
    {
        case BEGIN:
		 return "BEGIN";
        case END:
		 return "END";
        case READ:
		 return "READ";
        case WRITE:
		 return "WRITE";
        case IF:
		 return "IF";
        case THEN:
		 return "THEN";
        case ELSE:
		 return "ELSE";
        case ENDIF:
		 return "ENDIF";
        case WHILE:
		 return "WHILE";
        case ENDWHILE:
		 return "ENDWHILE";
        case ID:
		 return "ID";
        case INTLITERAL:
		 return "INTLITERAL";
        case FALSEOP:
		 return "FALSEOP";
        case TRUEOP:
		 return "TRUEOP";
        case NULLOP:
		 return "NULLOP";
        case LPAREN:
		 return "LPAREN";
        case RPAREN:
		 return "RPAREN";
        case SEMICOLON:
		 return "SEMICOLON";
        case COMMA:
		 return "COMMA";
        case ASSIGNOP:
		 return "ASSIGNOP";
        case PLUSOP:
		 return "PLUSOP";
        case MINUSOP:
		 return "MINUSOP";
        case MULTOP:
		 return "MULTOP";
        case DIVOP:
		 return "DIVOP";
        case NOTOP:
		 return "NOTOP";
        case LESSOP:
		 return "LESSOP";
        case LESSEQUALOP:
		 return "LESSEQUALOP";
        case GREATEROP:
		 return "GREATEROP";
        case GREATEREQUALOP:
		 return "GREATEREQUALOP";
        case EQUALOP:
		 return "EQUALOP";
        case NOTEQUALOP:
		 return "NOTEQUALOP";
        case ANDOP:
		 return "ANDOP";
        case OROP:
		 return "OROP";

        case SCANEOF:
		 return "SCANEOF";
        case ERROR:
		 return "ERROR";

        default:
		 return "UNKNOWNTYPE";
    }
}