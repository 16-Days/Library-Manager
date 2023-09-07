#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"
//#include "manageUsers.h"
#include <time.h>

// Estructura para representar un préstamo
struct Loan {
    int loanId; // Identificador único del préstamo
    char userId[20]; // Cédula del usuario
    int bookId; // Identificador del ejemplar
    char startDate[11]; // Fecha de inicio (formato: dd/mm/yyyy)
    char endDate[11]; // Fecha de entrega (formato: dd/mm/yyyy)
};



// Declaración de funciones externas definidas en otros archivos
cJSON *cargarLibrosDesdeArchivo(const char *archivo);
void marcarEjemplarNoDisponible(int bookId);
// En prestamo.h (o en un archivo de encabezado separado)
void agregarPrestamoAPrestamosJson(const struct Loan *loan);
// En prestamo.h (o en un archivo de encabezado separado)
void realizarDevolucionEjemplar(int loanId, const char *returnDate);
// En prestamo.h (o en un archivo de encabezado separado)
cJSON *buscarPrestamoPorId(int loanId);
// En prestamo.h (o en un archivo de encabezado separado)
float calcularMontoACancelar(const cJSON *prestamoObj, const char *returnDate);
// En prestamo.h (o en un archivo de encabezado separado)
void eliminarPrestamoPorId(int loanId);

// Declaración de funciones
void guardarLastLoanId();
void cargarLastLoanId();






// Variable global para llevar un seguimiento del último loanId utilizado
static int lastLoanId = 0;

// Ruta al archivo de configuración para almacenar lastLoanId
const char *configFilePath = "../data/config.txt";

// Función para guardar el último loanId en un archivo de configuración
void guardarLastLoanId() {
    FILE *configFile = fopen(configFilePath, "w");
    if (configFile != NULL) {
        fprintf(configFile, "%d", lastLoanId);
        fclose(configFile);
    }
}


// Función para cargar el último loanId desde un archivo de configuración
void cargarLastLoanId() {
    FILE *configFile = fopen(configFilePath, "r");
    if (configFile != NULL) {
        fscanf(configFile, "%d", &lastLoanId);
        fclose(configFile);
    } else {
        // Si el archivo no existe, inicializa lastLoanId a un valor predeterminado
        lastLoanId = 0;
        guardarLastLoanId(); // guarda el valor inicial en el archivo
    }
}


// Función para obtener el próximo ID de préstamo disponible
int obtenerProximoLoanId() {
    // Cargar el último loanId al iniciar el programa
    cargarLastLoanId();

    // Incrementar el loanId y guardar el nuevo valor
    int nextLoanId = ++lastLoanId;
    guardarLastLoanId();

    return nextLoanId;
}




// Función para obtener la fecha actual en formato "dd/mm/yyyy"
char *obtenerFechaActual() {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    char *buffer = (char *)malloc(11); // "dd/mm/yyyy" + null-terminator
    if (buffer != NULL) {
        strftime(buffer, 11, "%d/%m/%Y", tm_info);
    }

    return buffer;
}



// Función para actualizar la cantidad de un ejemplar y marcarlo como no disponible si es necesario
bool actualizarCantidadYVerificarEstado(int bookId) {
    cJSON *root = cargarLibrosDesdeArchivo("../data/libros.json");
    if (root == NULL) {
        return false; // Error al cargar los libros
    }

    cJSON *libroObj;
    cJSON_ArrayForEach(libroObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(libroObj, "id")->valueint;
        if (id == bookId) {
            cJSON *cantidadObj = cJSON_GetObjectItemCaseSensitive(libroObj, "cantidad");

            int cantidad = cantidadObj->valueint;

            if (cantidad <= 0) {
                cJSON_Delete(root);
                return false; // El ejemplar no está disponible
            }

            cantidadObj->valueint = cantidad - 1; // Reducir la cantidad

            if (cantidad - 1 == 0) {
                marcarEjemplarNoDisponible(bookId); // Marcar como no disponible
            }

            // Guardar los cambios en el archivo "libros.json"
            FILE *json_file = fopen("../data/libros.json", "w");
            if (json_file != NULL) {
                char *json_str = cJSON_Print(root);
                fputs(json_str, json_file);
                fclose(json_file);
                free(json_str);
            }

            cJSON_Delete(root);
            return true; // Ejemplar disponible y cantidad actualizada
        }
    }

    cJSON_Delete(root);
    return false; // El libro no se encontró en la lista
}


