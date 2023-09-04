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


//Seccion prestamo

// Estructura para representar un préstamo
struct Loan {
    int loanId; // Identificador único del préstamo
    char userId[20]; // Cédula del usuario
    int bookId; // Identificador del ejemplar
    char startDate[11]; // Fecha de inicio (formato: dd/mm/yyyy)
    char endDate[11]; // Fecha de entrega (formato: dd/mm/yyyy)
};

// Función para verificar si un ejemplar está disponible
bool isBookAvailable(int bookId) {
    cJSON *root = cargarLibrosDesdeArchivo("../data/libros.json");
    if (root == NULL) {
        return false; // Error al cargar los libros
    }

    cJSON *libroObj;
    cJSON_ArrayForEach(libroObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(libroObj, "id")->valueint;
        if (id == bookId) {
            bool estado = cJSON_GetObjectItemCaseSensitive(libroObj, "estado")->valueint;
            cJSON_Delete(root);
            return estado; // Devuelve true si el libro está disponible
        }
    }

    cJSON_Delete(root);
    return false; // El libro no se encontró en la lista
}

// Función para generar un comprobante de préstamo
struct Loan generarComprobantePrestamo(const char *userId, int bookId, const char *startDate, const char *endDate) {
    struct Loan loan;
    loan.loanId = 1; // Podría implementarse como un contador autoincremental en el futuro
    strcpy(loan.userId, userId);
    loan.bookId = bookId;
    strcpy(loan.startDate, startDate);
    strcpy(loan.endDate, endDate);
    return loan;
}

// Función para realizar un préstamo de ejemplar
void prestarEjemplar() {
    char userId[20];
    int bookId;
    char startDate[11];
    char endDate[11];

    // Solicitar la entrada del usuario
    printf("Ingrese su cédula: ");
    scanf("%s", userId);
    printf("Ingrese el identificador del ejemplar: ");
    scanf("%d", &bookId);
    printf("Ingrese la fecha de inicio (dd/mm/yyyy): ");
    scanf("%s", startDate);
    printf("Ingrese la fecha de entrega (dd/mm/yyyy): ");
    scanf("%s", endDate);

    // Verificar la disponibilidad del ejemplar
    if (!isBookAvailable(bookId)) {
        printf("El ejemplar no está disponible o no existe.\n");
        return;
    }

    // Generar el comprobante de préstamo
    struct Loan loan = generarComprobantePrestamo(userId, bookId, startDate, endDate);

    // Mostrar el comprobante al usuario
    printf("\nComprobante de Préstamo:\n");
    printf("ID de Préstamo: %d\n", loan.loanId);
    printf("Cédula del Usuario: %s\n", loan.userId);
    printf("ID del Ejemplar: %d\n", loan.bookId);
    printf("Fecha de Inicio: %s\n", loan.startDate);
    printf("Fecha de Entrega: %s\n", loan.endDate);

    // Marcar el ejemplar como no disponible
    marcarEjemplarNoDisponible(bookId);

    // Guardar el comprobante de préstamo (puedes implementar esta parte)
}