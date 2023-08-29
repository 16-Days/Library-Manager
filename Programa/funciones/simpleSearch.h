
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cJSON.h"
#include <string.h>


// Estructura para almacenar datos de libros
struct Book {
    char titulo[100];
    char autor[100];
    int anio;
    char genero[50];
    char descripcion[200];
    double calificacion;
    bool estado;
};

// Función para cargar los datos de los libros desde un archivo JSON
static inline cJSON *loadBooks(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("No se pudo abrir el archivo JSON.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *jsonStr = (char *)malloc(fileSize + 1);
    if (!jsonStr) {
        printf("Error al asignar memoria.\n");
        fclose(file);
        return NULL;
    }

    fread(jsonStr, 1, fileSize, file);
    fclose(file);

    jsonStr[fileSize] = '\0';

    cJSON *root = cJSON_Parse(jsonStr);
    if (!root) {
        printf("Error al analizar JSON.\n");
        free(jsonStr);
        return NULL;
    }

    free(jsonStr);
    return root;
}

// Función para realizar la búsqueda simple en los libros
static inline void simpleSearch(const char *searchText, const char *filePath) {
    cJSON *root = loadBooks(filePath);
    if (!root) {
        return;
    }

    cJSON *booksArray = cJSON_GetArrayItem(root, 0);
    if (!booksArray || !cJSON_IsArray(booksArray)) {
        printf("No se encontró el array de libros.\n");
        cJSON_Delete(root);
        return;
    }

    cJSON *book;
    int booksFound = 0;

    cJSON_ArrayForEach(book, booksArray) {
        cJSON *titulo = cJSON_GetObjectItem(book, "titulo");
        if (titulo && cJSON_IsString(titulo) && strstr(titulo->valuestring, searchText) != NULL) {
            cJSON *autor = cJSON_GetObjectItem(book, "autor");
            cJSON *anio = cJSON_GetObjectItem(book, "anio");
            cJSON *genero = cJSON_GetObjectItem(book, "genero");
            cJSON *descripcion = cJSON_GetObjectItem(book, "descripcion");
            cJSON *cantidad = cJSON_GetObjectItem(book, "cantidad");
            cJSON *estado = cJSON_GetObjectItem(book, "estado");

            printf("Titulo: %s\n", titulo->valuestring);
            printf("Autor: %s\n", autor->valuestring);
            printf("Año: %d\n", anio->valueint);
            printf("Genero: %s\n", genero->valuestring);
            printf("Descripción: %s\n", descripcion->valuestring);
            printf("cantidad: %.2f\n", cantidad->valuedouble);
            printf("Estado: %s\n", estado->valueint ? "Disponible" : "No disponible");
            printf("\n");

            booksFound++;
        }
    }

    if (booksFound == 0) {
        printf("No se encontraron libros que coincidan con la busqueda.\n");
    }

    cJSON_Delete(root);
}


