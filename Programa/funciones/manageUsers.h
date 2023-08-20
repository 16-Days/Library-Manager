#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void crearArchivo(const char *nombre, int id, const char *direccion) {
    FILE *archivo;
    archivo = fopen("data/user_Register.json", "a+");  // Cambia el nombre del archivo según tus necesidades

    if (archivo == NULL) {
        printf("No se pudo crear el archivo.\n");
        return;
    }

    fprintf(archivo, "{\n");
    fprintf(archivo, "  \"Nombre\": \"%s\",\n", nombre);
    fprintf(archivo, "  \"id\": %d,\n", id);
    fprintf(archivo, "  \"Correo\": \"%s\"\n", direccion);
    fprintf(archivo, "}\n");

    fclose(archivo);
    printf("Archivo JSON creado con éxito.\n");
}

int register_User() {
    char nombre[100];
    int id;
    char direccion[100];

    printf("Ingrese el nombre: ");
    scanf("%s", nombre);

    printf("Ingrese el numero de identificación: ");
    scanf("%d", &id);

    printf("Ingrese la dirección: ");
    scanf("%s", direccion);

    
    crearArchivo(nombre, id, direccion);

    return 0;
}

