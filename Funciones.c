
#include "Funciones.h"

RegTS TS[1000] = { {"$", 99} };
int numeroVariableTemporal=1;

/*----------------------------------------------------Validacion-----------------------------------*/


int validation (char* archive,char c){

	int i=0;
	while(archive[i]!='.') {
		i=i+1;
	}
	if ((archive[i+1] == c)&&(archive[i+2] =='\0')) {
		return 1;
	}else{
		return 0;
	}
}




/* ------------------------------------------PARSER---------------------------------------------------------------*/

// Llama al scanner para conseguir el siguiente token
void match(token tok){
	//printf("pre MATCH con: %d\n", tok);
	token temp = next_token();
	//printf("pre MATCH temp: %d\n", temp);
	if (temp!=tok) {
		//printf("token_buffer tiene %s \n",token_buffer);
		//printf("Error de sintaxis MATCH con: %d\n", tok);
		syntax_error();
	}
	flagToken=0;
}

// <program> SCANEOF
void system_goal(void){
	program();
	//printf("entrando a revisar match SCANEOF\n");
	match(SCANEOF);
}

// begin <statement list> end
void program(void){
	match(BEGIN);
	statement_list();
	match(END);
	finish();
}

// <statement> {<statement>}
void statement_list(void){
	statement();
	while(1) {
		switch(next_token()) {
		case ID: case READ: case WRITE:
			statement();
			break;
		default:
			return;
		}
	}
}

void statement(void){
	token tok = next_token();
	expr_rec left, right;
	switch (tok) {
	/* <statement> ::= ID := <expresion>; */
	case ID:
		id(&left);
		match(ASSIGNOP);
		expression(&right);
		assign(left, right);
		match(SEMICOLON);
		break;
	/* <statement> ::= READ (<id list>) ; */
	case READ:
		match(READ);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);
		break;
	/* <statement> ::= WRITE (<id list>) ; */
	case WRITE:
		match(WRITE);
		match(LPAREN);
		expr_list();
		match(RPAREN);
		match(SEMICOLON);
		break;
	default:
		//printf("syntax error en statement \n");
		syntax_error();
		break;
	}
}

//A parsing procedure including semantic processing
void expression(expr_rec * result){
	expr_rec left_operand, right_operand;
	token tok;
	char op[MAXIDLEN];
	primary(&left_operand);
	for(tok = next_token(); tok == PLUSOP || tok == MINUSOP; tok = next_token()) {
		add_op(op);
		primary(&right_operand);
		left_operand = gen_infix(left_operand, op, right_operand);
	}
	*result = left_operand;
}

// Int, variable o or combination of both that must be specified (expression)
void primary(expr_rec * result){
	switch(next_token()) {
	/* <primary> ::= ID */
	case ID:
		id(result);
		break;
	/* <primary> ::= INTLITERAL */
	case INTLITERAL:
		match(INTLITERAL);
		*result = process_literal();
		break;
	/* <primary> ::= { <expression> }*/
	case LPAREN:
		match(LPAREN);
		expression(result);
		match(RPAREN);
		break;
	}
}

/* <addop> ::= PLUSOP | MINUSOP */
void add_op(char * result){
	token tok = next_token();
	//printf("el operador asignado es: %d\n", tok);
	if (tok == PLUSOP || tok == MINUSOP) {
		//printf("el operador asignado es: %d\n", tok);
		match(tok);
		//printf("el operador asignado es: %d\n", tok);
		strcpy(result, process_op());
		//printf("el operador asignado es: %d\n", tok);
		//printf(result);
	} else
		syntax_error();
}

void syntax_error(){
	printf("ERROR SINTACTICO");
}

void lexical_error(){
	printf("ERROR LEXICO");
}


/* <id list> : ID { , ID} */
void id_list(void){
	token tok;
	expr_rec reg;
	id(&reg);
	read_id(reg);
	for ( tok = next_token(); tok == COMMA; tok = next_token()) {
		match(COMMA);
		id(&reg);
		read_id(reg);
	}
}

/* <expr list> ::= <expression> { , <expression> } */
void expr_list(void){
	token tok;
	expr_rec reg;
	expression(&reg);
	write_expr(reg);
	for ( tok = next_token(); tok == COMMA; tok = next_token()) {
		match(COMMA);
		expression(&reg);
		write_expr(reg);
	}
}