// Función para verificar si un usuario existe en la lista de usuarios por cédula
bool existeUsuarioPorCedula(const cJSON *usuarios, const char *cedula) {
    cJSON *usuarioObj;
    cJSON_ArrayForEach(usuarioObj, usuarios) {
        const char *cedulaUsuario = cJSON_GetObjectItemCaseSensitive(usuarioObj, "cedula")->valuestring;
        if (strcmp(cedulaUsuario, cedula) == 0) {
            return true; // El usuario existe en la lista de usuarios
        }
    }
    return false; // El usuario no se encontró en la lista de usuarios
}


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
    loan.loanId = obtenerProximoLoanId(); // Un contador autoincremental 
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

    // Verificar si el usuario existe
    cJSON *usuarios = cargarUsuariosDesdeArchivo("../data/usuarios.json");
    if (usuarios == NULL) {
        printf("Error al cargar la lista de usuarios.\n");
        return;
    }

    if (!existeUsuarioPorCedula(usuarios, userId)) {
        printf("El usuario con cédula %s no existe.\n", userId);
        cJSON_Delete(usuarios);
        return;
    }



    printf("Ingrese el identificador del ejemplar: ");
    scanf("%d", &bookId);

    // Verificar la disponibilidad del ejemplar
    if (!isBookAvailable(bookId)) {
        printf("El ejemplar no está disponible o no existe.\n");
        return;
    }

    // Actualizar la cantidad y marcar el ejemplar como no disponible si es necesario
    if (actualizarCantidadYVerificarEstado(bookId)) {
        printf("Ingrese la fecha de inicio (dd/mm/yyyy): ");
        scanf("%s", startDate);
        printf("Ingrese la fecha de entrega (dd/mm/yyyy): ");
        scanf("%s", endDate);

        // Generar el comprobante de préstamo
        struct Loan loan = generarComprobantePrestamo(userId, bookId, startDate, endDate);

        // Mostrar el comprobante al usuario
        printf("\nComprobante de Préstamo:\n");
        printf("ID de Préstamo: %d\n", loan.loanId);
        printf("Cédula del Usuario: %s\n", loan.userId);
        printf("ID del Ejemplar: %d\n", loan.bookId);
        printf("Fecha de Inicio: %s\n", loan.startDate);
        printf("Fecha de Entrega: %s\n", loan.endDate);

        // Guardar el comprobante de préstamo (puedes implementar esta parte)
        agregarPrestamoAPrestamosJson(&loan);
    } else {
        printf("El ejemplar no está disponible.\n");
    }
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


