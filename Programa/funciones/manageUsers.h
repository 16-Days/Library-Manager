#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"


 
// Declaración de la función validarUsuarioExistente
int validarUsuarioExistente(int id);

void crearArchivo(const char *nombre, int id, const char *direccion) {
    FILE *archivo;
    archivo = fopen("../data/user_Register.json", "a+");  // Cambia el nombre del archivo según tus necesidades

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

    if (validarUsuarioExistente(id)) {
        printf("Ya existe un usuario con la misma ID.\n");
        return 0; // Termina la función sin crear el usuario
    }

    printf("Ingrese la dirección: ");
    scanf("%s", direccion);

    
    crearArchivo(nombre, id, direccion);

    return 0;
}

//Validación que no se repitan
// Función para validar si ya existe un usuario con la misma ID en el archivo JSON
int validarUsuarioExistente(int id) {
    FILE *archivo;
    archivo = fopen("../data/user_Register.json", "r"); // Abre el archivo en modo lectura

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 0; // Indica que no se pudo validar
    }

    char buffer[1024];
    size_t size;

    fseek(archivo, 0, SEEK_END);
    size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);

    fread(buffer, size, 1, archivo);
    fclose(archivo);

    cJSON *json = cJSON_Parse(buffer); // Parsea el contenido del archivo como JSON

    if (json == NULL) {
        printf("Error al parsear el archivo JSON.\n");
        return 0; // Indica que no se pudo validar
    }

    cJSON *usuario;
    cJSON_ArrayForEach(usuario, json) {
        cJSON *idJson = cJSON_GetObjectItemCaseSensitive(usuario, "id");
        if (cJSON_IsNumber(idJson) && idJson->valueint == id) {
            cJSON_Delete(json);
            return 1; // Indica que el usuario existe
        }
    }

    cJSON_Delete(json);
    return 0; // Indica que el usuario no existe
}


