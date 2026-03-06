#ifndef SCANNER_H
#define SCANNER_H
#include <stdio.h>


	//---Program 2------------

	typedef enum {
		/* Keywords */
		BEGIN, 		//0
		END,		//1
		READ,		//2
		WRITE,		//3
		IF,			//4
		THEN,		//5
		ELSE,		//6
		ENDIF,		//7
		WHILE,		//8
		ENDWHILE,	//9

		/* Identifiers & literals */
		ID,			//10
		INTLITERAL,	//11

		/* Boolean / null literals */
		FALSEOP,	//12
		TRUEOP,		//13
		NULLOP,		//14

		/* Parentheses & punctuation */
		LPAREN,        //15 ( 
		RPAREN,        //16 )
		SEMICOLON,     //17 ;
		COMMA,         //18 ,

		/* Operators */
		ASSIGNOP,        //19 :=
		PLUSOP,          //20 +
		MINUSOP,         //21 -
		MULTOP,          //22 *
		DIVOP,           //23 /
		NOTOP,           //24 !
		LESSOP,          //25 <
		LESSEQUALOP,     //26 <=
		GREATEROP,       //27 >
		GREATEREQUALOP,  //28 >=
		EQUALOP,         //29 =
		NOTEQUALOP,      //30 <>
		ANDOP,           //31 and
		OROP,            //32 or

		/* Special */
		SCANEOF,		//33
		ERROR			//34

	} Token;
	
/* ---not sure the type yet? void??----Frome lecture 7*/
	void start_up(FILE *, FILE *, FILE *);
	void clear_buffer(char *);
	Token check_reserved(char *);
	int lexical_error(char *, int flag, FILE *);
	void token_ident(Token, char *);
	void add_char(char *, char);
	void print_actual_token(FILE *out, Token tok, char *buffer);
	void runScanner(Token token, FILE* inputFile, FILE* outputFile, FILE* listingFile, char* buffer);

	Token scanner(char * buffer, FILE * in_file, FILE * out_file, FILE * list_file);
	Token scan_digits(char *buffer, FILE *in_file, FILE *list_file);

	const char* token_to_string(Token);

#endif