#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "cJSON.h"

struct Usuario {
    char nombre[100];
    char cedula[20];
    char correo[100];
};

// Función para verificar si una cadena contiene solo dígitos y tiene exactamente 9 caracteres
int validarCedula(const char *cedula) {
    int len = strlen(cedula);
    if (len != 9) {
        return 0; // La cédula debe tener exactamente 9 dígitos
    }
    for (int i = 0; i < len; i++) {
        if (!isdigit(cedula[i])) {
            return 0; // La cédula debe contener solo dígitos
        }
    }
    return 1; // La cédula es válida
}

// Función para ingresar los datos del usuario
void ingresarUsuario(struct Usuario *usuario) {
    printf("Ingrese el nombre: ");
    scanf("%s", usuario->nombre);

    char cedula[20];
    do {
        printf("Ingrese la cédula (9 dígitos): ");
        scanf("%s", cedula);
    } while (!validarCedula(cedula));

    strcpy(usuario->cedula, cedula);

    printf("Ingrese el correo: ");
    scanf("%s", usuario->correo);
}

// Función para cargar la lista de usuarios desde el archivo JSON
cJSON *cargarUsuariosDesdeArchivo(const char *rutaArchivo) {
    FILE *fp = fopen(rutaArchivo, "r");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *file_content = (char *)malloc(file_size + 1);
        fread(file_content, 1, file_size, fp);
        fclose(fp);

        cJSON *root = cJSON_Parse(file_content);
        free(file_content);

        return root;
    } else {
        return NULL;
    }
}

// Función para guardar la lista de usuarios en el archivo JSON
void guardarUsuariosEnArchivo(const char *rutaArchivo, cJSON *root) {
    FILE *fp = fopen(rutaArchivo, "w");
    if (fp != NULL) {
        char *json = cJSON_Print(root);
        fputs(json, fp);
        fclose(fp);
        free(json);
    }
}

int mainss() {
    struct Usuario nuevoUsuario;
    cJSON *root = NULL;

    printf("Por favor, ingrese los datos del nuevo usuario:\n");
    ingresarUsuario(&nuevoUsuario);

    // Especificar la ruta completa al archivo JSON
    const char *rutaArchivo = "../data/usuarios.json";

    // Cargar la lista de usuarios existentes desde el archivo JSON
    root = cargarUsuariosDesdeArchivo(rutaArchivo);

    if (root == NULL) {
        // Si el archivo JSON no existe o está vacío, crea un nuevo objeto JSON
        root = cJSON_CreateArray();
    }

    // Crear un objeto JSON para el nuevo usuario y agregarlo a la lista
    cJSON *usuario = cJSON_CreateObject();
    cJSON_AddStringToObject(usuario, "nombre", nuevoUsuario.nombre);
    cJSON_AddStringToObject(usuario, "cedula", nuevoUsuario.cedula);
    cJSON_AddStringToObject(usuario, "correo", nuevoUsuario.correo);
    cJSON_AddItemToArray(root, usuario);

    // Guardar la lista actualizada de usuarios en el archivo JSON
    guardarUsuariosEnArchivo(rutaArchivo, root);
    cJSON_Delete(root); // Liberar la memoria

    printf("\nDatos del usuario registrados y guardados en %s\n", rutaArchivo);

    return 0;
}
