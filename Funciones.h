#ifndef FUNCIONES_H_
#define FUNCIONES_H_


/////////////////////////INCLUYO BIBLIOTECAS////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXIDLEN 32
///////Defino Variables

typedef enum {
	BEGIN, END, READ, WRITE, ID, INTLITERAL,
	LPAREN, RPAREN, SEMICOLON, COMMA, ASSIGNOP,
	PLUSOP, MINUSOP, SCANEOF, LEXICALERROR
} token;

typedef struct {
	token kind;
	char name[MAXIDLEN];
	int val;
} expr_rec;

FILE * archive;
FILE * outputArchive;
FILE * sectionData;


char token_buffer[MAXIDLEN];

token current_token;
int flagToken;
int ncol;

typedef struct {
	char identifier[MAXIDLEN];
	token tok;
} RegTS;


//////////////Defino Funciones///////////////////

void match(token token);
void system_goal(void);
void program(void);
void statement_list(void);
void statement(void);
void expression(expr_rec * presul);
void primary(expr_rec * presul);
void add_op(char * presul);
void syntax_error();
void lexical_error();
token next_token(void);
void id(expr_rec * presul);
void expr_list(void);
void id_list(void);

token scanner(void);
int columna(int c);
void buffer_char(int caracter, int posicion);
void clear_buffer(void);
int validation(char* arch, char c);
void initialicefiles();
void joinfiles();
int esEstadoFinal(int e);
token check_reserved();

expr_rec process_literal(void);
expr_rec process_id(void);
char * process_op(void);
void read_id(expr_rec in);
void write_expr(expr_rec out);
expr_rec gen_infix(expr_rec e1, char * op, expr_rec e2);
void generate(char * co, char * a, char * b, char * c);
char * extractExpr(expr_rec * reg);
int lookup(char * id, RegTS * TS, token * t);
void enter(char * id, RegTS * TS);
void check_id(char * s);
void assign(expr_rec izq, expr_rec der);
void start(void);
void finish(void);


#endif /* Funciones_H_ */
