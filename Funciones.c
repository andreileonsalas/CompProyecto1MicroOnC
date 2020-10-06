
#include "Funciones.h"
// #define _CRT_SECURE_NO_WARNINGS


//Tabla de simbolos
RegTS TS[1000] = { {"start", BEGIN}, {"end", END}, {"read", READ}, {"write", WRITE}, {"$", 99} };
int numeroVariableTemporal=1;

/* ------------------------------------------PARSER---------------------------------------------------------------*/

void syntax_error(token tok)
{
	printf("Syntax error at \"%c\" ", tok);
	exit(1);
}

void lexical_error()
{
	printf("Lexical error.");
	exit(1);
}

// Llama al scanner para conseguir el siguiente token
void match(token tok){
	if (next_token()!=tok) {
		syntax_error(tok);
	}
	flagToken=0;
}

// <program> SCANEOF
void system_goal(void)
{
	program();
	match(SCANEOF);
}

// begin <statement list> end
void program(void)
{
	match(BEGIN);
	statement_list();
	match(END);
}

// <statement> {<statement>}
void statement_list(void)
{
	statement();
	white (TRUE) {
		switch (next_token()) {
		case ID:
		case READ:
		case WRITE:
			statement ();
			break;
		default:
			return;
		}
	}
}

// Tiene 3 opciones: que sea read, write o declaración de variables
void statement(void)
{
	token tok = next_token();
	switch (tok) {
	case ID:
		/* <statement> ::= ID := <expresion>; */
		match (ID);
		match (ASSIGNOP);
		expression();
		match (SEMICOLON);
		break;

	case READ:
		/* <statement> ::= READ (<id list>) ; */
		match(READ);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);
		break;

	case WRITE:
		/* <statement> ::= WRITE (<id list>) ; */
		match(WRITE);
		match(LPAREN);
		expr_list();
		match(RPAREN);
		match(SEMICOLON);
		break;

	default:
		syntax_error(tok);
		break;
	}
}


//A parsing procedure including semantic processing
void expression (expr_rec *result)
{
	expr_rec left_operand, right_operand;
	op_rec op;

	primary (&left_operand);
	while (next_token() == PLUSOP || next_token() == MINUSOP)
	{
		add_op (&op);
		primary (&right_operand);
		left_operand = gen_infix (left_operand, op, right_operand);
	}
	*result = left_operand;
}

// Puede ser un int, una variable o combinación de ambas que se debe especificar (expression)
void primary (expr_rec * result)
{
	token tok = next_token();
	switch (tok) {
	case LPAREN:
		/* <primary> ::= { <expression> }*/
		match (LPAREN);
		expression(result);
		match (RPAREN);
		break;
	case ID:
		/* <primary> ::= ID */
		id(result);
		break;
	case INTLITERAL:
		/* <primary> ::= INTLITERAL */
		match(INTLITERAL);
		*result = process_literal();
		break;
	default:
		syntax_error(tok);
		break;
	}
}

// Operacion de suma o resta
void add_op(char * result)
{
	token tok = next_token();

	/* <addop> ::= PLUSOP | MINUSOP */
	if (tok == PLUSOP || tok == MINUSOP)
		match(tok);
	strcpy(result, process_op());
	else
	{
		syntax_error(tok);
	}
}

/* <id list> : ID { , ID} */
void id_list(void)
{
	token tok;
	expr_rec reg;
	id(&reg);
	read_id(reg);

	for(t=next_token(); t == COMA; t=next_token()) {
		match(COMA);
		id(&reg);
		read_id(reg);
	}

	// match(ID);
	//
	// while (next_token()==COMMA) {
	// 	match(COMMA);
	// 	match(ID);
	// }
}

/* <expr list> ::= <expression> { , <expression> } */
void expr_list(void)
{
	token tok;
	expr_rec reg;
	expression(&reg);
	write_expr(reg);
	for ( tok = next_token(); t == COMA; t = next_token()) {
		Match(COMA);
		Expresion(&reg);
		Escribir(reg);
	}
	// while (next_token() == COMMA) {
	// 	match(COMMA);
	// 	expression();
	// }
}