// Función para cargar la lista de préstamos desde el archivo JSON de préstamos
cJSON *cargarPrestamosDesdeArchivo(const char *rutaArchivo) {
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

// Función para guardar la lista de préstamos en el archivo JSON de préstamos
void guardarPrestamosEnArchivo(const char *rutaArchivo, cJSON *root) {
    FILE *fp = fopen(rutaArchivo, "w");
    if (fp != NULL) {
        char *json = cJSON_Print(root);
        fputs(json, fp);
        fclose(fp);
        free(json);
    }
}

// Función para agregar un préstamo al archivo prestamos.json
void agregarPrestamoAPrestamosJson(const struct Loan *loan) {
    cJSON *root = NULL;
    
    // Especifica la ruta completa al archivo JSON de préstamos
    const char *rutaArchivo = "../data/prestamos.json";

    // Carga la lista de préstamos existentes desde el archivo JSON
    root = cargarPrestamosDesdeArchivo(rutaArchivo);

    if (root == NULL) {
        // Si el archivo JSON no existe o está vacío, crea un nuevo objeto JSON
        root = cJSON_CreateArray();
    }

    // Crear un objeto JSON para el nuevo préstamo y agregarlo a la lista
    cJSON *prestamo = cJSON_CreateObject();
    cJSON_AddNumberToObject(prestamo, "loanId", loan->loanId);
    cJSON_AddStringToObject(prestamo, "userId", loan->userId);
    cJSON_AddNumberToObject(prestamo, "bookId", loan->bookId);
    cJSON_AddStringToObject(prestamo, "startDate", loan->startDate);
    cJSON_AddStringToObject(prestamo, "endDate", loan->endDate);
    cJSON_AddItemToArray(root, prestamo);

    // Guardar la lista actualizada de préstamos en el archivo JSON
    guardarPrestamosEnArchivo(rutaArchivo, root);
    cJSON_Delete(root); // Liberar la memoria
}



// Función para devolver un ejemplar
void devolverEjemplar() {
    int loanId;
    char returnDate[11];

    // Solicitar la entrada del usuario
    printf("Ingrese el identificador de préstamo: ");
    scanf("%d", &loanId);
    printf("Ingrese la fecha de devolución (dd/mm/yyyy): ");
    scanf("%s", returnDate);

    realizarDevolucionEjemplar(loanId, returnDate);
}



// Función para realizar la devolución de un ejemplar
void realizarDevolucionEjemplar(int loanId, const char *returnDate) {
    // Realizar las operaciones de devolución aquí
    cJSON *prestamoObj = buscarPrestamoPorId(loanId);

    if (prestamoObj == NULL) {
        printf("El préstamo con ID %d no se encontró.\n", loanId);
        return;
    }

    // Obtener el ID del ejemplar del préstamo
    int bookId = cJSON_GetObjectItemCaseSensitive(prestamoObj, "bookId")->valueint;

    // Verificar si el ejemplar está marcado como no disponible (estado `false`)
    cJSON *root = cargarLibrosDesdeArchivo("../data/libros.json");
    if (root == NULL) {
        printf("Error al cargar los libros.\n");
        return;
    }

    cJSON *libroObj;
    cJSON_ArrayForEach(libroObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(libroObj, "id")->valueint;
        if (id == bookId) {
            bool estado = cJSON_GetObjectItemCaseSensitive(libroObj, "estado")->valueint;
            if (!estado) {
                // Marcar el ejemplar como disponible solo si está como no disponible
                cJSON_GetObjectItemCaseSensitive(libroObj, "estado")->valueint = 1;
                break;
            }
        }
    }

    // Guardar los cambios en el archivo "libros.json"
    FILE *json_file = fopen("../data/libros.json", "w");
    if (json_file != NULL) {
        char *json_str = cJSON_Print(root);
        fputs(json_str, json_file);
        fclose(json_file);
        free(json_str);
    }

    cJSON_Delete(root);

    // Calcular el monto a cancelar
    float montoACancelar = calcularMontoACancelar(prestamoObj, returnDate);

    // Mostrar el monto a cancelar al usuario
    printf("Monto a cancelar: %.2f\n", montoACancelar);

    // Eliminar el préstamo del archivo "prestamos.json"
    eliminarPrestamoPorId(loanId);

    printf("Devolución exitosa. El ejemplar ha sido marcado como disponible y el préstamo ha sido eliminado.\n");
}





// Función para buscar un préstamo por su identificador en el archivo "prestamos.json"
cJSON *buscarPrestamoPorId(int loanId) {
    cJSON *root = cargarPrestamosDesdeArchivo("../data/prestamos.json");
    if (root == NULL) {
        return NULL; // Error al cargar los préstamos
    }

    cJSON *prestamoObj;
    cJSON_ArrayForEach(prestamoObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(prestamoObj, "loanId")->valueint;
        if (id == loanId) {
            return prestamoObj; // Devuelve el objeto JSON del préstamo encontrado
        }
    }

    cJSON_Delete(root);
    return NULL; // El préstamo no se encontró en la lista
}

// Función para calcular el monto a cancelar en función de la duración del préstamo y la fecha de devolución
float calcularMontoACancelar(const cJSON *prestamoObj, const char *returnDate) {
    // Obtener la fecha de inicio y la fecha de entrega del préstamo desde el objeto JSON
    const char *startDate = cJSON_GetObjectItemCaseSensitive(prestamoObj, "startDate")->valuestring;
    const char *endDate = cJSON_GetObjectItemCaseSensitive(prestamoObj, "endDate")->valuestring;

    // Parsear las fechas en formato "dd/mm/yyyy"
    int startDay, startMonth, startYear, endDay, endMonth, endYear, returnDay, returnMonth, returnYear;
    sscanf(startDate, "%d/%d/%d", &startDay, &startMonth, &startYear);
    sscanf(endDate, "%d/%d/%d", &endDay, &endMonth, &endYear);
    sscanf(returnDate, "%d/%d/%d", &returnDay, &returnMonth, &returnYear);

    // Calcular la diferencia en días entre la fecha de inicio y la fecha de devolución
    int daysDifference = (returnYear - startYear) * 365 + (returnMonth - startMonth) * 30 + (returnDay - startDay);

    // Calcular el monto a cancelar en función de la duración del préstamo
    float montoACancelar = 0.0;

    if (daysDifference >= 1 && daysDifference <= 7) {
        montoACancelar = daysDifference * 150.0; // Tarifa para préstamo de 1 a 7 días
    } else if (daysDifference >= 8 && daysDifference <= 15) {
        montoACancelar = daysDifference * 125.0; // Tarifa para préstamo de 8 a 15 días
    } else if (daysDifference >= 16) {
        montoACancelar = daysDifference * 100.0; // Tarifa para préstamo de 16 días o más
    }

    return montoACancelar;
}


// Función para marcar un ejemplar como disponible
void marcarEjemplarDisponible(int bookId) {
    cJSON *root = cargarLibrosDesdeArchivo("../data/libros.json");
    if (root == NULL) {
        return; // Error al cargar los libros
    }

    cJSON *libroObj;
    cJSON_ArrayForEach(libroObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(libroObj, "id")->valueint;
        if (id == bookId) {
            cJSON_GetObjectItemCaseSensitive(libroObj, "estado")->valueint = 1; // Marcar como disponible
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


// Función para eliminar un préstamo del archivo "prestamos.json" por loanId
void eliminarPrestamoPorId(int loanId) {
    cJSON *root = cargarPrestamosDesdeArchivo("../data/prestamos.json");
    if (root == NULL) {
        return; // Error al cargar los préstamos
    }

    cJSON *prestamoObj;
    cJSON *next = NULL;

    cJSON_ArrayForEach(prestamoObj, root) {
        int id = cJSON_GetObjectItemCaseSensitive(prestamoObj, "loanId")->valueint;
        if (id == loanId) {
            next = prestamoObj->next; // Obtener el siguiente elemento antes de eliminar
            cJSON_Delete(prestamoObj); // Eliminar el objeto JSON del préstamo
            break;
        }
    }

    if (next != NULL) {
        root->child = next; // Actualizar el puntero al primer elemento si era el primero
    }

    // Guardar la lista actualizada de préstamos en el archivo JSON
    guardarPrestamosEnArchivo("../data/prestamos.json", root);
    cJSON_Delete(root);
}

// Función para verificar si un préstamo está vencido o próximo a vencer
void verificarVencimientoPrestamos() {
    cJSON *prestamos = cargarPrestamosDesdeArchivo("../data/prestamos.json");
    if (prestamos == NULL) {
        printf("Error al cargar la lista de préstamos.\n");
        return;
    }

    char fechaActual[11];
    strcpy(fechaActual, obtenerFechaActual());

    cJSON *prestamoObj;
    cJSON_ArrayForEach(prestamoObj, prestamos) {
        const char *endDate = cJSON_GetObjectItemCaseSensitive(prestamoObj, "endDate")->valuestring;

        // Parsear la fecha de entrega en formato "dd/mm/yyyy"
        int endDay, endMonth, endYear;
        sscanf(endDate, "%d/%d/%d", &endDay, &endMonth, &endYear);

        // Parsea la fecha actual en formato "dd/mm/yyyy"
        int currentDay, currentMonth, currentYear;
        sscanf(fechaActual, "%d/%d/%d", &currentDay, &currentMonth, &currentYear);

        // Calcular la diferencia en días entre la fecha actual y la fecha de entrega
        int daysDifference = (endYear - currentYear) * 365 + (endMonth - currentMonth) * 30 + (endDay - currentDay);

        if (daysDifference >= 0 && daysDifference <= 3) {
            // El préstamo está vencido o próximo a vencer
            int loanId = cJSON_GetObjectItemCaseSensitive(prestamoObj, "loanId")->valueint;
            const char *userId = cJSON_GetObjectItemCaseSensitive(prestamoObj, "userId")->valuestring;
            printf("Préstamo vencido o próximo a vencer:\n");
            printf("ID de Préstamo: %d\n", loanId);
            printf("Cédula del Usuario: %s\n", userId);
            printf("Fecha de Entrega: %s\n", endDate);
        }
    }

    cJSON_Delete(prestamos);
    free(fechaActual); // Liberar la memoria asignada a fechaActual
}
