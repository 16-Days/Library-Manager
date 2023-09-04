#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h" 
#include <stdbool.h>
                                          
struct Libro {
    int id; // Agregamos el campo ID
    char titulo[100];
    char autor[100];
    int anio;
    char genero[100];
    char descripcion[200];
    int cantidad;
    char estado;
};

/**
 * Funcion para actualizar la base de datos de los libros 
*/
int actualizaBaseLibros() {
    FILE *archivo = fopen("../data/registro.txt", "r+");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    struct Libro libros[100];
    int contador = 0;
    char linea[1000];

    while (fgets(linea, sizeof(linea), archivo)) {
        char *token = strtok(linea, "|");
        libros[contador].id = contador + 1; // Asignamos el ID autoincrementable
        strcpy(libros[contador].titulo, token);

        token = strtok(NULL, "|");
        strcpy(libros[contador].autor, token);

        token = strtok(NULL, "|");
        libros[contador].anio = atoi(token);

        token = strtok(NULL, "|");
        strcpy(libros[contador].genero, token);

        token = strtok(NULL, "|");
        strcpy(libros[contador].descripcion, token);

        token = strtok(NULL, "|");
        libros[contador].cantidad = atof(token);

        contador++;
    }

    fclose(archivo);

    FILE *json_file = fopen("../data/libros.json", "w");
    if (json_file == NULL) {
        perror("No se pudo crear el archivo JSON");
        return 1;
    }

    fprintf(json_file, "[\n");
    for (int i = 0; i < contador; i++) {
        fprintf(json_file, "  {\n");
        fprintf(json_file, "    \"id\": %d,\n", libros[i].id);
        fprintf(json_file, "    \"titulo\": \"%s\",\n", libros[i].titulo);
        fprintf(json_file, "    \"autor\": \"%s\",\n", libros[i].autor);
        fprintf(json_file, "    \"anio\": %d,\n", libros[i].anio);
        fprintf(json_file, "    \"genero\": \"%s\",\n", libros[i].genero);
        fprintf(json_file, "    \"descripcion\": \"%s\",\n", libros[i].descripcion);
        fprintf(json_file, "    \"cantidad\": %d,\n", libros[i].cantidad);
        fprintf(json_file, "    \"estado\": true\n");
        if (i == contador - 1) {
            fprintf(json_file, "  }\n");
        } 
        else {
            fprintf(json_file, "  },\n");
        }
    }
    fprintf(json_file, "]\n");

    fclose(json_file);

    printf ("\n\nSe Actualizo la base de datos de libros registrados\n\n");

    return 0;
}


/**
 * Función para buscar libros por texto en un archivo JSON.
 *
 * @param textoBuscado El texto que se desea buscar en los libros.
 * @return 0 si la búsqueda fue exitosa, 1 en caso de error al abrir el archivo JSON, 2 en caso de error al analizar el archivo JSON.
 */
int buscarLibroPorTexto(const char *textoBuscado) {
    // Abrir el archivo JSON de libros en modo lectura
    FILE *json_file = fopen("../data/libros.json", "r");
    if (json_file == NULL) {
        perror("No se pudo abrir el archivo JSON");
        return 1; // Retorna 1 en caso de error al abrir el archivo
    }

    // Obtener el tamaño del archivo JSON
    fseek(json_file, 0, SEEK_END);
    long file_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);

    // Leer el contenido del archivo JSON en una cadena
    char *json_string = (char *)malloc(file_size + 1);
    fread(json_string, 1, file_size, json_file);
    json_string[file_size] = '\0';

    // Analizar la cadena JSON para crear una estructura cJSON
    cJSON *root = cJSON_Parse(json_string);

    // Manejar errores de análisis JSON
    if (root == NULL) {
        perror("Error al analizar el archivo JSON");
        fclose(json_file);
        free(json_string);
        return 2; // Retorna 2 en caso de error en el análisis JSON
    }

    cJSON *libroObj;
    bool huboCoincidencias = false; // Variable para rastrear coincidencias

    // Recorrer cada libro en la estructura cJSON
    cJSON_ArrayForEach(libroObj, root) {
        struct Libro libro;

        // Obtener los atributos del libro desde la estructura cJSON
        libro.id = cJSON_GetObjectItemCaseSensitive(libroObj, "id")->valueint;
        strcpy(libro.titulo, cJSON_GetObjectItemCaseSensitive(libroObj, "titulo")->valuestring);
        strcpy(libro.autor, cJSON_GetObjectItemCaseSensitive(libroObj, "autor")->valuestring);
        strcpy(libro.descripcion, cJSON_GetObjectItemCaseSensitive(libroObj, "descripcion")->valuestring);
        libro.estado = cJSON_GetObjectItemCaseSensitive(libroObj, "estado")->valueint;

        // Convertir a minúsculas y buscar coincidencias en los atributos del libro
        char *titulo_lower = strlwr(strdup(libro.titulo));
        char *autor_lower = strlwr(strdup(libro.autor));
        char *descripcion_lower = strlwr(strdup(libro.descripcion));
        char *textoBuscado_lower = strlwr(strdup(textoBuscado));

        // Comprobar si el texto buscado está contenido en los atributos del libro
        if (strstr(titulo_lower, textoBuscado_lower) || strstr(autor_lower, textoBuscado_lower) || strstr(descripcion_lower, textoBuscado_lower)) {
            printf("\n------------------------------");
            printf("\nLibro encontrado");
            printf("\nID: %d\n", libro.id);
            printf("Título: %s\n", libro.titulo);
            printf("Descripción: %s\n", libro.descripcion);
            printf("Estado: %s\n", libro.estado ? "Disponible" : "No disponible");
            printf("\n");
            huboCoincidencias = true; // Se encontró una coincidencia
        }

        free(titulo_lower);
        free(autor_lower);
        free(descripcion_lower);
        free(textoBuscado_lower);
    }

    // Cerrar el archivo JSON, liberar memoria y mostrar mensaje si no hubo coincidencias
    fclose(json_file);
    cJSON_Delete(root);
    free(json_string);

    if (!huboCoincidencias) {
        printf("\n\n\t\t---- No hay coincidencias con la búsqueda. ----\n");
    }

    return 0; // Retorna 0 al finalizar con éxito
}