// retorna el siguiente token a ser asociado
token next_token(void){
	if (flagToken!=1) {
		current_token = scanner();
		flagToken = 1;
		if(current_token == LEXICALERROR) {
			lexical_error();
		}
		if (current_token == ID) {
			lookup(token_buffer, TS, &current_token);
		}

	}
	return current_token;
}

void id(expr_rec * result){
	Match(ID);
	*result = ProcesarId();
}


/* -----------------------------------------------SCANNER------------------------------------------------------*/

token scanner (FILE *fp)
{
	int in_char, c;
	clear_buffer();//limpia el array
	if (feof(fp))
		return SCANEOF;
	while ((in_char = getc(fp)) != EOF) {
		if (isspace(in_char))
			continue; /* do nothing */
		else if (isalpha(in_char)) {//revisa si el caracter es alfabetico

			buffer_char(in_char);//agrega caracter al buffer

			for (c = getc(fp); isalnum(c) || c == '_'; c = getc(fp)) { //recorre la hilera agregando caracteres al buffer_char, termina cuando no es un caracter ni underscore
				buffer_char(c);
			}
			ungetc(c,fp);

			return check_reserved(); //Revisa si el string encontrado es una palabra reservada

		}  else if (isdigit(in_char)) {
			buffer_char(in_char);
			for (c = getc(fp); isdigit(c); c = getc(fp))
				buffer_char(c);
			ungetc(c,fp);
			return INTLITERAL;
		} else if (in_char == '(')
			return LPAREN;
		else if (in_char == ')')
			return RPAREN;
		else if (in_char == ';')
			return SEMICOLON;
		else if (in_char == ',')
			return COMMA;
		else if (in_char == '+')
			return PLUSOP;
		else if (in_char == ':') {
			/* looking for ":=" */
			c = getc(fp);
			if (c == '=')
				return ASSIGNOP;
			else {
				ungetc(c,fp);
				lexical_error();//
			}
		} else if (in_char == '-') {
			/* is it --, comment start */
			c = getc(fp);
			if (c == '-') {
				do
				{
					in_char = getc(fp);
				} while (in_char != '\n');
			} else {
				ungetc(c,fp);
				return MINUSOP;
			}
		}  else
			lexical_error();
	}

	return 0;
}

/* Agrega su argumento a un buffer de caracteres llamado token_buffer - AgregarCaracter()*/
void buffer_char(int c)
{
	//agregar
	//printf("buffer char character %c \n",c);
	token_buffer[ncol] = c;
	//printf("ncol char character %d \n",ncol);
	ncol++;

}

void clear_buffer(void)
// reset the token string to be empty
{ //token_buffer[0] = '\0';
	memset(token_buffer,0,strlen(token_buffer));
	ncol = 0;
} // end of clearTokStr()


/* check_reserved
 * Toma identifiers que reconoce el scanner y retorna la clase de token (ID o alguna de las palabras reservadas).
 *  Los caracteres en el token_buffer también sirven para la función check_reserved().
 */
token check_reserved(void)
{

	int i=0;
	token tok;
	if (strcmp(token_buffer,"begin")==0)
	{
		tok=BEGIN;
	}
	if (strcmp(token_buffer,"end")==0)
	{
		tok=END;
	}
	if (strcmp(token_buffer,"read")==0)
	{
		tok=READ;
	}
	if (strcmp(token_buffer,"write")==0)
	{
		tok=WRITE;
	}
	//printf("\n");
	//printf(token_buffer);
	return(tok);
}

/*-----------------------------Semantic routines-------------------------------*/


/*Convert literal to a numeric representation and build semantic record.*/
expr_rec process_literal(void)
{
	expr_rec t;

	t.kind = LITERALEXPR;
	strcpy(t.name, token_buffer);
	sscanf(token_buffer, "%d", &t.val);
	return t;
}

/*Declare ID and build a corresponding semantic record.*/
expr_rec process_id(void)
{
	expr_rec t;

	check_id(token_buffer);
	t.kind = IDEXPR;
	strcpy (t.name, token_buffer);
	return t;
}

