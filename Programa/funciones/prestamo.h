#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"
//#include "manageCatalog.h"



// Declaración de funciones externas definidas en otros archivos
cJSON *cargarLibrosDesdeArchivo(const char *archivo);
void marcarEjemplarNoDisponible(int bookId);

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


// Función para cargar los libros desde un archivo JSON
cJSON *cargarLibrosDesdeArchivo(const char *archivo) {
    FILE *json_file = fopen(archivo, "r");
    if (json_file == NULL) {
        perror("No se pudo abrir el archivo JSON");
        return NULL;
    }

    fseek(json_file, 0, SEEK_END);
    long file_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);

    char *json_string = (char *)malloc(file_size + 1);
    fread(json_string, 1, file_size, json_file);
    json_string[file_size] = '\0';

    cJSON *root = cJSON_Parse(json_string);

    fclose(json_file);
    free(json_string);

    return root;
}

// Función para marcar un ejemplar como no disponible
void marcarEjemplarNoDisponible(int bookId) {
    cJSON *root = cargarLibrosDesdeArchivo("../data/libros.json");
    if (root == NULL) {
        return; // Error al cargar los libros
    }

    cJSON *libroObj;
    cJSON_ArrayForEach(libroObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(libroObj, "id")->valueint;
        if (id == bookId) {
            cJSON_GetObjectItemCaseSensitive(libroObj, "estado")->valueint = 0; // Marcar como no disponible
            break;
        }
    }

    FILE *json_file = fopen("../data/libros.json", "w");
    if (json_file != NULL) {
        char *json_str = cJSON_Print(root);
        fputs(json_str, json_file);
        fclose(json_file);
        free(json_str);
    }

    cJSON_Delete(root);
}
