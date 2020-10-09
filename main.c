
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
	}else{
		outputArchive = stdout;
		outputArchive = fopen("salida.asm", "wb+");
	}

	system_goal();

	fclose(archive);
	if (argc==2) {
		fclose(outputArchive);
	}

	printf("Compilando el codigo\n");
	system("nasm -f elf64 salida.asm");
	system("ld salida.o");
	printf("Ejecutando el codigo\n");
	system("./a.out");
	

	return EXIT_SUCCESS;
}