op_rec process_op(void)
{
	/* Produce operator descriptor. */
	op_rec o;
	if (current_token == PLUSOP)
		o.operator=PLUS;
	else
		o.operator=MINUS;
	return o;
}

//Genera una instrucción de read - Leer()
void read_id(expr_rec in_var)
{
	generate("Read", in_var.name,"Integer","");
}

//Genera una instrucción de write - Escribir()
void write_expr(expr_rec out_expr)
{
	generate("Write", extract(out_expr),"Integer","");
}

//Auxiliar del gen_infix, rutina para almacenar temporales
char *get_temp(void)
{
	/* max temporary allocated so far */
	static int max_temp = 0
	                      static char tempname [MAXIDLEN];

	max_temp++;
	sprintf(tempname, "Temp&%d", max_temp);
	check_id(tempname);
	return tempname;
}

/* Generate code for infix operation.
   Get result temp and set up semantic record for result - GenInfijo()*/
expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2)
{
	static int max_te
	//expr_rec tiene kind,name,value
	expr_rec e_rec;     /* An expr_rec with temp variant set. */
	e_rec.kind = TEMPEXPR;

	//Asigna el nombre a la expresión (Ej. Temp&1)
	strcpy(e_rec.name, get_temp());

	if(e1.kind == IDEXPR) {
		check_id(extractExpr(&e1));
	}
	if(e2.kind == IDEXPR) {
		check_id(extractExpr(&e2));
	}
	generate (extractOp(op), extractExpr(e1),extractExpr(e2), e_rec.name);
	return e_rec;
}

//Escribe una instrucción sobre el archivo de salida (del formato OP A,B,C)
void generate(char * co, char * a, char * b, char * c){
	fprintf(archivoSalida, "%s %s", co, a);

	if (b[0]!='\0')
		fprintf(archivoSalida, "%c%s", ',', b);

	if (c[0]!='\0')
		fprintf(archivoSalida, "%c%s", ',', c);

	fprintf(archivoSalida, "\n");
}

//Esta es tomada del programa en español pero podría no funcionar,
// porque nuestro struct de expr_rec es diferente - Extraer()
// Toma la expresión y retorna una string que corresponde a la información semántica que contiene.
// Esta string podría ser un identifier, op code, literal, etc.
// Esta información se le da a generate() para crear una instrucción completa.
char * extractExpr(expr_rec * reg){
	if(reg->kind == IDEXPR) {
		return reg->name;
	}
	else if(reg->kind == LITERALEXPR) {
		return reg->val;
	}
	else{
		return;
	}
}

char * extractOp(op_rec * op){
	return op->operator;
}

//Busca si existe un símbolo dado en la tabla de símbolos - Buscar()
int lookup(char * id, RegTS * TS, token * t){
	int i = 0;
	while (strcmp("$", TS[i].identificador)) {
		if (!strcmp(id, TS[i].identificador)) {
			*t = TS[i].t;
			return 1;
		}
		i++;
	}
	return 0;
}

//Ingresa un símbolo en la tabla de símbolos - colocar()
void enter(char * id, RegTS * TS){
	int i = 4; //No sé si deberíamos tener las palabras reservadas en la tabla de símboos... Son esas 4
	//Navega al final de la tabla de símbolos
	while (strcmp("$", TS[i].identificador)) {
		i++;
	}

	if (i <= 999) {
		strcpy(TS[i].identificador, id );
		TS[i].t = ID;
		strcpy(TS[++i].identificador, "$" );
	}
}

//Chequea si un token se encuentra en la tabla de símbolos
//Si no se encuentra, lo agrega y luego genera la declaración de la variable - Chequear()
void check_id(char *s)
{
	//Aquí no entiendo para qué sirve tok
	token tok;
	if (!lookup(s,TS,&tok)) {
		enter(s, TS);
		generate("Declare",s,"Integer","");
	}
}

/* Generate code for assignment. */
void assign (expr_rec target, expr_rec source)
{
	generate ("Store", extract (source), target.name,"");
}


void start (void)
{
	/* semantic initialization, none needed. */
}

/* Generate code to finish program */
void finish (void)
{
	generate("Halt","","","");
}
