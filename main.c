
#include "Funciones.h"

/////////////////gcc2 fuente salida

int main(int argc, char* argv[]) {

	if (argc!=2 && argc!=3) {
		fprintf(stderr,"Debe ingresar el archivo fuente y opcionalmente el nombre que desea darle al archivo de salida.\n");
		return -1;
	}
	if (!validation(argv[1],'m')) {
		printf("La extension del archivo fuente debe ser .m\n");
		return -2;
	}
	archive = fopen(argv[1], "rb");
	if (archive == NULL) {
		printf("El archivo fuente no existe\n");
		return -3;
	}

	if (argc==3) {
		outputArchive = fopen(argv[2], "wb+");
		sectionData = fopen("salidadata.asm", "wb+");
	}else{
		outputArchive = stdout;
		outputArchive = fopen("salida.asm", "wb+");
		sectionData = fopen("salidadata.asm", "wb+");
	}
	
	system_goal();

	fclose(archive);
	if (argc==2) {
		fclose(outputArchive);
	}
	fclose(sectionData);


	FILE *fp1 = fopen("salidadata.asm", "r"); 
   	FILE *fp2 = fopen("salida.asm", "r"); 
	FILE *fp3 = fopen("microasm.asm", "w"); 
	char c; 
	
	if (fp1 == NULL || fp2 == NULL || fp3 == NULL) 
	{ 
			puts("Could not open files"); 
			exit(0); 
	} 
	
	//Copy contents of first file to file3.txt 
	while ((c = fgetc(fp1)) != EOF) 
		fputc(c, fp3);
	
	// Copy contents of second file to file3.txt 
	while ((c = fgetc(fp2)) != EOF) 
		fputc(c, fp3); 
	
	printf("Generado asm final"); 
	
	fclose(fp1); 
	fclose(fp2); 
	fclose(fp3);

	// printf("Compilando el codigo\n");
	// system("nasm -f elf64 microasm.asm");
	// system(" gcc salida.o -o salida");
	// printf("Ejecutando el codigo\n");
	// system("./a.out");
	

	return EXIT_SUCCESS;
}
