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

int validarCedula(const char *cedula) {
    int len = strlen(cedula);
    if (len != 9) {
        return 0;
    }
    for (int i = 0; i < len; i++) {
        if (!isdigit(cedula[i])) {
            return 0;
        }
    }
    return 1;
}

void ingresarUsuario(struct Usuario *usuario) {
    getchar();
    printf("Ingrese el nombre: ");
    fgets(usuario->nombre, sizeof(usuario->nombre), stdin);
    usuario->nombre[strcspn(usuario->nombre, "\n")] = '\0';

    char cedula[20];
    do {
        printf("Ingrese la cédula (9 dígitos): ");
        scanf("%s", cedula);
    } while (!validarCedula(cedula));

    strcpy(usuario->cedula, cedula);

    printf("Ingrese el correo: ");
    scanf("%s", usuario->correo);
}

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

void guardarUsuariosEnArchivo(const char *rutaArchivo, cJSON *root) {
    FILE *fp = fopen(rutaArchivo, "w");
    if (fp != NULL) {
        char *json = cJSON_Print(root);
        fputs(json, fp);
        fclose(fp);
        free(json);
    }
}

int cedulaExiste(cJSON *root, const char *cedula) {
    if (root == NULL) {
        return 0; // Si no hay usuarios en el JSON, la cédula no existe.
    }

    cJSON *usuario;
    cJSON_ArrayForEach(usuario, root) {
        cJSON *cedulaJSON = cJSON_GetObjectItemCaseSensitive(usuario, "cedula");
        if (cJSON_IsString(cedulaJSON) && (strcmp(cedula, cedulaJSON->valuestring) == 0)) {
            return 1; // La cédula ya existe en el JSON.
        }
    }

    return 0; // La cédula no existe en el JSON.
}

int mainss() {
    struct Usuario nuevoUsuario;
    cJSON *root = NULL;

    printf("Por favor, ingrese los datos del nuevo usuario:\n");
    ingresarUsuario(&nuevoUsuario);

    const char *rutaArchivo = "../data/usuarios.json";
    root = cargarUsuariosDesdeArchivo(rutaArchivo);

    if (root == NULL) {
        root = cJSON_CreateArray();
    }

    if (cedulaExiste(root, nuevoUsuario.cedula)) {
        printf("La cédula ingresada ya existe en el archivo JSON. No se registró el usuario.\n");
    } else {
        cJSON *usuario = cJSON_CreateObject();
        cJSON_AddStringToObject(usuario, "nombre", nuevoUsuario.nombre);
        cJSON_AddStringToObject(usuario, "cedula", nuevoUsuario.cedula);
        cJSON_AddStringToObject(usuario, "correo", nuevoUsuario.correo);
        cJSON_AddItemToArray(root, usuario);
        guardarUsuariosEnArchivo(rutaArchivo, root);
        cJSON_Delete(root);
        printf("\nDatos del usuario registrados y guardados en %s\n", rutaArchivo);
    }

    return 0;
}
