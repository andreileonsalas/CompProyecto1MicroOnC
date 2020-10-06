#include "Funciones.h"

int validacion (char* archivo,char c){

	int i=0;
	while(archivo[i]!='.') {
		i=i+1;
	}
	if ((archivo[i+1] == c)&&(archivo[i+2] =='\0')) {
		return 1;
	}else{
		return 0;
	}
}

int main(int argc, char* argv[]) {

	if (argc!=2 && argc!=3) {
		fprintf(stderr,"Debe ingresar el archivo fuente y opcionalmente el nombre que desea darle al archivo de salida.\n");
		return -1;
	}
	if (!validacion(argv[1],'m')) {
		printf("La extension del archivo fuente debe ser .m\n");
		return -2;
	}
	archivo = fopen(argv[1], "rb");
	if (archivo == NULL) {
		printf("El archivo fuente no existe\n");
		return -3;
	}

	if (argc==3) {
		archivoSalida = fopen(argv[2], "wb+");
	}else{
		archivoSalida = stdout;
	}

	system_goal();

	fclose(archivo);
	if (argc==3) {
		fclose(archivoSalida);
	}

	return EXIT_SUCCESS;
}
