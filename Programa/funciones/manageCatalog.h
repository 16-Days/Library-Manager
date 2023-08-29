#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Libro {
    int id; // Agregamos el campo ID
    char titulo[100];
    char autor[100];
    int anio;
    char genero[100];
    char descripcion[200];
    int cantidad;
};

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


