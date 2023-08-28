#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de una estructura para almacenar los datos
struct Libro {
    char titulo[100];
    char autor[100];
    int anio;
    char genero[100];
    char descripcion[200];
    float calificacion;
};

int esDuplicado(struct Libro *libros, int contador, const char *titulo) {
    for (int i = 0; i < contador; i++) {
        if (strcmp(libros[i].titulo, titulo) == 0) {
            return 1; // Libro encontrado, es un duplicado
        }
    }
    return 0; // No se encontró un duplicado
}

int actualizaBaseLibros() {
    // Abrir el archivo para lectura
    FILE *archivo = fopen("../data/registro.txt", "r+");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    // Crear un arreglo de estructuras para almacenar los libros
    struct Libro libros[100]; // Puedes ajustar el tamaño según la cantidad de libros en tu archivo

    int contador = 0;
    char linea[1000]; // Asumiendo que cada línea tendrá menos de 1000 caracteres

    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), archivo)) {
        // Utilizar strtok para dividir la línea en campos separados por '|'
        char *token = strtok(linea, "|");
        strcpy(libros[contador].titulo, token);

        // ... (código para el resto de los campos)

        contador++;
    }

    // Cerrar el archivo
    fclose(archivo);

    // ... (código para crear el archivo JSON)

    FILE *json_file = fopen("../data/libros.json", "w");
    if (json_file == NULL) {
        perror("No se pudo crear el archivo JSON");
        return 1;
    }

    fprintf(json_file, "[\n");
    for (int i = 0; i < contador; i++) {
        if (!esDuplicado(libros, i, libros[i].titulo)) { // Verificar si es un duplicado
            fprintf(json_file, "  {\n");
            fprintf(json_file, "    \"titulo\": \"%s\",\n", libros[i].titulo);
            fprintf(json_file, "    \"autor\": \"%s\",\n", libros[i].autor);
            fprintf(json_file, "    \"anio\": %d,\n", libros[i].anio);
            fprintf(json_file, "    \"genero\": \"%s\",\n", libros[i].genero);
            fprintf(json_file, "    \"descripcion\": \"%s\",\n", libros[i].descripcion);
            fprintf(json_file, "    \"calificacion\": %.1f,\n", libros[i].calificacion);
            fprintf(json_file, "    \"estado\": true\n"); // Agregar la etiqueta "estado" con valor "true"
            if (i == contador - 1) {
                fprintf(json_file, "  }\n");
            } else {
                fprintf(json_file, "  },\n");
            }
        }
    }
    fprintf(json_file, "]\n");

    // Cerrar el archivo JSON
    fclose(json_file);

    printf ("\n\nSe Actualizo la base de datos de libros registrados\n\n");

    return 0;
}

