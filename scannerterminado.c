/*   Este programa implementa un alnalizador lexico para el lenguaje MICRO.
La Entrada es una cadena de caracteres (fichero fuente) y la Salida son tokens.
Suponemos que el buffer de entrada es simplemente un vector de caracteres de longitud fija, que vamos recargando conforme vamos agotando. Leemos una linea cada vez, hasta encontrar el final de fichero (EOF)
EL AFD está implementado mediante switch anidados.
La distinción entre IDs y Palabras reservadas se hace mediante la inicialización de una tabla de palabras reservadas. Los identificadores tienen una longitud inferior a 32 caracteres.
Variables globales: contador de linea y columna, y el buffer de entrada. */

#include <stdio.h>
#include <string.h>
#include <ctype.h> //para funciones isdigit, isalpha ...
#define MAXLENID 32
#define MAXLENBUF 1024
#define MAXRESWORDS 4

typedef enum {
  TKN_BEGIN, TKN_END, TKN_READ, TKN_WRITE, TKN_ID, TKN_NUM, TKN_LPAREN, TKN_RPAREN, TKN_SEMICOLON, TKN_COMMA, TKN_ASSIGN, TKN_ADD, TKN_MINUS, TKN_EOF, TKN_ERROR }
token_types;

typedef enum {
  IN_START, IN_ID, IN_NUM, IN_LPAREN, IN_RPAREN, IN_SEMICOLON, IN_COMMA, IN_ASSIGN, IN_ADD, IN_MINUS, IN_EOF, IN_ERROR, IN_DONE}
States;

typedef struct {
  token_types type;
  char lexema[MAXLENID];
} Token;

Token ReservedWords[MAXRESWORDS]={ {TKN_BEGIN, "begin"}, {TKN_END, "end"}, {TKN_READ, "read"}, {TKN_WRITE, "write"} };

int nline=0;
int ncol=0;
char buffer[MAXLENBUF];

Token LookUpReservedWords(char *);
char GetChar(FILE *);
void UnGetChar(void);
Token GetToken(FILE *);
int isdelim(char c) ; //devuelve 1/0 si c es un blanco, tab, \n 

Token LookUpReservedWords(char *s)
{
  int i=0;
  Token tok;

  for (i=0; i< MAXRESWORDS; i++) {
      if (strcmp(s, ReservedWords[i].lexema)==0) {
	return(ReservedWords[i]); 
      }
    }
  strcpy(tok.lexema,s);
  tok.type=TKN_ID;
  return(tok);
}

char GetChar(FILE *fp) {

  char c;
  static int n; //longitud línea leida, se guarda valor de llamada a llamada

  if (( ncol==0) || (ncol==n) ) {
    if (NULL!=fgets(buffer, MAXLENBUF, fp)) /* lee hasta el salto de linea */
      {
	n=strlen(buffer);
	ncol=0;
	nline++;
      }
    else {
      return(EOF);
    }
  }
  c=buffer[ncol++];
  return (c);
}

void UnGetChar() {
  
  ncol--;  
}

int isdelim(char c) {
  
  char delim[3]={' ', '\t', '\n'};
  int i;

  for (i=0;i<3;i++) {
    if (c==delim[i]) {
      return(1);
    }
  }
  return(0);
}

Token GetToken(FILE *fp) {

  char c;
  States state=IN_START;
  Token token;
  int index=0; //indice al caracter actual del lexema

  while (state!=IN_DONE) {
    switch (state) {
    case IN_START: {
      c=GetChar(fp); 
      while ( isdelim(c)) {
	c=GetChar(fp);
      }
      if (isalpha((int) c)) {
	state=IN_ID;
	token.lexema[index++]=c;
      }
      else if (isdigit((int) c)) {
	state=IN_NUM;
	token.lexema[index++]=c;
      }
      else if (c=='(') {
	token.type=TKN_LPAREN;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
      else if (c==')') {
	token.type=TKN_RPAREN;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
      else if (c==';') {
	token.type=TKN_SEMICOLON;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
      else if (c==',') {
	token.type=TKN_COMMA;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
      else if (c==':') {
	state=IN_ASSIGN;
	token.lexema[index++]=c;
      }
      else if (c=='+') {
	token.type=TKN_ADD;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
      else if (c=='-') {
	token.type=TKN_MINUS;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
      else if (EOF==c) {
	token.type=TKN_EOF;
	state=IN_DONE;
	token.lexema[index++]=c;
      }
     else {
	token.type=TKN_ERROR;
	state=IN_DONE;
      }
    break;
    }
    case IN_NUM: {
	c=GetChar(fp);
	token.lexema[index++]=c;
	if (!isdigit((int) c) ) {
	  token.type=TKN_NUM;
	  state=IN_DONE;
	  UnGetChar();
	  index--;
	}
        break;
    }
    case IN_ID: {
	c=GetChar(fp);
	token.lexema[index++]=c;
	if (! ( (isalnum( (int) c) ) || (c=='_') ) ) {
	  token.type=TKN_ID;
	  state=IN_DONE;
	  UnGetChar();
	  index--;
	  token.lexema[index]='\0';
	  token=LookUpReservedWords(token.lexema);
	}
	break;
    }
    case IN_ASSIGN: {
	c=GetChar(fp);
	token.lexema[index++]=c;
	if (c=='=') {
	  token.type=TKN_ASSIGN;
	  state=IN_DONE;
	  index--;
	}
        break;
    }
  default: {
      token.type=TKN_ERROR;
      state=IN_DONE;
      token.lexema[index++]=c;
    }
  } //end switch
}//end while

   if (token.type==TKN_ERROR) {
     fprintf(stderr, "\nLine %d:%d, Error: Character %c does not match any token\n", nline, ncol, c);
   }
   token.lexema[index]='\0'; //para acabar la cadena
   return(token);
}

int main(int argc, char **argv){

  FILE *fp;
  Token token;
  
  if (argc==2) {
    if ( NULL== (fp= fopen(argv[1],"r")) ) {
      fprintf(stderr, "File %s cannot be open\n", argv[1]);
      return(0);
    }
    else {
      token=GetToken(fp);
      while (TKN_EOF != token.type) {
	fprintf(stderr,"(%d,%s) ", token.type, token.lexema);
	token=GetToken(fp);
      }
      printf("\n%d lineas analizadas", nline);
      return(0);
    }
  }
  else {
    fprintf(stderr, "Usage: program_name file_name\n");
    return(0);
  }
}