token next_token(void){
	//printf("%i\n",flagToken);
	if (flagToken!=1) {
		current_token = scanner();
		//printf("next token es: %d\n", current_token);
		flagToken = 1;
		if(current_token == LEXICALERROR) {
			lexical_error();
		}
		if (current_token == ID) {
			lookup(token_buffer, TS, &current_token);
		}

	}
	//printf("next token al final devuelve: %d\n", current_token);
	//printf("y token buffer tiene %s\n", token_buffer);
	return current_token;
}

void id(expr_rec * result){
	match(ID);
	*result = process_id();
}

/* -----------------------------------------------scanner------------------------------------------------------*/

/* check_reserved
 * Toma identifiers que reconoce el scanner y retorna la clase de token (ID o alguna de las palabras reservadas).
 *  Los caracteres en el token_buffer también sirven para la función check_reserved().
 */
token check_reserved(void)
{
	//token_buffer[ncol] = '\0';
	////printf( "Token_Buffer = %s\n", &token_buffer[0] );


	int i=0;
	token tok;

	if (strcmp(token_buffer,"begin")==0)
	{
		tok=BEGIN;
		return(tok);
	}
	else if (strcmp(token_buffer,"end")==0)
	{
		tok=END;
		return(tok);
	}
	else if (strcmp(token_buffer,"read")==0)
	{
		tok=READ;
		return(tok);
	}
	else if (strcmp(token_buffer,"write")==0)
	{
		tok=WRITE;
		return(tok);
	}
	else if (strcmp(token_buffer,"SCANEOF")==0)
	{
		tok=SCANEOF;
		return(tok);
	}
	else {
		tok=ID;
		return(tok);
	}
}

token scanner ()
{

	int in_char, c;
	int i=0;
	clear_buffer();//limpia el array
	if (feof(archive))
		return SCANEOF;


	while ((in_char = getc(archive)) != EOF) {
		//printf("caracter actual es:%c\n",in_char);

		if (isspace(in_char))
			continue; /* do nothing */
		else if (isalpha(in_char)) {//revisa si el caracter es alfabetico

			buffer_char(in_char, i);//agrega caracter al buffer
			i++;

			for (c = getc(archive); isalnum(c) || c == '_'; c = getc(archive)) { //recorre la hilera agregando caracteres al buffer_char, termina cuando no es un caracter ni underscore
				buffer_char(c, i);
				i++;
			}
			ungetc(c,archive);

			return check_reserved(); //Revisa si el string encontrado es una palabra reservada

		}  else if (isdigit(in_char)) {
			buffer_char(in_char, i);
			i++;
			for (c = getc(archive); isdigit(c); c = getc(archive)){
				buffer_char(c, i);
				i++;
			}
			ungetc(c,archive);

			//token_buffer[ncol] = '\0';
			////printf( "Token_Buffer = %s\n", &token_buffer[0]);
			return INTLITERAL;

		} else if (in_char == '(')
			return LPAREN;
		else if (in_char == ')')
			return RPAREN;
		else if (in_char == ';')
			return SEMICOLON;
		else if (in_char == ',')
			return COMMA;
		else if (in_char == '+') {
			//printf("caracter actual in_char es:%c\n",in_char);
			//printf("caracter actual c es:%c\n",c);
			buffer_char(in_char, i);
			i++;
			return PLUSOP;
		}
		else if (in_char == ':') {
			/* looking for ":=" */
			c = getc(archive);
			if (c == '=') {
				return ASSIGNOP;
			}
			else {
				ungetc(c,archive);
				lexical_error();//
			}
		} else if (in_char == '-') {
			//printf("entro al comentario/menos \n");
			/* is it --, comment start */
			c = getc(archive);
			if (c == '-') {
				//printf("entro al comentario \n");
				do
				{
					in_char = getc(archive);
				} while (in_char != '\n');
			} else {
				//printf("caracter actual in_char es:%c\n",in_char);
				//printf("caracter actual c es:%c\n",c);
				buffer_char(in_char, i);
				i++;
				ungetc(c,archive);
				return MINUSOP;
			}
		}  else
			lexical_error();
	}

	return 0;
}



// int esEstadoFinal(int e){
// 	switch (e) {
// 	case 0: case 1: case 3: case 11: case 14:
// 		return 0;
// 	}
// 	return 1;
// }



