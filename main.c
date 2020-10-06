#include "Funciones.h"

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
