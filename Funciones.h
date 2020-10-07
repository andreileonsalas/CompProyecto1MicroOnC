#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#define MAXRESWORDS 4
#define MAXIDLEN 33
///////Defino Variables

typedef enum token_types {
	BEGIN, END, READ, WRITE, ID, INTLITERAL,
	LPAREN, RPAREN, SEMICOLON, COMMA, ASSIGNOP,
	PLUSOP, MINUSOP, SCANEOF, LEXICALERROR
} token;


typedef struct operator { /* for operator */
	enum op { PLUS, MINUS} operator;
}   op_rec;

/* expression types */
enum expr {IDEXPR, LITERALEXPR, TEMPEXPR};

/* for <primary> and <epxression> */
typedef struct expression {
	enum expr kind;
	union {
		char name[MAXIDLEN]; /* for IDEXPR, TEMPEPR */
		int val; /* for LITERALEXPR */
	};
} expr_rec; //REG_EXPRESION en el programa en español

typedef struct {
	char identificador[MAXIDLEN];
	token t; /* t=0, 1, 2, 3 Palabra Reservada, t=ID=4 Identificador */
} RegTS; //Creo que en nuestro caso no necesitamos el token, porque ya las palabras reservadas las manejamos distinto

token current_token;
int flagToken;

// token ReservedWords[MAXRESWORDS]={ BEGIN, END, READ, WRITE };
int token_len;
int ncol;


char token_buffer[MAXIDLEN];

FILE * archivo;
FILE * archivoSalida;

void syntax_error(token tok);
void lexical_error();
void match(token tok);
void system_goal();
void program();
void statement_list();
void statement();
void expression(expr_rec * result);
void primary(expr_rec * result);
void add_op(op_rec * result);
void id_list();
void expr_list();
token next_token();
void id(expr_rec * result);

token scanner();
void buffer_char(int c);
void clear_buffer();
token check_reserved();

expr_rec process_literal();
expr_rec process_id();
op_rec process_op();
void read_id(expr_rec in_var);
void write_expr(expr_rec out_expr);
expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2);
void generate(char * co, char * a, char * b, char * c);
char * extractExpr(expr_rec * reg);
char * extractOp(op_rec * op);
int lookup(char * id, RegTS * TS, token * t);
void enter(char * id, RegTS * TS);
void check_id(char *s);
void assign (expr_rec target, expr_rec source);
void start ();
void finish ();

#endif /* Funciones_H_ */