// token scanner(void){
// 	char c;
// 	int col, i = 0, estado_actual = 0;

// 	static int tabla [15][13]={
// 		{1,3,5,6,7,8,9,10,11,14,13,0,14},
// 		{1,1,2,2,2,2,2,2,2,2,2,2,2},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{4,3,4,4,4,4,4,4,4,4,4,4,4},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,12,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,12,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14},
// 		{14,14,14,14,14,14,14,14,14,14,14,14,14}
// 	};

// 	clear_buffer();

// 	do {
// 		c = getc(archive);
// 		col = columna(c);
// 		estado_actual = tabla[estado_actual][col];
// 		if (col!= 11) {
// 			buffer_char(c, i);
// 			i++;
// 		}
// 	} while (!esEstadoFinal(estado_actual));

// 	switch(estado_actual) {
// 	case 2:
// 		if (col != 11) {
// 			ungetc(c, archive);
// 			token_buffer[i-1] = '\0';
// 		}
// 		return ID;
// 	case 4:
// 		if (col != 11) {
// 			ungetc(c, archive);
// 			token_buffer[i-1] = '\0';
// 		}
// 		return INTLITERAL;
// 	case 5:
// 		return PLUSOP;
// 	case 6:
// 		return MINUSOP;
// 	case 7:
// 		return LPAREN;
// 	case 8:
// 		return RPAREN;
// 	case 9:
// 		return COMMA;
// 	case 10:
// 		return SEMICOLON;
// 	case 12:
// 		return ASSIGNOP;
// 	case 13:
// 		return SCANEOF;
// 	case 14:
// 		return LEXICALERROR;

// 	}
// 	return SCANEOF;
// }

// int columna(int c){
// 	if(isalpha(c)) {
// 		return 0;
// 	}else if (isdigit(c)) {
// 		return 1;
// 	}else if (isspace(c)) {
// 		return 11;
// 	}
// 	switch (c) {
// 	case '+':
// 		return 2;
// 	case '-':
// 		return 3;
// 	case '(':
// 		return 4;
// 	case ')':
// 		return 5;
// 	case ',':
// 		return 6;
// 	case ';':
// 		return 7;
// 	case ':':
// 		return 8;
// 	case '=':
// 		return 9;
// 	case EOF:
// 		return 10;
// 	default:
// 		return 12;

// 	}
// }

//Esta función tiene algo malo, no me pregunten qué, pero funciona mejor la de la progra en esp.
// /* Agrega su argumento a un buffer de caracteres llamado token_buffer - AgregarCaracter()*/
// void buffer_char(int c)
// {
// 	//agregar
// 	//printf("buffer char character %c \n",c);
// 	token_buffer[ncol] = c;
// 	////printf("ncol char character %d \n",ncol);
// 	ncol++;
//
// }


/* Adds argument to token_buffer*/
void buffer_char(int c, int posicion){
	token_buffer[posicion] = c;
	//printf("\nSe imprime %c\n",c);
}

// Reset the token string to be empty
void clear_buffer(void){
	//printf("Se limpio el buffer\n");
	memset(token_buffer,0,strlen(token_buffer));
	int i;
	for (i=0; i < strlen(token_buffer); i++){
		token_buffer[i]='\0';
	}
	
}

/*-----------------------------Semantic routines-------------------------------*/

/*Convert literal to a numeric representation and build semantic record.*/
expr_rec process_literal(void){
	expr_rec reg;
	reg.kind = INTLITERAL;
	strcpy(reg.name, token_buffer);
	sscanf(token_buffer, "%d", &reg.val);
	return reg;
}

/*Declare ID and build a corresponding semantic record.*/
expr_rec process_id(void){
	expr_rec reg;
	check_id(token_buffer);
	reg.kind = ID;
	strcpy(reg.name, token_buffer);
	return reg;
}

/* Produce operator descriptor. (Diff on book)*/
char * process_op(void){
	//printf("El operador es %s \n",token_buffer);
	return token_buffer;
}

/* Produce read instruction*/
void read_id(expr_rec in_var){
	generate("Read", in_var.name, "Integer", "");
}

/* Produce write instruction*/
void write_expr(expr_rec out_expr){
	generate("Write", extractExpr(&out_expr), "Integer", "");
}

