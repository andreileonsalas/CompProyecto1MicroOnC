#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS
#define MAXRESWORDS 4



typedef enum token_types {
	BEGIN, END, READ, WRITE, ID, INTLITERAL,
	LPAREN, RPAREN, SEMICOLON, COMMA, ASSIGNOP,
	PLUSOP, MINUSOP, SCANEOF
} token;

token ReservedWords[MAXRESWORDS]={ {BEGIN}, {END}, {READ}, {WRITE} };
char token_buffer[1024];
int token_len;
int ncol = 0; 

extern token scanner(FILE *); 
extern char token_buffer[];


void lexical_error(int ch)
{ printf("Lexical error at \"%c\" ", ch);  
  exit(1);
}

void clear_buffer(void)
// reset the token string to be empty
{ //token_buffer[0] = '\0';
memset(token_buffer,0,strlen(token_buffer));
ncol = 0;
} // end of clearTokStr()



/* buffer_char
* agrega su argumento a un buffer de caracteres llamado token_buffer
*/
void buffer_char(int c)
{
   //agregar 
   //printf("buffer char character %c \n",c);
   token_buffer[ncol] = c;
   //printf("ncol char character %d \n",ncol);
   ncol++;

}

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
     printf("La palabra reservada es begin \n");
  }
    if (strcmp(token_buffer,"end")==0)
  {
     tok=END;
     printf("La palabra reservada es end \n");
  }
    if (strcmp(token_buffer,"read")==0)
  {
     tok=READ;
     printf("La palabra reservada es read \n");
  }
    if (strcmp(token_buffer,"write")==0)
  {
     tok=WRITE;
     printf("La palabra reservada es write \n");
  }
   
//   for (i=0; i< MAXRESWORDS; i++) {
//       if (strcmp(s, ReservedWords[i].lexema)==0) {
// 	return(ReservedWords[i]); 
//       }
//     }
  printf("\n");
  printf(token_buffer);
  
  return(tok);
}



token scanner (FILE *fp)
{
   int in_char, c;
   clear_buffer();//limpia el array
   if (feof(fp))
      return SCANEOF;
   while ((in_char = getc(fp)) != EOF){
      //printf("%i\n",in_char);
      if (isspace(in_char))
         continue; /* do nothing */
      else if (isalpha(in_char)) {//revisa si el caracter es alfabetico
         
         buffer_char(in_char);//agrega caracter al buffer
         
         for (c = getc(fp); isalnum(c) || c == '_'; //revisa si es alfanumerico o si es un underscore
            c = getc(fp))
            
            buffer_char(c);
            ungetc(c,fp);
            return check_reserved();//
      }  else if (isdigit(in_char)){
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
      else if (in_char == ':'){
         /* looking for ":=" */
         c = getc(fp);
         if (c == '=')
            return ASSIGNOP;
         else {
            ungetc(c,fp);
            lexical_error(in_char);//
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
         lexical_error(in_char);
   }
   
   return 0;
}






/*
int main(int argc, char **argv){
  printf("probando");
  FILE *fp;
  token token;
  fp = fopen("example.txt","r");
  printf("primer token \n");
  token = scanner(fp);
  printf("liego de primer token \n");
  printf("andrei %d ", token);
  
  while (EOF != token) {
	printf("%d ", token);
   token=scanner(fp);
  }
  //printf(token);
   //printf(stderr, "Usage: program_name file_name\n");
   return(0);

}*/

int main(int argc, char **argv){

  FILE *fp;
  token token;
  
  if (argc==2) {
    if ( NULL== (fp= fopen(argv[1],"r")) ) {
      fprintf(stderr, "File %s cannot be open\n", argv[1]);
      return(0);
    }
    else {
      token=scanner(fp);
      while (EOF != token) {
	      token=scanner(fp);
      }
      //printf(token_buffer);
      // int result = strcmp(token_buffer, "Begin");
      // printf("strcmp(str1, str2) = %d\n", result);
      return(0);
    }
  }
  else {
    fprintf(stderr, "Usage: program_name file_name\n");
    return(0);
  }
}