//Produce temporals, gen_infix aux routine
char *get_temp(void)
{
	/* max temporary allocated so far */
	static int max_temp = 0;
	static char tempname [MAXIDLEN];

	max_temp++;
	sprintf(tempname, "Temp%d", max_temp);
	check_id(tempname);
	return tempname;
}

/* Generate code for infix operation. Get result temp and set up semantic record for result*/
//Esta no calza con libro tampoco
// expr_rec gen_infix(expr_rec e1, char * op, expr_rec e2){
// 	expr_rec reg;
// 	char opString[MAXIDLEN];
// 	reg.kind = TEMPEXPR;
//
// 	//Assigns name to expression (Ej. Temp&1)
// 	strcpy(reg.name, get_temp());
//
// 	if (op[0] == '-')
// 		strcpy(opString, "Add");
// 	if (op[0] == '+')
// 		strcpy(opString, "Sub");
//
// 	if (e1.kind == IDEXPR)
// 		check_id(extractExpr(&e1));
// 	if (e2.kind == IDEXPR)
// 		check_id(extractExpr(&e2));
// 	check_id(reg.name);
//
// 	generate(opString, extractExpr(&e1), extractExpr(&e2), reg.name);
//
// 	return reg;
// }

expr_rec gen_infix(expr_rec e1, char * op, expr_rec e2){
	expr_rec reg;
	char cadenaTemporal[MAXIDLEN] = "Temp";
	char numero[MAXIDLEN];
	char cadenaOperador[MAXIDLEN];
	//printf("Genera codigo aaaaa%c\n",op[0]);
	//printf("Genera codigo aaaaa%s\n",op);
	//printf("el buffer tiene: %s\n",token_buffer);
	if (op[0] == '-')
		{//printf("Genera codigo para sub \n");
		strcpy(cadenaOperador, "Sub");}
	if (op[0] == '+')
		{//printf("Genera codigo para add \n");
		strcpy(cadenaOperador, "Add");}
	sprintf(numero, "%d", numeroVariableTemporal);
	numeroVariableTemporal++;
	strcat(cadenaTemporal, numero);
	if (e1.kind == ID)
		check_id(extractExpr(&e1));
	if (e2.kind == ID)
		check_id(extractExpr(&e2));
	check_id(cadenaTemporal);
	generate(cadenaOperador, extractExpr(&e1), extractExpr(&e2), cadenaTemporal);
	strcpy(reg.name, cadenaTemporal);
	return reg;
}

/* Write instruction to archive */
void generate(char * co, char * a, char * b, char * c){
	fprintf(outputArchive, "%s %s", co, a);

	if (b[0]!='\0')
		fprintf(outputArchive, "%c%s", ',', b);

	if (c[0]!='\0')
		fprintf(outputArchive, "%c%s", ',', c);

	fprintf(outputArchive, "\n");
}

/*Takes expression and returns semantic info it contains*/
// char * extractExpr(expr_rec * reg){
// 	if(reg->kind == IDEXPR) {
// 		return reg->name;
// 	} else {
// 		//printf("Something went wrong at extractExpr");
// 	}
// }

char * extractExpr(expr_rec * reg){
	return reg->name;
}

/*Checks if a given symbol is in the TS*/
int lookup(char * id, RegTS * TS, token * t){
	int i = 0;
	while (strcmp("$", TS[i].identifier)) {
		// CHEQUEAR ESTE LOOP QUE NO ENTIENDO
		if (!strcmp(id, TS[i].identifier)) {
			*t = TS[i].tok;
			return 1;
		}
		i++;
	}
	return 0;
}

/*Enters a symbol at the last spot of the symbol table (before $)*/
void enter(char * id, RegTS * TS){
	int i = 0;
	while (strcmp("$", TS[i].identifier)) i++;
	if (i <= 999) {
		strcpy(TS[i].identifier, id );
		TS[i].tok = ID;
		strcpy(TS[++i].identifier, "$" );
	}
}

/*Checks if a token is in the symbol table*/
void check_id(char * s){
	token tok;
	if (!lookup(s, TS, &tok)) {
		enter(s, TS);
		generate("Declare", s, "Integer", "");
	}
}

/* Generate code for assignment. */
void assign(expr_rec target, expr_rec source){
	generate("Store", extractExpr(&source), target.name, "");
}

/* semantic initialization, none needed. */
void start(void){

}

/* Generate code to finish program */
void finish(void){
	generate("Halt", "", "", "");
}
